/*
* CoolQ Demo for VC++
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����
#include <time.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;

int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;


//ssssss


static  string flag[15] = { "3","4","5","6","7","8","9","10","J","Q","K","A","2", "��","��" };

const int STATE_WAIT = 0;
const int STATE_START = 1;
const int STATE_BOSSING = 2;
const int STATE_GAMEING = 2;

int findAndRemove(vector<string> &dest, string str) {

	for (unsigned i = 0; i < dest.size(); i++) {
		if (dest.at(i) == str) {
			vector<string>::iterator it = dest.begin() + i;
			dest.erase(it);
			return i;
		}
	}
	return -1;
}

int find(vector<string> &dest, string str) {

	for (unsigned i = 0; i < dest.size(); i++) {
		if (dest.at(i) == str) {
			return i;
		}
	}
	return -1;
}

int findFlag(string str)
{

	for (int i = 0; i < 15; i++) {
		if (flag[i] == str) {
			return i;
		}
	}
	return -1;

}

int desc(int a, int b)
{
	return a>b;
}

int asc(int a, int b)
{
	return a<b;
}

bool compareCard(const string &carda, const string &cardb)
{
	return findFlag(carda) < findFlag(cardb);
}


void trim(string &s)
{

	int index = 0;
	if (!s.empty())
	{
		while ((index = s.find(' ', index)) != string::npos)
		{
			s.erase(index, 1);
		}
	}

}

void toUpper(string &str) {
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}


class Player
{
public:
	stringstream msg;
	int64_t number;
	vector<string> card;
	int32_t socre = 5000;
	bool isReady = false;

	void sendMsg();
	void listCards();
	void breakLine();
};



class Desk {
public:
	//string cards[54] = {
	//	"10","j","10","j","10","j","4","5","4","5","4","5",
	//	"2","3","6","7","8","9","q","k","a",
	//	"2","3","6","7","8","9","q","k","a",
	//	"2","3","6","7","8","9","q","k","a",
	//	"2","3","4","5","6","7","8","9","10","j","q","k","a",
	//	"��","��"
	//};
	string cards[54] = {
		"2","3","4","5","6","7","8","9","10","J","Q","K","A",
		"2","3","4","5","6","7","8","9","10","J","Q","K","A",
		"2","3","4","5","6","7","8","9","10","J","Q","K","A",
		"2","3","4","5","6","7","8","9","10","J","Q","K","A",
		"��","��"
	};
	int64_t number;
	vector<Player*> players;

	int state = 0;
	int lastPlayIndex = -1;//��ǰ˭������
	int nextPlayIndex = -1;//��˭����
	int bossIndex = -1;//˭�ǵ���

	vector<string> lastCard;//��λ��ҵ���
	string lastCardType = "";//��λ��ҵ�����
	vector<int> *lastWeights = new vector<int>;//��λ��ҵ���

	stringstream msg;

	void join(int64_t playNum);
	void startGame();
	static void gameOver(int64_t number);
	void exit(int64_t playNum);
	void commandList();

	void shuffle();//ϴ��
	void deal();//����
	void creataBoss();//������
	void getBoss(int64_t playerNum);
	void dontBoss(int64_t playerNum);
	void sendBossCard();
	void play(int64_t playNum, string msg);
	void play(int64_t playNum, vector<string> list);//����
	void discard(int64_t playNum);

	void at(int64_t playNum);
	void breakLine();
	int getPlayer(int64_t number);//��qq�Ż����ҵ�����
	void setNextPlayerIndex();//�����¸����Ƶ��������
	void listPlayers();
	void listPlayers(int type);
	bool isCanWin(int cardCount, vector<int> *Weights, string type);
	string getMycardType(vector<string> list, vector<int> *Weights);
	void sendMsg();
	void sendPlayerMsg();

};

class Desks {
public:
	vector<Desk*> desks;
	Desk* getOrCreatDesk(int64_t deskNum);
	void game(int64_t deskNum, int64_t playNum, const char *msg);
	int getDesk(int64_t deskNum);
};


static Desks datas;


void Player::sendMsg()
{
	string tmp = this->msg.str();
	if (tmp.empty()) {
		return;
	}
	int length = tmp.length();
	if (tmp[length - 2] == '\r' && tmp[length - 1] == '\n') {
		tmp = tmp.substr(0, length - 2);
	}
	CQ_sendPrivateMsg(ac, this->number, tmp.data());
	this->msg.str("");
}

void Desk::at(int64_t playNum)
{
	this->msg << "[CQ:at,qq=" << playNum << "]";
}

void Desk::breakLine()
{
	this->msg << "\r\n";
}

int Desk::getPlayer(int64_t number) {
	for (unsigned i = 0; i < players.size(); i++) {
		if (players[i]->number == number) {
			return i;
		}
	}
	return -1;
}


void Desk::listPlayers()
{
	this->msg << "���:";
	this->breakLine();
	this->listPlayers(1);
}

void Desk::listPlayers(int type)
{

	bool hasType = (type & 1) == 1;
	bool hasOwn = ((type >> 1) & 1 )== 1;

	for (unsigned i = 0; i < this->players.size(); i++) {
		this->msg << i + 1 << ":";
		if (hasType) {
			this->msg << "[" << (i == this->bossIndex && state != STATE_GAMEING ? "����" : "ũ��") << "]";
		}

		this->msg << "[CQ:at,qq=" << this->players[i]->number << "]";
		if (hasOwn) {
			if (this->bossIndex == this->nextPlayIndex) {//����ǵ���Ӯ��
				this->msg << "[" << (i == this->bossIndex ? "ʤ��" : "ʧ��") << "]";
			}
			else {
				this->msg << "[" << (i == this->bossIndex ? "ʧ��" : "ʤ��") << "]";
			}
		}

		this->breakLine();
	}
}

bool Desk::isCanWin(int cardCount, vector<int> *weights, string type)
{

	if (type == "" || this->lastCardType == "��ը") {
		return false;
	}

	if (this->lastCardType == "") {
		return true;
	}

	if (type == "��ը") {
		return true;
	}
	if (type == "ը��" && type != this->lastCardType) {
		return true;
	}


	if (type == this->lastCardType && cardCount == this->lastCard.size()) {

		for (unsigned i = 0; i < weights->size(); i++) {
			if (weights[i] > this->lastWeights[i]) {
				return true;
			}
		}

	}


	return false;
}



string Desk::getMycardType(vector<string> list, vector<int> *weights)
{
	int cardCount = list.size();
	sort(list.begin(), list.end(), compareCard);

	if (cardCount == 2 && findFlag(list[0]) + findFlag(list[1]) == 29) {//��ը
		return "��ը";
	}

	vector<string> cards;
	vector<int> counts;

	for (unsigned i = 0; i < list.size(); i++) {
		int index = find(cards, list[i]);
		if (index == -1) {
			cards.push_back(list[i]);
			counts.push_back(1);
		}
		else {
			counts[index] = counts[index] + 1;
		}
	}

	int max = counts[0];//��Ŵ�ֵ
	int min = counts[0];//���Сֵ
	int cardGroupCout = cards.size();
	int tmp;
	for (unsigned i = 0; i < counts.size(); i++) {
		tmp = counts[i];
		if (tmp > max) {
			max = tmp;
		}
		if (tmp < min) {
			min = tmp;
		}
	}

	vector<int> tmpCount(counts);
	sort(tmpCount.begin(), tmpCount.end(), desc);
	if (cardCount == 1) {//����
		weights->push_back(findFlag(cards[0]));
		return "����";
	}
	if (cardCount == 2 && max == 2) {//����
		weights->push_back(findFlag(cards[0]));
		return "����";
	}
	if (cardCount == 3 && max == 3) {//����
		weights->push_back(findFlag(cards[0]));
		return "����";
	}
	if (cardCount == 4 && max == 4) {//ը��
		weights->push_back(findFlag(cards[0]));
		return "ը��";
	}

	if (cardCount == 4 && max == 3) {//3��1

		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return "3��1";
	}

	if (cardCount == 5 && max == 3 && min == 2) {//3��2
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return "3��2";
	}

	if (cardCount == 6 && max == 4) {//4��2
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return "4��2";
	}

	if (cardGroupCout > 2 && max == 2 && min == 2
		&& findFlag(cards[0]) == findFlag(cards[cardGroupCout - 1]) - cardGroupCout + 1
		&& findFlag(cards[cardGroupCout - 1]) < 13
		) {//����
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return "����";
	}

	if (cardGroupCout > 4 && max == 1 && min == 1
		&& findFlag(cards[0]) == findFlag(cards[cardGroupCout - 1]) - cardGroupCout + 1
		&& findFlag(cards[cardGroupCout - 1]) < 13
		) {//˳��
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return "˳��";
	}

	//�ɻ�
	int  planeCount = 0;
	for (unsigned i = 0; i < counts.size() && counts[i] >= 3; i++, planeCount++);
	if (planeCount>1) {
		string tmp;
		if (cardCount == planeCount * 3) {
			tmp = "�ɻ�";
		}
		else if (cardCount == planeCount * 4) {
			tmp = "����ɻ�";
		}
		else if (cardCount == planeCount * 5 && min == 2) {
			tmp = "˫��ɻ�";
		}

		for (int i = 0; i < planeCount; i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		sort(weights->begin(), weights->end(), desc);

		int weightscount = weights->size();

		if (weights->at(0) - weightscount + 1 != weights->at(weightscount - 1)) {
			return "";
		}

		return tmp;
	}
	return "";
}





void Desk::sendMsg()
{

	string tmp = this->msg.str();
	if (tmp.empty()) {
		return;
	}
	int length = tmp.length();
	if (tmp[length - 2] == '\r' && tmp[length - 1] == '\n') {
		tmp = tmp.substr(0, length - 2);
	}
	CQ_sendGroupMsg(ac, this->number, tmp.data());
	this->msg.str("");
}

void Desk::sendPlayerMsg()
{
	for (unsigned i = 0; i < this->players.size(); i++) {
		players[i]->sendMsg();
	}
}



void Desk::shuffle() {
	srand((unsigned)time(NULL));
	for (unsigned i = 0; i < 54; i++) {
		swap(this->cards[i], this->cards[rand() % 54]);
	}
}

void Desk::creataBoss() {

	state = STATE_BOSSING;

	int index = rand() % 3;

	this->bossIndex = index;
	this->nextPlayIndex = index;
	this->at(this->players[index]->number);
	this->msg << "���Ƿ�Ҫ������";
	this->breakLine();
}

void Desk::getBoss(int64_t playerNum)
{
	int index = this->getPlayer(playerNum);
	if (this->state == STATE_BOSSING && this->nextPlayIndex == index) {

		this->bossIndex = index;
		sendBossCard();
	}
}

void Desk::dontBoss(int64_t playerNum)
{
	int index = this->getPlayer(playerNum);
	if (this->state == STATE_BOSSING && this->nextPlayIndex == index) {

		this->setNextPlayerIndex();

		if (this->nextPlayIndex == this->bossIndex) {
			this->sendBossCard();
		}
		else {
			this->msg << "[CQ:at,qq=" << this->players[index]->number << "] "
				<< "������"
				<< "[CQ:at,qq=" << this->players[nextPlayIndex]->number << "] "
				<< "���Ƿ�Ҫ������";
		}
	}
}

void Desk::sendBossCard()
{
	Player *playerBoss = players[this->bossIndex];

	this->msg << "[CQ:at,qq=" << playerBoss->number << "] "
		<< "�ǵ��������ǣ�"
		<< "[" << this->cards[53] << "]"
		<< "[" << this->cards[52] << "]"
		<< "[" << this->cards[51] << "]"
		<< "�����";
	this->breakLine();

	for (int i = 0; i < 3; i++) {
		playerBoss->card.push_back(cards[53 - i]);
	}
	sort(playerBoss->card.begin(), playerBoss->card.end(), compareCard);

	playerBoss->msg << "���ǵ������յ����ƣ�";
	for (unsigned m = 0; m < playerBoss->card.size(); m++) {
		playerBoss->msg << "[" << playerBoss->card.at(m) << "]";
	}
	playerBoss->breakLine();

	state = STATE_GAMEING;
}

void Desk::play(int64_t playNum, string msg)
{

	int length = msg.length();
	char *msgs = new char[length];
	strncpy(msgs, msg.data(), length);
	vector<string> msglist;

	stringstream data;
	string msgFindTmp;
	for (int i = 2; i < length; i++) {

		data << msgs[i];
		msgFindTmp = msgs;
		int gIndex = msgFindTmp.find("��");
		int wIndex = msgFindTmp.find("��");
		int tenIndex = msgFindTmp.find("10");
		if (i == gIndex || i == wIndex || i == tenIndex) {
			data << msgs[i + 1];
			msgs[i++] = -1;
			msgs[i] = -1;
		}

		msglist.push_back(data.str());
		data.str("");
	}

	this->play(playNum, msglist);

}

void Desk::play(int64_t playNum, vector<string> list)
{

	int playIndex = this->getPlayer(playNum);

	if (playIndex == -1 || playIndex != this->nextPlayIndex || this->state != STATE_GAMEING) {
		return;
	}
	Player *player = this->players[playIndex];
	vector<string> mycardTmp(player->card);

	int cardCount = list.size();

	for (int i = 0; i < cardCount; i++) {
		if (findAndRemove(mycardTmp, list[i]) == -1) {
			this->msg << "���û��������ƣ��᲻���棿";
			return;
		}
	}

	vector<int> *weights = new vector<int>;

	string type = this->getMycardType(list, weights);

	bool isCanWin = this->isCanWin(cardCount, weights, type);

	if (isCanWin) {
		if (mycardTmp.size() == 0) {//Ӯ�ˡ�
			this->msg << "��Ϸ����";
			this->breakLine();
			this->listPlayers(3);

			this->msg << (this->bossIndex == this->nextPlayIndex ? "����Ӯ��" : "ũ��Ӯ��");
			this->gameOver(this->number);
			return;
		}
		player->card = mycardTmp;
		this->lastWeights = weights;
		this->lastCard = list;
		this->lastCardType = type;
		this->lastPlayIndex = this->nextPlayIndex;


		player->listCards();

		if (player->card.size() < 3) {
			this->at(this->lastPlayIndex);
			this->msg << "��ʣ��" << player->card.size() << "����";
			this->breakLine();
		}

		this->msg << "�ϻغϣ�" << this->lastCardType;
		for (unsigned m = 0; m < this->lastCard.size(); m++) {
			this->msg << "[" << this->lastCard.at(m) << "]";
		}
		this->breakLine();
		this->setNextPlayerIndex();
		this->at(this->players[this->nextPlayIndex]->number);
		this->msg << "�����";
		this->breakLine();
	}
	else {
		this->msg << "��Ϲ���ѳ���";
		this->breakLine();
	}
}

void Desk::discard(int64_t playNum)
{
	if (this->nextPlayIndex != this->getPlayer(playNum) || this->state != STATE_GAMEING) {
		return;
	}

	if (this->nextPlayIndex == this->lastPlayIndex) {
		this->msg << "�����������ã��᲻���棬�㲻�ܹ�����";
		return;
	}


	this->msg << "�ϻغϣ�" << this->lastCardType;
	for (unsigned m = 0; m < this->lastCard.size(); m++) {
		this->msg << "[" << this->lastCard.at(m) << "]";
	}
	this->breakLine();
	this->msg << "��λ��ң�����";
	this->breakLine();
	this->setNextPlayerIndex();
	this->at(this->players[this->nextPlayIndex]->number);
	this->msg << "�����";
	this->breakLine();
}

void Desk::gameOver(int64_t number)
{
	int index = datas.getDesk(number);
	if (index == -1) {
		return;
	}
	vector<Desk*>::iterator it = datas.desks.begin() + index;
	datas.desks.erase(it);
	CQ_sendGroupMsg(ac, number, "��Ϸ����");
}

void Desk::exit(int64_t number)
{
	if (this->state == STATE_WAIT) {
		int index = this->getPlayer(number);
		if (index != -1) {
			vector<Player*>::iterator it = this->players.begin() + index;
			this->players.erase(it);
			this->msg << "�˳��ɹ�";
			this->breakLine();
		}

	}
	else {
		this->msg << "��Ϸ�Ѿ���ʼ�����˳�";
		this->breakLine();
	}
}

void Desk::commandList()
{
	int i = 1;
	this->msg
		<< i++ << " "<< "������������Ϸ"
		<< i++ << " " << "�������� ���� ��23456"
		<< i++ << " " << "��������"
		<< i++ << " " << "������ | �������Ƿ�������"
		<< i++ << " " << "��ʼ��Ϸ���Ƿ�ʼ��Ϸ"
		<< i++ << " " << "�������˳���Ϸ��ֻ����׼������ʹ��"
		<< i++ << " " << "����б���ǰ����Ϸ�е������Ϣ"
		<< i++ << " " << "����������ʾ�Ѿ���������";
	this->breakLine();
}

void Desk::setNextPlayerIndex()
{
	this->nextPlayIndex = this->nextPlayIndex == 2 ? 0 : this->nextPlayIndex + 1;

	if (this->nextPlayIndex == this->lastPlayIndex) {
		this->lastCard.clear();
		this->lastWeights->clear();
		this->lastCardType = "";
	}


}

void Desk::deal() {
	unsigned i, k, j;

	for (i = k = 0; i < 3; i++) {
		Player *player = players[i];

		for (j = 0; j < 17; j++) {
			player->card.push_back(cards[k++]);
		}

		sort(player->card.begin(), player->card.end(), compareCard);

		for (unsigned m = 0; m < player->card.size(); m++) {
			player->msg << "[" << player->card.at(m) << "]";
		}

		player->breakLine();
	}

}

int Desks::getDesk(int64_t deskNum) {

	for (unsigned i = 0; i < this->desks.size(); i++) {
		if (this->desks[i]->number == deskNum) {
			return i;
		}
	}

	return -1;
}

void Desk::join(int64_t playNum)
{

	int playIndex = this->getPlayer(playNum);

	if (playIndex != -1) {
		this->msg << "[CQ:at,qq=" << playNum << "] �Ѿ�������Ϸ";
		this->breakLine();
		return;
	}

	if (this->players.size() == 3) {
		this->msg << "[CQ:at,qq=" << playNum << "] ��������";
		this->breakLine();
		return;
	}

	Player *player = new Player;
	player->number = playNum;
	this->players.push_back(player);

	this->msg << "[CQ:at,qq=" << playNum << "] "
		<< "����ɹ����������:" << this->players.size() << "��";
	this->breakLine();
	if (this->players.size() == 3) {
		this->msg << "������[��ʼ��Ϸ]";
		this->breakLine();
	}
}

Desk* Desks::getOrCreatDesk(int64_t deskNum) {

	Desk *desk = NULL;
	int deskIndex = getDesk(deskNum);
	if (deskIndex == -1) {//û������
		desk = new Desk;
		desk->number = deskNum;
		desks.push_back(desk);
	}
	else {
		desk = datas.desks[deskIndex];
	}

	return desk;
}
void Desk::startGame() {
	if (this->players.size() == 3 && this->state == STATE_WAIT) {
		this->state = STATE_START;
		this->msg << "��Ϸ��ʼ";
		this->breakLine();

		this->listPlayers();

		this->shuffle();

		this->deal();

		this->creataBoss();
	}
	else {
		this->msg << "û���㹻����һ����Ѿ���ʼ��Ϸ";
		this->breakLine();
		this->listPlayers();
	}
}

void Desks::game(int64_t deskNum, int64_t playNum, const char* msgArray) {

	string msg = msgArray;
	trim(msg);
	toUpper(msg);

	Desk *desk = datas.getOrCreatDesk(deskNum);

	if (msg == "����" || msg == "������Ϸ" || msg=="JOIN") {
		desk->join(playNum);
	}
	else if (msg.find("��") == 0 || msg.find("����") == 0) {//���ƽ׶�
		desk->play(playNum, msg);
	}
	else if (msg == "��" || msg == "����" || msg == "����" || msg == "��Ҫ" || msg=="PASS") {//�������ƽ׶�
		desk->discard(playNum);
	}
	else if (msg == "�˳���Ϸ" || msg == "����" || msg == "����") {//������Ϸ
		desk->exit(playNum);
	}
	else if (msg == "�����б�") {
		desk->commandList();
	}
	else if ((msg == "������Ϸ" || msg=="GAMEOVER") && playNum == 895252155) {//������Ϸ
		desk->gameOver(deskNum);
		return;
	}
	else if (msg == "����б�") {
		desk->listPlayers();
	}
	else if (msg == "��ʼ��Ϸ") {
		desk->startGame();
	}
	else if (msg == "������" || msg == "��") {
		desk->getBoss(playNum);
	}
	else if (msg == "����") {
		desk->dontBoss(playNum);
	}
	else if (msg=="������") {
		desk->msg << "δ�������";
	}

	desk->sendMsg();
	desk->sendPlayerMsg();
}

void Player::listCards()
{
	for (unsigned m = 0; m < this->card.size(); m++) {
		this->msg << "[" << this->card.at(m) << "]";
	}
	this->breakLine();

}

void Player::breakLine()
{
	this->msg << "\r\n";
}





/*
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}



/*
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 ˽����Ϣ
* subType �����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {

	//���Ҫ�ظ���Ϣ������ÿ�Q�������ͣ��������� return EVENT_BLOCK - �ضϱ�����Ϣ�����ټ�������  ע�⣺Ӧ�����ȼ�����Ϊ"���"(10000)ʱ������ʹ�ñ�����ֵ
	//������ظ���Ϣ������֮���Ӧ��/�������������� return EVENT_IGNORE - ���Ա�����Ϣ
	return EVENT_IGNORE;
}


/*
* Type=2 Ⱥ��Ϣ
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {

	datas.game(fromGroup, fromQQ, msg);

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}



/*
* Type=4 ��������Ϣ
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=101 Ⱥ�¼�-����Ա�䶯
* subType �����ͣ�1/��ȡ������Ա 2/�����ù���Ա
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=102 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/ȺԱ�뿪 2/ȺԱ���� 3/�Լ�(����¼��)����
* fromQQ ������QQ(��subTypeΪ2��3ʱ����)
* beingOperateQQ ������QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=103 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/����Ա��ͬ�� 2/����Ա����
* fromQQ ������QQ(������ԱQQ)
* beingOperateQQ ������QQ(����Ⱥ��QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=201 �����¼�-���������
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=301 ����-�������
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=302 ����-Ⱥ���
* subType �����ͣ�1/����������Ⱥ 2/�Լ�(����¼��)������Ⱥ
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

/*
* �˵������� .json �ļ������ò˵���Ŀ��������
* �����ʹ�ò˵������� .json ���˴�ɾ�����ò˵�
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "����menuA�����������봰�ڣ����߽�������������", "", 0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "����menuB�����������봰�ڣ����߽�������������", "", 0);
	return 0;
}


