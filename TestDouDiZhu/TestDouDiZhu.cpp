
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

	Util::setIsTest(true);
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

		//int index = datas.desks[datas.getDesk(desknum)]->nextPlayIndex;
		//datas.game(desknum, playNum + index, str);

	}

	system("pause");
	return 0;
};

