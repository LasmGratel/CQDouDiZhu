
#include "stdafx.h"
#include "string"
#include <vector>
#include <algorithm>
#include <iostream>
#include <time.h>
#include <sstream>
#include "../com.master5.doudizhu/game.h"

using namespace std;


int main() {

	string info = "���";
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

		Util::testMsg(desknum,playNum,str);

	}

	system("pause");
	return 0;
};

