#include "stdafx.h"
#include "game.h"

using namespace std;

int Util::AC = 0;

static Desks casino;


void Util::testMsg(bool subType, int64_t desknum, int64_t playNum, const char * str) {
	int index = casino.desks[0]->currentPlayIndex;
	casino.game(subType, desknum, playNum + index, str);
}


void Util::sendGroupMsg(int64_t groupid, const char *msg) {
#ifdef _DEBUG  
	string aa = msg;
	cout << "群发：" << aa << endl;
#else
	CQ_sendGroupMsg(Util::AC, groupid, msg);
#endif
}


void Util::sendDiscussMsg(int64_t groupid, const char *msg) {
#ifdef _DEBUG  
	string aa = msg;
	cout << "群发：" << aa << endl;
#else
	CQ_sendDiscussMsg(Util::AC, groupid, msg);
#endif
}

void Util::sendPrivateMsg(int64_t number, const char* msg) {
#ifdef _DEBUG  
	string aa = msg;
	cout << "私聊" << number << "：" << aa << endl;
#else
	CQ_sendPrivateMsg(Util::AC, number, msg);
#endif
}


//将string转换成wstring  
wstring Util::string2wstring(string str)
{
	wstring result;
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //添加字符串结尾  
									//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}

void Util::mkdir()
{
	CreateDirectory(CONFIG_DIR.c_str(), NULL);
}

//将wstring转换成string  
string Util::wstring2string(wstring wstr)
{
	string result;
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//删除缓冲区并返回值  
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
	this->lastPlayIndex = -1;//当前谁出得牌
	this->currentPlayIndex = -1;//该谁出牌
	this->bossIndex = -1;//谁是地主

	vector<wstring> lastCard;//上位玩家的牌
	this->lastCardType = L"";//上位玩家得牌类
	this->lastWeights = new vector<int>;//上位玩家的牌

	this->whoIsWinner = 0;
	this->multiple = 1;
	this->turn = 0;

}

Player::Player() {
	this->isReady = false;
	this->isOpenCard = false;
	this->isSurrender = false;
}
void Player::sendMsg()
{
	wstring tmp = this->msg.str();
	if (tmp.empty()) {
		this->msg.str(L"");
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

void Desk::listPlayers(int type)
{

	bool hasType = (type & 1) == 1;
	bool hasWin = ((type >> 1) & 1) == 1;

	int score = CONFIG_BOTTOM_SCORE* this->multiple;
	int halfScore = score / 2;
	this->msg << L"积分倍数：" << this->multiple;
	this->breakLine();
	this->msg << L"出牌次数(不计算过牌)：" << this->turn;
	this->breakLine();

	for (unsigned i = 0; i < this->players.size(); i++) {
		this->msg << i + 1 << L":";
		if (hasType) {
			this->msg << L"[" << (i == this->bossIndex && state == STATE_GAMEING ? L"地主" : L"农民") << L"]";
		}

		this->msg << L"[CQ:at,qq=" << this->players[i]->number << L"]";
		if (hasWin) {
			if (this->whoIsWinner == 2) {//如果是农民赢了
				if (i == this->bossIndex) {
					this->msg << L"[" << L"失败-" << score << L"分]";
					Admin::addScore(this->players[i]->number, -score);
				}
				else if (this->players[i]->isSurrender) {
					this->msg << L"[" << L"弃牌+" << 0 << L"分]";
				}
				else {
					this->msg << L"[" << L"胜利+" << halfScore << L"分]";
					Admin::addScore(this->players[i]->number, halfScore);
				}
			}
			else {
				if (i == this->bossIndex) {
					this->msg << L"[" << L"胜利+" << score << L"分]";
					Admin::addScore(this->players[i]->number, score);
				}
				else if (this->players[i]->isSurrender) {
					this->msg << L"[" << L"弃牌-" << score << L"分]";
					Admin::addScore(this->players[i]->number, -score);
				}
				else {
					this->msg << L"[" << L"失败-" << halfScore << L"分]";
					Admin::addScore(this->players[i]->number, -halfScore);
				}
			}
		}

		this->breakLine();
	}
}

bool Desk::isCanWin(int cardCount, vector<int> *weights, wstring type)
{

	if (type == L"" || this->lastCardType == L"王炸") {
		return false;
	}

	if (this->lastCardType == L"") {
		return true;
	}

	if (type == L"王炸") {
		return true;
	}
	if (type == L"炸弹" && type != this->lastCardType) {
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

	if (cardCount == 2 && Util::findFlag(list[0]) + Util::findFlag(list[1]) == 27) {//王炸
		return L"王炸";
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

	int max = counts[0];//存放大值
	int min = counts[0];//存放小值
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
	if (cardCount == 1) {//单牌
		weights->push_back(Util::findFlag(cards[0]));
		return L"单牌";
	}
	if (cardCount == 2 && max == 2) {//对子
		weights->push_back(Util::findFlag(cards[0]));
		return L"对子";
	}
	if (cardCount == 3 && max == 3) {//三张
		weights->push_back(Util::findFlag(cards[0]));
		return L"三张";
	}
	if (cardCount == 4 && max == 4) {//炸弹
		weights->push_back(Util::findFlag(cards[0]));
		return L"炸弹";
	}

	if (cardCount == 4 && max == 3) {//3带1

		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"3带1";
	}

	if (cardCount == 5 && max == 3 && min == 2) {//3带2
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"3带2";
	}

	if (cardCount == 6 && max == 4) {//4带2
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"4带2";
	}

	if (cardGroupCout > 2 && max == 2 && min == 2
		&& Util::findFlag(cards[0]) == Util::findFlag(cards[cardGroupCout - 1]) - cardGroupCout + 1
		&& Util::findFlag(cards[cardGroupCout - 1]) < 13
		) {//连对
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"连对";
	}

	if (cardGroupCout > 4 && max == 1 && min == 1
		&& Util::findFlag(cards[0]) == Util::findFlag(cards[cardGroupCout - 1]) - cardGroupCout + 1
		&& Util::findFlag(cards[cardGroupCout - 1]) < 13
		) {//顺子
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"顺子";
	}

	//飞机
	int  planeCount = 0;
	for (unsigned i = 0; i < counts.size(); i++) {
		if (counts[i] >= 3) {
			planeCount++;
		}
	}
	if (planeCount > 1) {
		wstring tmp;
		if (cardCount == planeCount * 3) {
			tmp = L"飞机";
		}
		else if (cardCount == planeCount * 4) {
			tmp = L"飞机带翅膀";
		}
		else if (cardCount == planeCount * 5 && min == 2) {
			tmp = L"飞机带双翅膀";
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

void Desk::sendMsg(bool subType)
{
	wstring tmp = this->msg.str();
	if (tmp.empty()) {
		return;
	}
	int length = tmp.length();
	if (tmp[length - 2] == '\r' && tmp[length - 1] == '\n') {
		tmp = tmp.substr(0, length - 2);
	}
	if (subType) {
		Util::sendGroupMsg(this->number, Util::wstring2string(tmp).data());
	}
	else {
		Util::sendDiscussMsg(this->number, Util::wstring2string(tmp).data());
	}

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
	this->msg << L"你是否要抢地主";
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
				<< L"不抢，"
				<< L"[CQ:at,qq=" << this->players[currentPlayIndex]->number << L"] "
				<< L"你是否要抢地主";
		}
	}
}

void Desk::sendBossCard()
{
	Player *playerBoss = players[this->bossIndex];

	this->msg << L"[CQ:at,qq=" << playerBoss->number << L"] "
		<< L"是地主,底牌是："
		<< L"[" << this->cards[53] << L"]"
		<< L"[" << this->cards[52] << L"]"
		<< L"[" << this->cards[51] << L"]"
		<< L"请出牌";
	this->breakLine();

	for (int i = 0; i < 3; i++) {
		playerBoss->card.push_back(cards[53 - i]);
	}
	sort(playerBoss->card.begin(), playerBoss->card.end(), Util::compareCard);

	playerBoss->msg << L"你是地主，收到底牌：";
	for (unsigned m = 0; m < playerBoss->card.size(); m++) {
		playerBoss->msg << L"[" << playerBoss->card.at(m) << L"]";
	}
	playerBoss->breakLine();

	this->state = STATE_SEND_BOSS_CARD;

}

void Desk::play(int64_t playNum, wstring msg)
{
	int playIndex = this->getPlayer(playNum);
	int length = msg.length();

	if (playIndex == -1 || playIndex != this->currentPlayIndex
		|| (!(this->state == STATE_GAMEING && this->turn > 0)
			&& !(this->state == STATE_SEND_BOSS_CARD && this->turn == 0))
		|| length < 2) {
		return;
	}

	vector<wstring> msglist;

	for (int i = 1; i < length; i++) {
		wstring tmp = msg.substr(i, 1);
		if (tmp == L"1") {
			tmp = msg.substr(i, 2);
			i++;
		}
		msglist.push_back(tmp);
	}

	this->play(msglist, playIndex);

}

void Desk::play(vector<wstring> list, int playIndex)
{

	Player *player = this->players[playIndex];
	vector<wstring> mycardTmp(player->card);

	int cardCount = list.size();

	for (int i = 0; i < cardCount; i++) {
		if (Util::findAndRemove(mycardTmp, list[i]) == -1) {
			this->msg << L"你就没有你出的牌，会不会玩？";
			return;
		}
	}

	vector<int> *weights = new vector<int>;

	wstring type = this->getMycardType(list, weights);

	bool isCanWin = this->isCanWin(cardCount, weights, type);

	if (isCanWin) {

		if (this->turn == 0) {
			this->state = STATE_GAMEING;
		}

		player->card = mycardTmp;
		this->lastWeights = weights;
		this->lastCard = list;
		this->lastCardType = type;
		this->lastPlayIndex = this->currentPlayIndex;
		this->turn++;


		//处理积分
		if (type == L"王炸") {
			this->multiple += 2;
		}
		else if (type == L"炸弹") {
			this->multiple += 1;
		}

		if (mycardTmp.size() == 0) {//赢了。
			this->whoIsWinner = this->bossIndex == this->currentPlayIndex ? 1 : 2;
			this->msg << L"游戏结束";
			this->breakLine();
			this->listPlayers(3);

			this->msg << (this->whoIsWinner == 1 ? L"地主赢了" : L"农民赢了");
			casino.gameOver(this->number);
			return;
		}

		player->listCards();

		if (player->isOpenCard) {
			this->at(player->number);
			this->msg << L"明牌：";
			this->listCardsOnDesk(player);
			this->breakLine();
		}

		if (player->card.size() < 3) {
			this->at(player->number);
			this->msg << L"仅剩下" << player->card.size() << L"张牌";
			this->breakLine();
		}

		this->msg << L"上回合：" << this->lastCardType;
		for (unsigned m = 0; m < this->lastCard.size(); m++) {
			this->msg << L"[" << this->lastCard.at(m) << L"]";
		}
		this->breakLine();
		this->setNextPlayerIndex();
		this->at(this->players[this->currentPlayIndex]->number);
		this->msg << L"请出牌";
		this->breakLine();
	}
	else {
		this->msg << L"别瞎几把出牌";
		this->breakLine();
	}
}

void Desk::discard(int64_t playNum)
{
	if (this->currentPlayIndex != this->getPlayer(playNum) || this->state != STATE_GAMEING) {
		return;
	}

	if (this->currentPlayIndex == this->lastPlayIndex) {
		this->msg << L"过过过过你妹，会不会玩，你不能过牌了";
		return;
	}


	this->msg << L"上回合：" << this->lastCardType;
	for (unsigned m = 0; m < this->lastCard.size(); m++) {
		this->msg << L"[" << this->lastCard.at(m) << L"]";
	}
	this->breakLine();
	this->msg << L"上位玩家：过牌";
	this->breakLine();
	this->setNextPlayerIndex();

	this->at(this->players[this->currentPlayIndex]->number);
	this->msg << L"请出牌";
	this->breakLine();
}

void Desk::surrender(int64_t playNum)
{
	int index = this->getPlayer(playNum);
	if (index == -1 || this->state != STATE_GAMEING || this->players[index]->isSurrender) {
		return;
	}

	Player *player = this->players[index];

	player->isSurrender = true;

	if (index == this->bossIndex) {
		this->whoIsWinner = 2;//农民赢
	}
	else {
		for (size_t i = 0; i < this->players.size(); i++) {
			if (players[i]->isSurrender && i != this->bossIndex && i != index) {
				this->whoIsWinner = 1;
				break;
			}
		}
	}

	if (this->whoIsWinner > 0) {
		this->msg << L"游戏结束";
		this->breakLine();
		this->listPlayers(3);

		this->msg << (this->whoIsWinner == 1 ? L"地主赢了" : L"农民赢了");
		casino.gameOver(this->number);
		return;
	}


	if (this->currentPlayIndex == index) {
		this->msg << L"上回合：" << this->lastCardType;
		for (unsigned m = 0; m < this->lastCard.size(); m++) {
			this->msg << L"[" << this->lastCard.at(m) << L"]";
		}
		this->breakLine();
		this->msg << L"上位玩家：弃牌";
		this->breakLine();
		this->setNextPlayerIndex();
		this->at(this->players[this->currentPlayIndex]->number);
		this->msg << L"请出牌";
		this->breakLine();
	}
	else {
		this->at(playNum);
		this->msg << L"弃牌";
		this->breakLine();
	}

}

void Desk::openCard(int64_t playNum)
{

	int index = this->getPlayer(playNum);
	if (index == -1 || this->state > STATE_SEND_BOSS_CARD || this->state < STATE_START) {
		return;
	}
	Player *player = this->players[index];

	if (!player->isOpenCard) {
		player->isOpenCard = true;
		this->multiple += 2;
	}

	this->at(playNum);
	this->msg << L"明牌";
	this->breakLine();

	this->listCardsOnDesk(player);
	this->breakLine();


}

void Desk::getPlayerInfo(int64_t playNum)
{
	this->at(playNum);
	this->msg << L"积分：" << Admin::readScore(playNum);
	this->breakLine();
}

void Desk::getScore(int64_t playNum)
{
	this->at(playNum);
	this->msg << (Admin::getScore(playNum) ? L"么么哒给你添加积分啦" : L"又输光想要分？明天来吧");
	this->breakLine();
}

void Desks::gameOver(int64_t number)
{
	int index = casino.getDesk(number);
	if (index == -1) {
		return;
	}
	vector<Desk*>::iterator it = casino.desks.begin() + index;
	casino.desks.erase(it);
	Util::sendGroupMsg(number, "游戏结束");
}

void Desk::exit(int64_t number)
{
	if (this->state == STATE_WAIT) {
		int index = this->getPlayer(number);
		if (index != -1) {
			vector<Player*>::iterator it = this->players.begin() + index;
			this->players.erase(it);
			this->msg << L"退出成功";
			this->breakLine();
		}

	}
	else {
		this->msg << L"游戏已经开始不能退出";
		this->breakLine();
	}
}

void Desk::commandList()
{
	this->msg << L"=    命令列表    =" << "\r\n"
		<< L"命令说明：" << "\r\n"
		<< L"         带有[管]的命令 只能管理员使用，使用前必须先绑定自己为管理员，管理员只有重置游戏后能更改(也可以修改配置)" << "\r\n"
		<< L"         带有[开发中]的命令 还未开发完成" << "\r\n"
		<< L"         带有[B]的命令 是测试功能，可能会更改和加强" << "\r\n"
		<< L"         带有[R]的命令 是正式功能，一般不会做更改" << "\r\n"
		<< L"群聊命令：" << "\r\n"
		<< L"[R]" << L"上桌：加入游戏\r\n"
		<< L"[R]" << L"出：出牌 比如 出23456\r\n"
		<< L"[R]" << L"过|过牌|pass：过牌\r\n"
		<< L"[R]" << L"抢地主|不抢：是否抢地主\r\n"
		<< L"[R]" << L"开始游戏：是否开始游戏\r\n"
		<< L"[R]" << L"下桌：退出游戏，只能在准备环节使用\r\n"
		<< L"[R]" << L"玩家列表：当前在游戏中得玩家信息\r\n"
		<< L"[R]" << L"明牌：显示自己的牌给所有玩家，明牌会导致积分翻倍，只能在发完牌后以及出牌之前使用。\r\n"
		<< L"[R]" << L"弃牌：放弃本局游戏，当地主或者两名农民弃牌游戏结束,弃牌农民玩家赢了不得分，输了双倍扣分" << "\r\n"
		<< L"[R]" << L"获取积分：获取积分，每天可获取2w分。" << "\r\n"
		<< L"[B]" << L"我的信息：我的信息" << "\r\n"
		<< L"私聊命令：" << "\r\n"
		<< L"[管][B]" << L"我是管理：绑定游戏管理员为当前发送消息的qq，管理员可使用管理命令。管理设置后不能更改" << "\r\n"
		<< L"[管][B]" << L"重置斗地主：删除所有配置。重置后可重新设定管理员" << "\r\n"
		<< L"[管][B]" << L"结束游戏[群号]：结束指定群号的游戏，比如：结束游戏123456" << "\r\n"
		<< L"[管][B]" << L"分配积分[qq号]=[积分]：给指定qq分配积分，如：分配积分123456=500"
		;
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

	//如果下一个该出牌的玩家正好弃牌了 则重新set下一位玩家
	//由于不可能大于2个人弃牌 所以下一个人一定没有弃牌
	if (this->players[this->currentPlayIndex]->isSurrender) {
		this->setNextPlayerIndex();
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
		this->msg << L"[CQ:at,qq=" << playNum << L"] 已经加入游戏";
		this->breakLine();
		return;
	}

	if (this->players.size() == 3) {
		this->msg << L"[CQ:at,qq=" << playNum << L"] 人数已满";
		this->breakLine();
		return;
	}

	if (Admin::readScore(playNum) < 1) {
		this->msg << L"[CQ:at,qq=" << playNum << L"] 没积分了你";
		this->breakLine();
		return;
	}

	Player *player = new Player;
	player->number = playNum;
	this->players.push_back(player);

	this->msg << L"[CQ:at,qq=" << playNum << L"] "
		<< L"加入成功，已有玩家:" << this->players.size() << L"个";
	this->breakLine();
	if (this->players.size() == 3) {
		this->msg << L"请输入[开始游戏]";
		this->breakLine();
	}
}

Desk* Desks::getOrCreatDesk(int64_t deskNum) {

	Desk *desk = NULL;
	int deskIndex = getDesk(deskNum);
	if (deskIndex == -1) {//没有桌子
		desk = new Desk;
		desk->number = deskNum;
		desks.push_back(desk);
	}
	else {
		desk = casino.desks[deskIndex];
	}

	return desk;
}
void Desk::startGame() {
	if (this->players.size() == 3 && this->state == STATE_WAIT) {
		this->state = STATE_START;
		this->msg << L"游戏开始";
		this->breakLine();

		this->listPlayers(1);

		this->shuffle();

		this->deal();

		this->creataBoss();
	}
	else {
		this->msg << L"没有足够的玩家或者已经开始游戏";
		this->breakLine();
		this->listPlayers(1);
	}
}

bool Desks::game(bool subType, int64_t deskNum, int64_t playNum, const char* msgArray) {

	string tmp = msgArray;

	wstring msg = Util::string2wstring(tmp);
	Util::trim(msg);
	Util::toUpper(msg);

	Desk *desk = casino.getOrCreatDesk(deskNum);

	if (msg == L"上桌" || msg == L"JOIN") {
		desk->join(playNum);
	}
	else if (msg.find(L"出") == 0) {//出牌阶段
		desk->play(playNum, msg);
	}
	else if (msg == L"过" || msg == L"过牌" || msg == L"不出" || msg == L"要不起" || msg == L"不要" || msg == L"PASS") {//跳过出牌阶段
		desk->discard(playNum);
	}
	else if (msg == L"退出游戏" || msg == L"退桌" || msg == L"下桌") {//结束游戏
		desk->exit(playNum);
	}
	else if (msg == L"命令列表") {
		desk->commandList();
	}
	else if (msg == L"玩家列表") {
		desk->listPlayers(1);
	}
	else if (msg == L"开始游戏") {
		desk->startGame();
	}
	else if (msg == L"抢地主" || msg == L"抢") {
		desk->getBoss(playNum);
	}
	else if (msg == L"不抢") {
		desk->dontBoss(playNum);
	}
	else if (msg == L"明牌") {
		desk->openCard(playNum);
	}
	else if (msg == L"弃牌") {
		desk->surrender(playNum);
	}
	else if (msg == L"记牌器") {
		desk->msg << L"未开发完成";
	}
	else if (msg == L"我的信息") {
		desk->getPlayerInfo(playNum);
	}
	else if (msg == L"获取积分") {
		desk->getScore(playNum);
	}
	else {
		return false;
	}

	desk->sendMsg(subType);
	desk->sendPlayerMsg();
	return true;
}

bool Desks::game(int64_t playNum, const char * msgArray)
{
	string tmp = msgArray;

	wstring msg = Util::string2wstring(tmp);
	Util::trim(msg);
	Util::toUpper(msg);


	bool result;
	if (msg == L"我是管理") {
		result = Admin::IAmAdmin(playNum);
	}
	else if (msg == L"重置斗地主" || msg == L"初始化斗地主") {
		result = Admin::resetGame(playNum);
	}
	else if (regex_match(msg, allotReg)) {
		result = Admin::allotScoreTo(msg, playNum);
	}
	else if (msg.find(L"结束游戏") == 0) {//结束游戏
		result = Admin::gameOver(msg, playNum);
	}
	else {
		return false;
	}

	msg = result ? L"操作成功" : L"操作失败";
	Util::sendPrivateMsg(playNum, Util::wstring2string(msg).data());
	return true;
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


int64_t Admin::readAdmin()
{
	wstring model = L"admin";
	wstring key = L"admin";
	return GetPrivateProfileInt(model.c_str(), key.c_str(), 0, CONFIG_PATH.c_str());
}

bool Admin::isAdmin(int64_t playNum)
{
	return playNum == Admin::readAdmin();
}

wstring Admin::readString() {
	WCHAR tmp[15];
	GetPrivateProfileString(L"admin", L"admin", L"", tmp, 15, CONFIG_PATH.c_str());
	return wstring(tmp);
}

bool Admin::allotScoreTo(wstring msg, int64_t playNum)
{


	int score;
	int64_t playerNum;

	wsmatch mr;
	wstring::const_iterator src_it = msg.begin(); // 获取起始位置
	wstring::const_iterator src_end = msg.end(); // 获取结束位置
	regex_search(src_it, src_end, mr, numberReg);
	wstringstream ss;
	ss << mr[0].str();
	ss >> playerNum;
	ss.str(L"");
	src_it = mr[0].second;
	regex_search(src_it, src_end, mr, numberReg);
	wstringstream scoress;
	scoress << mr[0].str();
	scoress >> score;
	scoress.str(L"");

	return Admin::isAdmin(playNum) && Admin::writeScore(playerNum, score < 0 ? 0 : score);
}

bool Admin::gameOver(wstring msg, int64_t playNum)
{
	if (Admin::isAdmin(playNum)) {
		wstringstream ss;
		ss << msg.substr(4, msg.size());
		int64_t destNum;
		ss >> destNum;
		ss.str(L"");
		casino.gameOver(destNum);
		return true;
	}
	return false;
}

bool Admin::writeAdmin(int64_t playerNum)
{
	wstring model = L"admin";
	wstring key = L"admin";
	wstringstream ss;
	ss << playerNum;
	wstring value = ss.str();
	ss.str(L"");
	return WritePrivateProfileString(model.c_str(), key.c_str(), value.c_str(), CONFIG_PATH.c_str());

}

int64_t Admin::readScore(int64_t playerNum)
{
	wstring model = L"score";
	wstringstream ss;
	ss << playerNum;
	wstring key = ss.str();
	ss.str(L"");
	return GetPrivateProfileInt(model.c_str(), key.c_str(), 0, CONFIG_PATH.c_str());
}

bool Admin::getScore(int64_t playerNum)
{
	wstring model = L"time";
	wstringstream ss;
	ss << playerNum;
	wstring key = ss.str();
	ss.str(L"");
	int64_t lastGetScoreTime = GetPrivateProfileInt(model.c_str(), key.c_str(), 0, CONFIG_PATH.c_str());
	time_t rawtime;
	int64_t now = time(&rawtime);

	if (now > lastGetScoreTime + 24 * 60 * 60) {
		Admin::addScore(playerNum, CONIFG_INIT_SCORE);
		ss << now;
		wstring value = ss.str();
		ss.str(L"");
		return WritePrivateProfileString(model.c_str(), key.c_str(), value.c_str(), CONFIG_PATH.c_str());
	}
	return false;
}

bool Admin::writeScore(int64_t playerNum, int64_t score)
{
	wstring model = L"score";
	wstringstream ss;
	ss << playerNum;
	wstring key = ss.str();
	ss.str(L"");
	ss << score;
	wstring value = ss.str();
	ss.str(L"");
	return WritePrivateProfileString(model.c_str(), key.c_str(), value.c_str(), CONFIG_PATH.c_str());
}

bool Admin::addScore(int64_t playerNum, int score)
{
	int64_t hasScore = Admin::readScore(playerNum) + score;
	return  Admin::writeScore(playerNum, hasScore < 0 ? 0 : hasScore);
}

bool Admin::IAmAdmin(int64_t playerNum)
{
	return  Admin::readAdmin() == 0 && Admin::writeAdmin(playerNum);
}

bool Admin::resetGame(int64_t playNum)
{
	return playNum == Admin::readAdmin() && DeleteFile(CONFIG_PATH.c_str());
}
