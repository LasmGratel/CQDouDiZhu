#include "stdafx.h"
#include "../com.master5.doudizhu/game.h"
#include <fstream>
using namespace std;

void testGame() {

	wstring info = L"���";
	int64_t desknum = 10000000;
	int64_t playNum = 111111;
	Desks::game(desknum, playNum, "����б�");

	for (int i = 0; i < 3; i++) {
		Desks::game(desknum, playNum + i, "����");
	}

	Desks::game(desknum, playNum, "��ʼ��Ϸ");

	while (true) {
		char str[30];
		cin.getline(str, 30);
		cout << str << endl;

		Util::testMsg(desknum, playNum, str);

	}

}

int main() {
	Desks::game(33333, "���ӻ���22222");
	//DeleteFile(L".\\com.master5.doudizu\\a.txt");
	//Config::getScore(11111);
	//wstring dirName = L".\\com.master.five\\";
	//CreateDirectory(dirName.c_str(), NULL);
	//testGame();
	system("pause");



	return 0;
};

