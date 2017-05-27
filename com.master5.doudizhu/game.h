
#include "stdafx.h"
#include "string"
#ifdef _DEBUG 
#else
#include "cqp.h"
#endif 
#include <time.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
using namespace std;
static const string  cardDest[54] = {
	"��","��",
	"2","3","4","5","6","7","8","9","10","J","Q","K","A",
	"2","3","4","5","6","7","8","9","10","J","Q","K","A",
	"2","3","4","5","6","7","8","9","10","J","Q","K","A",
	"2","3","4","5","6","7","8","9","10","J","Q","K","A"
	
	//"2","3","3","3","4","4","4","5","5","5","6","6","7","7","8","8","9","10","J","Q","K","A","��","��",
	//"2","9","10","J","Q","K","A",
	//"2","6","7","8","9","10","J","Q","K","A",
	//"2","3","4","5","6","7","8","9","10","J","Q","K","A",
};

static const string flag[15] = { "3","4","5","6","7","8","9","10","J","Q","K","A","2", "��","��" };

const int STATE_WAIT = 0;
const int STATE_START = 1;
const int STATE_BOSSING = 2;
const int STATE_GAMEING = 3;


class Util{
public:
	static int AC;
	static void testMsg(int64_t playNum, int64_t desknum, const char * str);
	static void sendGroupMsg(int64_t groupid, const char *msg);
	static void sendPrivateMsg(int64_t groupid, const char *msg);
	static int  findAndRemove(vector<string> &dest, string str);
	static int  find(vector<string> &dest, string str);
	static int  findFlag(string str);
	static int  desc(int a, int b);
	static int  asc(int a, int b);
    static bool  compareCard(const string &carda, const string &cardb);
	static void  trim(string &s);
	static void  toUpper(string &str);
	static void setAC(int32_t ac);
};

class Player
{
public:
	Player();
	stringstream msg;
	int64_t number;
	vector<string> card;
	int32_t socre;
	bool isReady;
	bool isOpenCard;//����״̬
	bool isSurrender;//Ͷ��״̬

	void sendMsg();
	void listCards();

	void breakLine();
};



class Desk {
public:

	Desk();
	string cards[54];
	int64_t number;
	vector<Player*> players;

	int state;
	int lastPlayIndex;//��ǰ˭������
	int nextPlayIndex;//��˭����
	int bossIndex;//˭�ǵ���

	vector<string> lastCard;//��λ��ҵ���
	string lastCardType;//��λ��ҵ�����
	vector<int> *lastWeights;//��λ��ҵ���

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
	void surrender(int64_t playNum);//Ͷ��
	void openCard(int64_t playNum);//����

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
	void listCardsOnDesk(Player* player);

};

class Desks {
public:
	vector<Desk*> desks;
	Desk* getOrCreatDesk(int64_t deskNum);
	static void game(int64_t deskNum, int64_t playNum, const char *msg);
	int getDesk(int64_t deskNum);
};