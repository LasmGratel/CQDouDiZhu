#include "stdafx.h"
#include "../com.master5.doudizhu/game.h"
#include <fstream>
using namespace std;

void testGame() {

	wstring info = L"你好";
	int64_t desknum = 10000000;
	int64_t playNum = 111111;
	Desks::game(desknum, playNum, "玩家列表");

	for (int i = 0; i < 3; i++) {
		Desks::game(desknum, playNum + i, "上桌");
	}

	Desks::game(desknum, playNum, "开始游戏");

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
int main() {
	WritePrivateProfileString(_T("895252155"), _T("用户名"), _T("nihao"), _T("./nihao.ini"));
	TCHAR UserName[20];
	GetPrivateProfileString(_T("895252155"), _T("用户名"), _T(""), UserName,20, _T("./nihao.ini"));
	

	wstring ssw = L"33";

	stringstream sm;
	sm << ssw.c_str();

	cout << wstring2string(ssw) << endl;

	char *ss = "sss";

	wstringstream msg;
	msg << ss;
	
	wcout << msg.str() << endl;


	testGame();
	system("pause");



	return 0;
};

