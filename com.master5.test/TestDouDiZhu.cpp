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

string wstring2string(wstring wstr)
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
int main() {


	Config::writeAdmin(22234444);
	
	//wcout << tmp << endl;
	Config::writeScore(1111,333);

	cout << Config::readScore(1111);
	//testGame();
	system("pause");



	return 0;
};

