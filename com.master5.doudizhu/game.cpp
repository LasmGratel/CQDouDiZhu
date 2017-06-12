#include "stdafx.h"
#include "game.h"
using namespace std;

int Util::AC = 0;

static Desks datas;

 void Util::testMsg(int64_t desknum, int64_t playNum,const char * str) {
	int index = datas.desks[0]->currentPlayIndex;
	datas.game(desknum, playNum + index, str);
}


void Util::sendGroupMsg(int64_t groupid, const char *msg) {
#ifdef _DEBUG  
	string aa = msg;
	cout << "Ⱥ����" << aa << endl;
#else
	CQ_sendGroupMsg(Util::AC, groupid, msg);
#endif
}

void Util::sendPrivateMsg(int64_t number, const char* msg) {
#ifdef _DEBUG  
	string aa = msg;
	cout << "˽�ģ�" << aa << endl;
#else
	CQ_sendPrivateMsg(Util::AC, number, msg);
#endif
}


//��stringת����wstring  
wstring Util::string2wstring(string str)
{
	wstring result;
	//��ȡ��������С��������ռ䣬��������С���ַ�����  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//���ֽڱ���ת���ɿ��ֽڱ���  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //����ַ�����β  
									//ɾ��������������ֵ  
	result.append(buffer);
	delete[] buffer;
	return result;
}

//��wstringת����string  
string Util::wstring2string(wstring wstr)
{
	string result;
	//��ȡ��������С��������ռ䣬��������С�°��ֽڼ����  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//���ֽڱ���ת���ɶ��ֽڱ���  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//ɾ��������������ֵ  
	result.append(buffer);
	delete[] buffer;
	return result;
}

int Util::findAndRemove(vector<wstring> &dest, wstring str) {

	for (unsigned i = 0; i < dest.size(); i++) {
		if (dest.at(i) == str) {
			vector<wstring>::iterator it = dest.begin() + i;
			dest.erase(it);
			return i;
		}
	}
	return -1;
}

int Util::find(vector<wstring> &dest, wstring str) {

	for (unsigned i = 0; i < dest.size(); i++) {
		if (dest.at(i) == str) {
			return i;
		}
	}
	return -1;
}

int Util::findFlag(wstring str)
{

	for (int i = 0; i < 15; i++) {
		if (flag[i] == str) {
			return i;
		}
	}
	return -1;

}

int Util::desc(int a, int b)
{
	return a > b;
}

int Util::asc(int a, int b)
{
	return a < b;
}

bool Util::compareCard(const wstring &carda, const wstring &cardb)
{
	return findFlag(carda) < findFlag(cardb);
}


void Util::trim(wstring &s)
{

	int index = 0;
	if (!s.empty())
	{
		while ((index = s.find(' ', index)) != wstring::npos)
		{
			s.erase(index, 1);
		}
	}

}

void Util::toUpper(wstring &str) {
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void Util::setAC(int32_t ac)
{
	Util::AC = ac;
}


Desk::Desk() {
	for (int i = 0; i < 54; i++) {
		this->cards[i] = cardDest[i];
	}

	this->state = 0;
	this->lastPlayIndex = -1;//��ǰ˭������
	this->currentPlayIndex = -1;//��˭����
	this->bossIndex = -1;//˭�ǵ���

	vector<wstring> lastCard;//��λ��ҵ���
	this->lastCardType = L"";//��λ��ҵ�����
	this->lastWeights = new vector<int>;//��λ��ҵ���

	this->whoIsWinner = 0;

}

Player::Player() {
	this->socre = 5000;
	this->isReady = false;
	this->isOpenCard = false;
	this->isSurrender = false;
}
void Player::sendMsg()
{
	wstring tmp = this->msg.str();
	if (tmp.empty()) {
		return;
	}
	int length = tmp.length();
	if (tmp[length - 2] == '\r' && tmp[length - 1] == '\n') {
		tmp = tmp.substr(0, length - 2);
	}

	Util::sendPrivateMsg(this->number, Util::wstring2string(tmp).data());
	this->msg.str(L"");
}

void Desk::at(int64_t playNum)
{
	this->msg << L"[CQ:at,qq=" << playNum << L"]";
}

void Desk::breakLine()
{
	this->msg << L"\r\n";
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
	this->msg << L"���:";
	this->breakLine();
	this->listPlayers(1);
}

void Desk::listPlayers(int type)
{

	bool hasType = (type & 1) == 1;
	bool hasWin = ((type >> 1) & 1) == 1;

	for (unsigned i = 0; i < this->players.size(); i++) {
		this->msg << i + 1 << L":";
		if (hasType) {
			this->msg << L"[" << (i == this->bossIndex && state == STATE_GAMEING ? L"����" : L"ũ��") << L"]";
		}

		this->msg << L"[CQ:at,qq=" << this->players[i]->number << L"]";
		if (hasWin) {
			if (this->whoIsWinner==2) {//�����ũ��Ӯ��
				this->msg << L"[" << (i == this->bossIndex ? L"ʧ��" : L"ʤ��") << L"]";
			}
			else {
				this->msg  << L"["  << (i == this->bossIndex ? L"ʤ��" : L"ʧ��")  << L"]";
			}
		}

		this->breakLine();
	}
}

bool Desk::isCanWin(int cardCount, vector<int> *weights, wstring type)
{

	if (type == L"" || this->lastCardType == L"��ը") {
		return false;
	}

	if (this->lastCardType == L"") {
		return true;
	}

	if (type == L"��ը") {
		return true;
	}
	if (type == L"ը��" && type != this->lastCardType) {
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



wstring Desk::getMycardType(vector<wstring> list, vector<int> *weights)
{
	int cardCount = list.size();
	sort(list.begin(), list.end(), Util::compareCard);

	if (cardCount == 2 && Util::findFlag(list[0]) + Util::findFlag(list[1]) == 27) {//��ը
		return L"��ը";
	}

	vector<wstring> cards;
	vector<int> counts;

	for (unsigned i = 0; i < list.size(); i++) {
		int index = Util::find(cards, list[i]);
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
	sort(tmpCount.begin(), tmpCount.end(), Util::desc);
	if (cardCount == 1) {//����
		weights->push_back(Util::findFlag(cards[0]));
		return L"����";
	}
	if (cardCount == 2 && max == 2) {//����
		weights->push_back(Util::findFlag(cards[0]));
		return L"����";
	}
	if (cardCount == 3 && max == 3) {//����
		weights->push_back(Util::findFlag(cards[0]));
		return L"����";
	}
	if (cardCount == 4 && max == 4) {//ը��
		weights->push_back(Util::findFlag(cards[0]));
		return L"ը��";
	}

	if (cardCount == 4 && max == 3) {//3��1

		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"3��1";
	}

	if (cardCount == 5 && max == 3 && min == 2) {//3��2
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"3��2";
	}

	if (cardCount == 6 && max == 4) {//4��2
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"4��2";
	}

	if (cardGroupCout > 2 && max == 2 && min == 2
		&& Util::findFlag(cards[0]) == Util::findFlag(cards[cardGroupCout - 1]) - cardGroupCout + 1
		&& Util::findFlag(cards[cardGroupCout - 1]) < 13
		) {//����
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"����";
	}

	if (cardGroupCout > 4 && max == 1 && min == 1
		&& Util::findFlag(cards[0]) == Util::findFlag(cards[cardGroupCout - 1]) - cardGroupCout + 1
		&& Util::findFlag(cards[cardGroupCout - 1]) < 13
		) {//˳��
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"˳��";
	}

	//�ɻ�
	int  planeCount = 0;
	for (unsigned i = 0; i < counts.size() && counts[i] >= 3; i++, planeCount++);
	if (planeCount > 1) {
		wstring tmp;
		if (cardCount == planeCount * 3) {
			tmp = L"�ɻ�";
		}
		else if (cardCount == planeCount * 4) {
			tmp = L"����ɻ�";
		}
		else if (cardCount == planeCount * 5 && min == 2) {
			tmp = L"˫��ɻ�";
		}

		for (int i = 0; i < planeCount; i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		sort(weights->begin(), weights->end(), Util::desc);

		int weightscount = weights->size();

		if (weights->at(0) - weightscount + 1 != weights->at(weightscount - 1)) {
			return L"";
		}

		return tmp;
	}
	return L"";
}


void Desk::sendMsg()
{

	wstring tmp = this->msg.str();
	if (tmp.empty()) {
		return;
	}
	int length = tmp.length();
	if (tmp[length - 2] == '\r' && tmp[length - 1] == '\n') {
		tmp = tmp.substr(0, length - 2);
	}
	Util::sendGroupMsg(this->number, Util::wstring2string(tmp).data());
	this->msg.str(L"");
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
	this->currentPlayIndex = index;
	this->at(this->players[index]->number);
	this->msg << L"���Ƿ�Ҫ������";
	this->breakLine();
}

void Desk::getBoss(int64_t playerNum)
{
	int index = this->getPlayer(playerNum);
	if (this->state == STATE_BOSSING && this->currentPlayIndex == index) {

		this->bossIndex = index;
		this->currentPlayIndex = index;
		this->lastPlayIndex = index;
		sendBossCard();
	}
}

void Desk::dontBoss(int64_t playerNum)
{
	int index = this->getPlayer(playerNum);
	if (this->state == STATE_BOSSING && this->currentPlayIndex == index) {

		this->setNextPlayerIndex();

		if (this->currentPlayIndex == this->bossIndex) {
			this->sendBossCard();
		}
		else {
			this->msg << L"[CQ:at,qq=" << this->players[index]->number << L"] "
				<< L"������"
				<< L"[CQ:at,qq=" << this->players[currentPlayIndex]->number << L"] "
				<< L"���Ƿ�Ҫ������";
		}
	}
}

void Desk::sendBossCard()
{
	Player *playerBoss = players[this->bossIndex];

	this->msg << L"[CQ:at,qq=" << playerBoss->number << L"] "
		<< L"�ǵ���,�����ǣ�"
		<< L"[" << this->cards[53] << L"]"
		<< L"[" << this->cards[52] << L"]"
		<< L"[" << this->cards[51] << L"]"
		<< L"�����";
	this->breakLine();

	for (int i = 0; i < 3; i++) {
		playerBoss->card.push_back(cards[53 - i]);
	}
	sort(playerBoss->card.begin(), playerBoss->card.end(), Util::compareCard);

	playerBoss->msg << L"���ǵ������յ����ƣ�";
	for (unsigned m = 0; m < playerBoss->card.size(); m++) {
		playerBoss->msg << L"[" << playerBoss->card.at(m) << L"]";
	}
	playerBoss->breakLine();

	state = STATE_GAMEING;
}

void Desk::play(int64_t playNum, wstring msg)
{

	int length = msg.length();
	vector<wstring> msglist;

	for (int i = 1; i < length; i++) {
		wstring tmp = msg.substr(i, 1);
		if (tmp == L"1") {
			tmp=msg.substr(i, 2);
			i++;
		}
		msglist.push_back(tmp);
	}

	this->play(playNum, msglist);

}

void Desk::play(int64_t playNum, vector<wstring> list)
{

	int playIndex = this->getPlayer(playNum);

	if (playIndex == -1 || playIndex != this->currentPlayIndex || this->state != STATE_GAMEING) {
		return;
	}

	Player *player = this->players[playIndex];
	vector<wstring> mycardTmp(player->card);

	int cardCount = list.size();

	for (int i = 0; i < cardCount; i++) {
		if (Util::findAndRemove(mycardTmp, list[i]) == -1) {
			this->msg << L"���û��������ƣ��᲻���棿";
			return;
		}
	}

	vector<int> *weights = new vector<int>;

	wstring type = this->getMycardType(list, weights);

	bool isCanWin = this->isCanWin(cardCount, weights, type);

	if (isCanWin) {

		if (player->card.size() == 0) {//Ӯ�ˡ�
			this->whoIsWinner = this->bossIndex == this->currentPlayIndex ? 1 : 2;
			this->msg << L"��Ϸ����";
			this->breakLine();
			this->listPlayers(3);

			this->msg << (this->whoIsWinner==1 ? L"����Ӯ��" : L"ũ��Ӯ��");
			this->gameOver(this->number);
			return;
		}

		player->card = mycardTmp;
		this->lastWeights = weights;
		this->lastCard = list;
		this->lastCardType = type;
		this->lastPlayIndex = this->currentPlayIndex;

		player->listCards();
		
		if (player->isOpenCard) {
			this->at(player->number);
			this->msg<< L"���ƣ�";
			this->listCardsOnDesk(player);
			this->breakLine();
		}

		if (player->card.size() < 3) {
			this->at(player->number);
			this->msg << L"��ʣ��" << player->card.size() << L"����";
			this->breakLine();
		}

		this->msg << L"�ϻغϣ�" << this->lastCardType;
		for (unsigned m = 0; m < this->lastCard.size(); m++) {
			this->msg << L"[" << this->lastCard.at(m) << L"]";
		}
		this->breakLine();
		this->setNextPlayerIndex();

		//�����һ���ó��Ƶ�������������� ������set��һλ���
		//���ڲ����ܴ���2�������� ������һ����һ��û������
		if (this->players[this->currentPlayIndex]->isSurrender) {
			this->setNextPlayerIndex();
		}

		this->at(this->players[this->currentPlayIndex]->number);
		this->msg << L"�����";
		this->breakLine();
	}
	else {
		this->msg << L"��Ϲ���ѳ���";
		this->breakLine();
	}
}

void Desk::discard(int64_t playNum)
{
	if (this->currentPlayIndex != this->getPlayer(playNum) || this->state != STATE_GAMEING) {
		return;
	}

	if (this->currentPlayIndex == this->lastPlayIndex) {
		this->msg << L"�����������ã��᲻���棬�㲻�ܹ�����";
		return;
	}


	this->msg << L"�ϻغϣ�" << this->lastCardType;
	for (unsigned m = 0; m < this->lastCard.size(); m++) {
		this->msg << L"[" << this->lastCard.at(m) << L"]";
	}
	this->breakLine();
	this->msg << L"��λ��ң�����";
	this->breakLine();
	this->setNextPlayerIndex();
	this->at(this->players[this->currentPlayIndex]->number);
	this->msg << L"�����";
	this->breakLine();
}

void Desk::surrender(int64_t playNum)
{
	int index = this->getPlayer(playNum);
	if (index == -1 || this->state!= STATE_GAMEING || this->players[index]->isSurrender) {
		return;
	}

	Player *player = this->players[index];

	player->isSurrender = true;

	for (size_t i = 0; i < this->players.size();i++) {
		if (players[i]->isSurrender) {
			if (i==this->bossIndex || i!=index) {

				this->whoIsWinner = this->bossIndex == i ? 2 : 1;

				this->msg << L"��Ϸ����";
				this->breakLine();
				this->listPlayers(3);

				this->msg << (this->whoIsWinner ==1 ? L"����Ӯ��" : L"ũ��Ӯ��"  );
				this->gameOver(this->number);
				return;
			}
		}
	}

	if (this->currentPlayIndex == index) {
		this->msg << L"�ϻغϣ�" << this->lastCardType;
		for (unsigned m = 0; m < this->lastCard.size(); m++) {
			this->msg << L"[" << this->lastCard.at(m) << L"]";
		}
		this->breakLine();
		this->msg << L"��λ��ң�����";
		this->breakLine();
		this->setNextPlayerIndex();
		this->at(this->players[this->currentPlayIndex]->number);
		this->msg << L"�����";
		this->breakLine();
	}
	else {
		this->at(playNum);
		this->msg << L"����";
		this->breakLine();
	}

}

void Desk::openCard(int64_t playNum)
{

	int index = this->getPlayer(playNum);
	if (index == -1 || this->state != STATE_GAMEING) {
		return;
	}
	Player *player = this->players[index];

	player->isOpenCard = true;


	this->at(playNum);
	this->msg << L"���ƣ�";
	this->breakLine();

	this->listCardsOnDesk(player);
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
	Util::sendGroupMsg(number, "��Ϸ����");
}

void Desk::exit(int64_t number)
{
	if (this->state == STATE_WAIT) {
		int index = this->getPlayer(number);
		if (index != -1) {
			vector<Player*>::iterator it = this->players.begin() + index;
			this->players.erase(it);
			this->msg << L"�˳��ɹ�";
			this->breakLine();
		}

	}
	else {
		this->msg << L"��Ϸ�Ѿ���ʼ�����˳�";
		this->breakLine();
	}
}

void Desk::commandList()
{
	this->msg << L"=    �����б�    =\r\n"
		<< 1 << L" " << L"������������Ϸ\r\n"
		<< 2 << L" " << L"�������� ���� ��23456\r\n"
		<< 3 << L" " << L"��������\r\n"
		<< 4 << L" " << L"������ | �������Ƿ�������\r\n"
		<< 5 << L" " << L"��ʼ��Ϸ���Ƿ�ʼ��Ϸ\r\n"
		<< 6 << L" " << L"�������˳���Ϸ��ֻ����׼������ʹ��\r\n"
		<< 7 << L" " << L"����б���ǰ����Ϸ�е������Ϣ\r\n"
		<< 8 << L" " << L"����������ʾ�Ѿ���������\r\n"
		<< 9 << L" " << L"����:����������Ϸ����������������ũ��������Ϸ����\r\n"
		<< 10 << L" " << L"������Ϸ:������Ϸ";
	this->breakLine();
}

void Desk::setNextPlayerIndex()
{
	this->currentPlayIndex = this->currentPlayIndex == 2 ? 0 : this->currentPlayIndex + 1;

	if (this->currentPlayIndex == this->lastPlayIndex) {
		this->lastCard.clear();
		this->lastWeights->clear();
		this->lastCardType = L"";
	}


}

void Desk::deal() {
	unsigned i, k, j;

	for (i = k = 0; i < 3; i++) {
		Player *player = players[i];

		for (j = 0; j < 17; j++) {
			player->card.push_back(cards[k++]);
		}

		sort(player->card.begin(), player->card.end(), Util::compareCard);

		for (unsigned m = 0; m < player->card.size(); m++) {
			player->msg << L"[" << player->card.at(m) << L"]";
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
		this->msg << L"[CQ:at,qq=" << playNum << L"] �Ѿ�������Ϸ";
		this->breakLine();
		return;
	}

	if (this->players.size() == 3) {
		this->msg << L"[CQ:at,qq=" << playNum << L"] ��������";
		this->breakLine();
		return;
	}

	Player *player = new Player;
	player->number = playNum;
	this->players.push_back(player);

	this->msg << L"[CQ:at,qq=" << playNum << L"] "
		<< L"����ɹ����������:" << this->players.size() << L"��";
	this->breakLine();
	if (this->players.size() == 3) {
		this->msg << L"������[��ʼ��Ϸ]";
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
		this->msg << L"��Ϸ��ʼ";
		this->breakLine();

		this->listPlayers();

		this->shuffle();

		this->deal();

		this->creataBoss();
	}
	else {
		this->msg << L"û���㹻����һ����Ѿ���ʼ��Ϸ";
		this->breakLine();
		this->listPlayers();
	}
}

void Desks::game(int64_t deskNum, int64_t playNum, const char* msgArray) {

	string tmp= msgArray;

	wstring msg = Util::string2wstring(tmp);
	Util::trim(msg);
	Util::toUpper(msg);

	Desk *desk = datas.getOrCreatDesk(deskNum);

	if (msg == L"����" || msg == L"JOIN") {
		desk->join(playNum);
	}
	else if (msg.find(L"��") == 0 || msg.find(L"����") == 0) {//���ƽ׶�
		desk->play(playNum, msg);
	}
	else if (msg == L"��" || msg == L"����" || msg == L"����" || msg == L"��Ҫ" || msg == L"PASS") {//�������ƽ׶�
		desk->discard(playNum);
	}
	else if (msg == L"�˳���Ϸ" || msg == L"����" || msg == L"����") {//������Ϸ
		desk->exit(playNum);
	}
	else if (msg == L"�����б�") {
		desk->commandList();
	}
	//else if ((msg == "������Ϸ" || msg == "GAMEOVER") && playNum == 895252155) {//������Ϸ
	 else if (msg == L"������Ϸ" || msg == L"GAMEOVER") {//������Ϸ
		desk->gameOver(deskNum);
		return;
	}
	else if (msg == L"����б�") {
		desk->listPlayers();
	}
	else if (msg == L"��ʼ��Ϸ") {
		desk->startGame();
	}
	else if (msg == L"������" || msg == L"��") {
		desk->getBoss(playNum);
	}
	else if (msg == L"����") {
		desk->dontBoss(playNum);
	}
	else if (msg == L"����") {
		desk->openCard(playNum);
	}
	else if (msg == L"����") {
		desk->surrender(playNum);
	}
	else if (msg == L"������") {
		desk->msg << L"δ�������";
	}

	desk->sendMsg();
	desk->sendPlayerMsg();
}

void Player::listCards()
{
	for (unsigned m = 0; m < this->card.size(); m++) {
		this->msg << L"[" << this->card.at(m) << L"]";
	}

}

void Desk::listCardsOnDesk(Player* player)
{
	for (unsigned m = 0; m < player->card.size(); m++) {
		this->msg << L"[" << player->card.at(m) << L"]";
	}
}

void Player::breakLine()
{
	this->msg << L"\r\n";
}
