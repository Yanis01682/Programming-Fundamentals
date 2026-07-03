// 庄  坚

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include <cstring>
#include <string>
#include <bits/stdc++.h>
#include <vector>
#include <cctype> // 用于 isdigit 函数
using namespace std;

// 运行用户指令时运行到哪一步出现异常情况就立即报错，输出error on instruction x
// 如果能够按照程序一直运行完，那就输出success或者fail
string id1lev1, id1lev2, id1lev3, id1lev4, id2lev1, id2lev2, id2lev3, id2lev4, id3lev1, id3lev2, id3lev3, id3lev4; // 用于储存每一关通关信息
string level1str, level2str, level3str, level4str;
string customfilename;
int id;
struct InstructionNum
{
	int num;
	bool ifhave;
} instructionNum;
int level, speed;
string filename;
bool ifnormalend = 1;	// 判断是否正常结束
int totalstep;			// 关卡结束后输出总共的操作数
int robotcurrentx = 11; // 机器人现在的x坐标（纵坐标永远是19）
string inputmethod;
ifstream fin;
ofstream fout;
int instructionnum;
int outboxshuzu[30];
int outboxshuzuxiabiao = 1;
// outboxshuzu是通过执行玩家的指令得到的
int geichuinboxshuzuxiabiao = 0; // 当前输入传送带第一个元素所在的下标，从0开始
// 指令打包成结构体
struct Instruction
{
	string name;
	int num;
	int ifused; // 帮助储存一共进行了多少次指令
};
Instruction instruction[105];
// 机器人手头的东西
struct Currentblock
{
	bool ifhave;
	int num;
} currentblock;
// 空地的数字
struct Blankspace
{
	bool ifhave;
	int num;
} blankspace[3]; // 空地下标从0开始
// 关卡信息打包成结构体
struct Levelinfo
{
	int inboxnum;
	int geichuinboxshuzu[10];
	int outboxnum;
	int yuqishuchu[10]; // 存储正确的输出
	int availileblanknum;
	int availableinstructionnum;
	std::vector<string> availableinstruction;
	// string availableinstruction[10];
	string levelMission;
};
Levelinfo levelinfo[5] = {
	{2, {1, 2}, 2, {1, 2}, 0, 2, {"inbox", "outbox"}, {"Let the robot take each block in the input sequence and put it into the output sequence"}},
	{8, {3, 9, 5, 1, -2, -2, 9, -9}, 8, {-6, 6, 4, -4, 0, 0, 18, -18}, 3, 8, {"inbox", "outbox", "copyfrom", "copyto", "add", "sub", "jump", "jumpifzero"}, {"For every two items in the input sequence, first subtract the second from the first and put the result in the output sequence, then subtract the first from the second and put the result in the output sequence, and repeat."}},
	{8, {6, 2, 7, 7, -9, 3, -3, -3}, 2, {7, -3}, 3, 8, {"inbox", "outbox", "copyfrom", "copyto", "add", "sub", "jump", "jumpifzero"}, {"Take two numbers from the input sequence in sequence, output one if they are equal, otherwise throw it away. Repeat this process until the input conveyor belt is empty."}},
	{4, {6, -3, 4, 0}, 4, {48, -24, 32, 0}, 3, 8, {"inbox", "outbox", "copyfrom", "copyto", "add", "sub", "jump", "jumpifzero"}, {"For each number in the inbox, multiply it by 8 and put the result in the outbox"}}};

// 报错后或者正常结束后清空一些数据，防止影响后续关卡游玩'
void clear();

// 打开关卡信息文件
void openlevelinformation()
{

	fin.open("levelinformation.txt");
	getline(fin, id1lev1);
	getline(fin, id1lev2);
	getline(fin, id1lev3);
	getline(fin, id1lev4);
	getline(fin, id2lev1);
	getline(fin, id2lev2);
	getline(fin, id2lev3);
	getline(fin, id2lev4);
	getline(fin, id3lev1);
	getline(fin, id3lev2);
	getline(fin, id3lev3);
	getline(fin, id3lev4);
	fin.close();
}

// 通关之后存储通关状态
void storelevelinformation()
{
	fout.open("levelinformation.txt");
	fout << id1lev1 << endl
		 << id1lev2 << endl
		 << id1lev3 << endl
		 << id1lev4 << endl
		 << id2lev1 << endl
		 << id2lev2 << endl
		 << id2lev3 << endl
		 << id2lev4 << endl
		 << id3lev1 << endl
		 << id3lev2 << endl
		 << id3lev3 << endl
		 << id3lev4;
	fout.close();
}
// 光标移动函数
void gotoxy(int xpos, int ypos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = xpos;
	scrn.Y = ypos;
	SetConsoleCursorPosition(hOuput, scrn);
}
void pre();
// 输出关卡信息，绘制输入传送带，输出传送带，等待用户输入操作数（keyboard）或者文件名称（file）
void print()
{
	// 输出关卡信息
	gotoxy(0, 0);
	cout << "Information of level " << level << ":";
	cout << endl
		 << "levelmission:" << levelinfo[level - 1].levelMission;
	cout << endl
		 << "inboxnumber:" << levelinfo[level - 1].inboxnum;
	cout << endl
		 << "Input sequence:";
	for (int i = 0; i < levelinfo[level - 1].inboxnum; i++)
		cout << levelinfo[level - 1].geichuinboxshuzu[i] << " ";
	cout << endl
		 << "outboxnumber:" << levelinfo[level - 1].outboxnum;
	cout << endl
		 << "Target output sequence:";
	for (int i = 0; i < levelinfo[level - 1].outboxnum; i++)
		cout << levelinfo[level - 1].yuqishuchu[i] << " ";
	cout << endl
		 << "availibleblanknumber:" << levelinfo[level - 1].availileblanknum;
	cout << endl
		 << "Usable instructions:" << "  ";
	if (level == 5)
	{
		for (int i = 0; i < levelinfo[3].availableinstructionnum; i++)
			cout << levelinfo[3].availableinstruction[i] << " ";
	}
	else
	{
		for (int i = 0; i < levelinfo[level - 1].availableinstructionnum; i++)
			cout << levelinfo[level - 1].availableinstruction[i] << " ";
	}
	// 绘制inboxnum次输入传送带

	gotoxy(0, 19);
	cout << "IN";
	int j = 18;
	for (int i = 0; i < levelinfo[level - 1].inboxnum; i++)
	{
		gotoxy(5, j);
		cout << "+---+";
		if (levelinfo[level - 1].geichuinboxshuzu[i] >= 0)
		{
			gotoxy(5, j + 1);
			cout << "| " << levelinfo[level - 1].geichuinboxshuzu[i] << " |";
		}
		else
		{
			gotoxy(5, j + 1);
			cout << "|" << levelinfo[level - 1].geichuinboxshuzu[i] << " |";
		}
		gotoxy(5, j + 2);
		cout << "+---+";
		j += 3;
	}
	// 绘制inboxnum次输入传送带
	gotoxy(56, 19);
	cout << "OUT";
	j = 18;
	for (int i = 0; i < levelinfo[level - 1].outboxnum; i++)
	{
		gotoxy(48, j);
		cout << "+---+";
		gotoxy(48, j + 1);
		cout << "|   |";
		gotoxy(48, j + 2);
		cout << "+---+";
		j += 3;
	}
	// 绘制小人
	gotoxy(11, 18);
	cout << "+---+";
	gotoxy(11, 19);
	cout << "|   |";
	gotoxy(11, 20);
	cout << "+---+";
	gotoxy(11, 21);
	cout << "@   @";
	gotoxy(11, 22);
	cout << "-----";
	gotoxy(11, 23);
	cout << "|@ @|";
	gotoxy(11, 24);
	cout << "  +  ";
	gotoxy(11, 25);
	cout << "/   \\";
	gotoxy(11, 26);
	cout << " | | ";
	robotcurrentx = 11;
	// 绘制空地,x:21,27,33 y:30   横坐标6*instructionNum+21
	int x = 21;
	for (int i = 0; i < levelinfo[level - 1].availileblanknum; ++i)
	{
		gotoxy(x, 30);
		cout << "+---+";
		gotoxy(x, 31);
		cout << "|   |";
		gotoxy(x, 32);
		cout << "+---+";
		gotoxy(x + 2, 33);
		cout << i;
		x += 6;
	}
	// 绘制分割线
	int y = 18;
	while (y <= 50)
	{
		gotoxy(60, y++);
		cout << "|";
	}
	gotoxy(65, 18);
	cout << "===== CODE =====";
	gotoxy(0, 10);
	cout << "Please select your input method ('keyboard'or'file')" << endl;
	getline(cin, inputmethod);
	if (inputmethod == "keyboard")
	{
		gotoxy(0, 10);
		cout << "                                                                      " << endl
			 << "                              ";
		gotoxy(0, 10);
		cout << "Please input the number of your instructions(2-30):" << endl;
	}
	else if (inputmethod == "file")
	{
		gotoxy(0, 10);
		cout << "                                                                 " << endl
			 << "                              ";
		gotoxy(0, 10);
		cout << "Please input your filename:" << endl;
	}
	else
	{
		cout << "\033c";
		gotoxy(0, 0);
		cout << "invalid input";
		inputmethod = {};
		Sleep(3000);
		pre();
	}
}
void pre();
void robotmove(string, int);

// 清屏函数
void clearScreen()
{
	cout << "\033c";
}

int getValidLevelInput()
{
	int lev; // 将 lev 改为 int 类型
	while (true)
	{
		// 显示之前的通关信息
		gotoxy(0, 0);
		cout << "Your previous customs clearance are as followed:" << endl;
		if (id == 1)
		{
			cout << "level1: " << id1lev1 << endl;
			cout << "level2: " << id1lev2 << endl;
			cout << "level3: " << id1lev3 << endl;
			cout << "level4: " << id1lev4 << endl;
		}
		if (id == 2)
		{
			cout << "level1: " << id2lev1 << endl;
			cout << "level2: " << id2lev2 << endl;
			cout << "level3: " << id2lev3 << endl;
			cout << "level4: " << id2lev4 << endl;
		}
		if (id == 3)
		{
			cout << "level1: " << id3lev1 << endl;
			cout << "level2: " << id3lev2 << endl;
			cout << "level3: " << id3lev3 << endl;
			cout << "level4: " << id3lev4 << endl;
		}
		cout << "Please select level, if you enter 5, it means you will add custom levels through level data files." << endl;

		string input;
		getline(cin, input); // 获取用户输入

		try
		{
			// 检查输入是否是一个有效的数字，并且在有效范围内
			if (input.find_first_not_of("0123456789") != string::npos)
			{
				// 如果输入中有非数字字符
				throw invalid_argument("Invalid input.");
			}
			else
			{
				lev = stoi(input); // 将有效数字转换为整数
				if (lev < 1 || lev > 5)
				{
					throw out_of_range("Input out of valid range.");
				}
				else
				{
					return lev; // 返回有效的关卡输入
				}
			}
		}
		catch (const invalid_argument &e)
		{
			// 捕获非法输入（非数字字符）
			cout << "Invalid input, please input again." << endl;
			Sleep(2000);
			clearScreen();
		}
		catch (const out_of_range &e)
		{
			// 捕获超出 int 范围的数字
			cout << "Number out of range, please input again." << endl;
			Sleep(2000);
			clearScreen();
		}
	}
}
void pre1();
void pre234();
void custom();

void rgb_init()
{
	// 初始化
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);   // 输入句柄
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // 输出句柄
	DWORD dwInMode, dwOutMode;
	GetConsoleMode(hIn, &dwInMode);	  // 获取控制台输入模式
	GetConsoleMode(hOut, &dwOutMode); // 获取控制台输出模式
	dwInMode |= 0x0200;				  // 更改
	dwOutMode |= 0x0004;
	SetConsoleMode(hIn, dwInMode);	 // 设置控制台输入模式
	SetConsoleMode(hOut, dwOutMode); // 设置控制台输出模式
}

void rgb_set(int wr, int wg, int wb, int br, int bg, int bb)
{
	// 设置RGB
	printf("\033[38;2;%d;%d;%dm\033[48;2;%d;%d;%dm", wr, wg, wb, br, bg, bb); //\033[38表示前景，\033[48表示背景，三个%d表示混合的数
}

int gamespeed()
{
	gotoxy(2, 1);
	rgb_init();
	rgb_set(255, 255, 255, 63, 126, 130);
	cout << "Please select the game speed by entering a number between 1-2000.The larger the number, the faster the speed.";
	gotoxy(2, 5);
	string speedstr;
	int spe;
	getline(cin, speedstr);
	try
	{
		// 检查输入是否是一个有效的数字，并且在有效范围内
		if (speedstr.find_first_not_of("0123456789") != string::npos)
		{
			// 如果输入中有非数字字符
			throw invalid_argument("Invalid input.");
			gamespeed();
		}
		else
		{
			spe = stoi(speedstr); // 将有效数字转换为整数
			if (spe < 1 || spe > 2000)
			{
				throw out_of_range("Input out of valid range.");
				gamespeed();
			}
			else
			{
				return 2001 - spe; // 返回有效的关卡输入
			}
		}
	}
	catch (const invalid_argument &e)
	{
		// 捕获非法输入（非数字字符）
		cout << "Invalid input, please input again." << endl;
		Sleep(2000);
		clearScreen();
		gamespeed();
	}
	catch (const out_of_range &e)
	{
		// 捕获超出 int 范围的数字
		cout << "Number out of range, please input again." << endl;
		Sleep(2000);
		clearScreen();
		gamespeed();
	}
}
// 开始时向屏幕输出图形界面放在了getValidLevelInput函数中
void pre()
{

	clearScreen();
	speed = gamespeed();
	clearScreen();
	gotoxy(0, 0);

	level = getValidLevelInput(); // 获取有效的关卡输入

	switch (level)
	{
	case 1:
		cout << "Loading..." << endl;
		Sleep(500);
		clearScreen();
		pre1();
		break;
	case 2:
		if (level1str == "succeed")
		{
			cout << "Loading..." << endl;
			Sleep(500);
			clearScreen();
			pre234();
		}
		else
		{
			cout << "The previous level has not been cleared yet, please input agein" << endl;
			Sleep(1000);
			clearScreen();
			pre();
		}
		break;
	case 3:
		if (level1str == "succeed" && level2str == "succeed")
		{
			cout << "Loading..." << endl;
			Sleep(500);
			clearScreen();
			pre234();
		}
		else
		{
			cout << "The previous level has not been cleared yet, please input agein" << endl;
			Sleep(1000);
			clearScreen();
			pre();
		}
		break;
	case 4:
		if (level1str == "succeed" && level2str == "succeed" && level3str == "succeed")
		{
			cout << "Loading..." << endl;
			Sleep(500);
			clearScreen();
			pre234();
		}
		else
		{
			cout << "The previous level has not been cleared yet, please input agein" << endl;
			Sleep(1000);
			clearScreen();
			pre();
		}
		break;
	case 5:
		cout << "Loading..." << endl;
		Sleep(500);
		clearScreen();
		custom();
		break;
	default:
		cout << "Invalid number,please input again" << endl;
		break;
	}
	return;
}

void robotmove(string instructionname, int instructionIndex)
{
	// 参数是指令的名称//左传送带11右传送带42，空地还没考虑

	if (instructionname == "outbox")
	{
		// 向右移动过程
		for (int i = robotcurrentx - 1; i <= 41; i++)
		{
			gotoxy(i, 18);
			cout << "     ";
			gotoxy(i, 19);
			cout << "     ";

			gotoxy(i, 20);
			cout << "     ";
			gotoxy(i, 21);
			cout << "     ";
			gotoxy(i, 22);
			cout << "     ";
			gotoxy(i, 23);
			cout << "     ";
			gotoxy(i, 24);
			cout << "     ";
			gotoxy(i, 25);
			cout << "     ";
			gotoxy(i, 26);
			cout << "     ";

			gotoxy(i + 1, 18);
			cout << "+---+";
			if (currentblock.ifhave == 1 && currentblock.num >= 0 && currentblock.num < 10)
			{
				gotoxy(i + 1, 19);
				cout << "| " << currentblock.num << " |";
			}
			else if (currentblock.ifhave == 1 && currentblock.num >= 10)
			{
				gotoxy(i + 1, 19);
				cout << "| " << currentblock.num << "|";
			}
			else if (currentblock.ifhave == 1 && currentblock.num < 0 && currentblock.num > -10)
			{
				gotoxy(i + 1, 19);
				cout << "|" << currentblock.num << " |";
			}
			else if (currentblock.ifhave == 1 && currentblock.num <= -10)
			{
				gotoxy(i + 1, 19);
				cout << "|" << currentblock.num << "|";
			}
			else
			{
				gotoxy(i + 1, 19);
				cout << "|   |";
			}
			gotoxy(i + 1, 20);
			cout << "+---+";
			gotoxy(i + 1, 21);
			cout << "@   @";
			gotoxy(i + 1, 22);
			cout << "-----";
			gotoxy(i + 1, 23);
			cout << "|@ @|";
			gotoxy(i + 1, 24);
			cout << "  +  ";
			gotoxy(i + 1, 25);
			cout << "/   \\";
			gotoxy(i + 1, 26);
			cout << " | | ";
			Sleep(speed);
		}
		robotcurrentx = 42;
		// 如果手头没有东西
		if (currentblock.ifhave == 0)
		{
			clearScreen();
			gotoxy(0, 0);
			cout << "Error on instruction " << instructionIndex; // 如果中途中断，那就中断程序输出err
			Sleep(3000);
			cout << "\033c";
			clear();
			pre();
		}
		else
		{
			// 执行放出的操作//48,18
			outboxshuzu[outboxshuzuxiabiao] = currentblock.num;
			outboxshuzuxiabiao++;
			// 第一个输出的数字会被存储到outboxshuzhu[1]中，以此类推
			// 先全部清楚掉
			int j = 18;
			for (int i = 0; i < levelinfo[level - 1].outboxnum; i++)
			{
				gotoxy(48, j);
				cout << "     ";
				gotoxy(48, j + 1);
				cout << "      ";
				gotoxy(48, j + 2);
				cout << "      ";
				j += 3;
			}
			// 再输出有东西的输出传送带
			j = 18;
			int temp = outboxshuzuxiabiao - 1;
			for (int i = 1; i < outboxshuzuxiabiao; i++)
			{
				// 输出outboxxshuzuxiabiao-1次
				gotoxy(48, j);
				cout << "+---+";
				if (outboxshuzu[temp] >= 0 && outboxshuzu[temp] < 10)
				{
					gotoxy(48, j + 1);
					cout << "| " << outboxshuzu[temp] << " |";
				}
				if (outboxshuzu[temp] >= 10)
				{
					gotoxy(48, j + 1);
					cout << "| " << outboxshuzu[temp] << "|";
				}
				else if (outboxshuzu[temp] < 0 && outboxshuzu[temp] > -10)
				{
					gotoxy(48, j + 1);
					cout << "|" << outboxshuzu[temp] << " |";
				}
				else if (outboxshuzu[temp] <= -10)
				{
					gotoxy(48, j + 1);
					cout << "|" << outboxshuzu[temp] << "|";
				}
				gotoxy(48, j + 2);
				cout << "+---+";
				j += 3;
				temp--;
			}
			// 输出没有东西的输出传送带
			for (int i = 0; i < levelinfo[level - 1].outboxnum + 1 - outboxshuzuxiabiao; i++)
			{
				// 输出outboxxshuzuxiabiao-1次
				gotoxy(48, j);
				cout << "+---+";
				gotoxy(48, j + 1);
				cout << "|   |";
				gotoxy(48, j + 2);
				cout << "+---+";
				j += 3;
			}
			// 将机器人手头的东西清空
			currentblock.ifhave = false;
			currentblock.num = 0;
			gotoxy(44, 19);
			cout << " ";
			// 此时不能判断是否正确输出，因为还不满足程序结束条件
			// 如果正确输入，无事发生，继续下一个指令
		}
	}
	else if (instructionname == "inbox")
	{
		// 先向左移动到11
		for (int i = robotcurrentx; i >= 11; i--)
		{
			gotoxy(i, 18);
			cout << "     ";
			gotoxy(i, 19);
			cout << "     ";
			gotoxy(i, 20);
			cout << "     ";
			gotoxy(i, 21);
			cout << "     ";
			gotoxy(i, 22);
			cout << "     ";
			gotoxy(i, 23);
			cout << "     ";
			gotoxy(i, 24);
			cout << "     ";
			gotoxy(i, 25);
			cout << "     ";
			gotoxy(i, 26);
			cout << "     ";

			gotoxy(i - 1, 18);
			cout << "+---+";
			if (currentblock.ifhave == 1 && currentblock.num >= 0 && currentblock.num < 10)
			{
				gotoxy(i - 1, 19);
				cout << "| " << currentblock.num << " |";
			}
			else if (currentblock.ifhave == 1 && currentblock.num >= 10)
			{
				gotoxy(i - 1, 19);
				cout << "| " << currentblock.num << "|";
			}
			else if (currentblock.ifhave == 1 && currentblock.num < 0 && currentblock.num > -10)
			{
				gotoxy(i - 1, 19);
				cout << "|" << currentblock.num << " |";
			}
			else if (currentblock.ifhave == 1 && currentblock.num <= -10)
			{
				gotoxy(i - 1, 19);
				cout << "|" << currentblock.num << "|";
			}
			else
			{
				gotoxy(i - 1, 19);
				cout << "|   |";
			}

			gotoxy(i - 1, 20);
			cout << "+---+";
			gotoxy(i - 1, 21);
			cout << "@   @";
			gotoxy(i - 1, 22);
			cout << "-----";
			gotoxy(i - 1, 23);
			cout << "|@ @|";
			gotoxy(i - 1, 24);
			cout << "  +  ";
			gotoxy(i - 1, 25);
			cout << "/   \\";
			gotoxy(i - 1, 26);
			cout << " | | ";
			Sleep(speed);
		}
		robotcurrentx = 11;

		// inbox时左边传送带没有的情况,应该结束然后判断，但是这个放在了pre1函数里，也就是进行inbox操作之前就要先判断一次左传送带是否有东西
		currentblock.ifhave = 1;
		currentblock.num = levelinfo[level - 1].geichuinboxshuzu[geichuinboxshuzuxiabiao]; // 下标从0开始
		geichuinboxshuzuxiabiao++;
		// 改变机器人手中拿着的东西
		gotoxy(11, 19);
		cout << "   ";
		if (currentblock.num >= 0)
		{
			gotoxy(12, 19);
			cout << currentblock.num;
		}
		else if (currentblock.num < 0)
		{
			gotoxy(11, 19);
			cout << currentblock.num;
		}
		Sleep(300);
		// 拿入之后改变输入传送带状态
		// 左传送带先全部清空
		int j = 18;
		for (int i = 0; i < levelinfo[level - 1].inboxnum; i++)
		{
			gotoxy(5, j);
			cout << "     ";
			gotoxy(5, j + 1);
			cout << "     ";
			gotoxy(5, j + 2);
			cout << "     ";
			j += 3;
		}
		// 再输出有东西的左传送带
		j = 18;
		int temp = geichuinboxshuzuxiabiao;
		for (int i = 0; i < levelinfo[level - 1].inboxnum - geichuinboxshuzuxiabiao; i++)
		{
			gotoxy(5, j);
			cout << "+---+";
			if (levelinfo[level - 1].geichuinboxshuzu[temp] >= 0 && levelinfo[level - 1].geichuinboxshuzu[temp] < 10)
			{
				gotoxy(5, j + 1);
				cout << "| " << levelinfo[level - 1].geichuinboxshuzu[temp++] << " |";
			}
			else if (levelinfo[level - 1].geichuinboxshuzu[temp] >= 10)
			{
				gotoxy(5, j + 1);
				cout << "| " << levelinfo[level - 1].geichuinboxshuzu[temp++] << "|";
			}
			else if (levelinfo[level - 1].geichuinboxshuzu[temp] < 0 && levelinfo[level - 1].geichuinboxshuzu[temp] > -10)
			{
				gotoxy(5, j + 1);
				cout << "|" << levelinfo[level - 1].geichuinboxshuzu[temp++] << " |";
			}
			else if (levelinfo[level - 1].geichuinboxshuzu[temp] <= -10)
			{
				gotoxy(5, j + 1);
				cout << "|" << levelinfo[level - 1].geichuinboxshuzu[temp++] << "|";
			}
			gotoxy(5, j + 2);
			cout << "+---+";
			j += 3;
		}
		// 再输出没有东西的左传送带
		for (int i = 0; i < geichuinboxshuzuxiabiao; i++)
		{
			gotoxy(5, j);
			cout << "+---+";
			gotoxy(5, j + 1);
			cout << "|   |";
			gotoxy(5, j + 2);
			cout << "+---+";
			j += 3;
		}
	}
	else if (instructionname == "add")
	{
		// 先移动过去
		if (robotcurrentx > (6 * instructionNum.num + 21))
		{
			for (int i = robotcurrentx; i > (6 * instructionNum.num + 21); --i)
			{
				gotoxy(i, 18);
				cout << "     ";
				gotoxy(i, 19);
				cout << "     ";
				gotoxy(i, 20);
				cout << "     ";
				gotoxy(i, 21);
				cout << "     ";
				gotoxy(i, 22);
				cout << "     ";
				gotoxy(i, 23);
				cout << "     ";
				gotoxy(i, 24);
				cout << "     ";
				gotoxy(i, 25);
				cout << "     ";
				gotoxy(i, 26);
				cout << "     ";

				gotoxy(i - 1, 18);
				cout << "+---+";
				if (currentblock.ifhave == 1 && currentblock.num >= 0 && currentblock.num < 10)
				{
					gotoxy(i - 1, 19);
					cout << "| " << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num >= 10)
				{
					gotoxy(i - 1, 19);
					cout << "| " << currentblock.num << "|";
				}
				else if (currentblock.ifhave == 1 && currentblock.num < 0 && currentblock.num > -10)
				{
					gotoxy(i - 1, 19);
					cout << "|" << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num <= -10)
				{
					gotoxy(i - 1, 19);
					cout << "|" << currentblock.num << "|";
				}
				else
				{
					gotoxy(i - 1, 19);
					cout << "|   |";
				}
				gotoxy(i - 1, 20);
				cout << "+---+";
				gotoxy(i - 1, 21);
				cout << "@   @";
				gotoxy(i - 1, 22);
				cout << "-----";
				gotoxy(i - 1, 23);
				cout << "|@ @|";
				gotoxy(i - 1, 24);
				cout << "  +  ";
				gotoxy(i - 1, 25);
				cout << "/   \\";
				gotoxy(i - 1, 26);
				cout << " | | ";
				Sleep(speed);
			}
			robotcurrentx = 6 * instructionNum.num + 21;
		}
		else if (robotcurrentx < (6 * instructionNum.num + 21))
		{
			for (int i = robotcurrentx - 1; i < (6 * instructionNum.num + 21); ++i)
			{
				gotoxy(i, 18);
				cout << "     ";
				gotoxy(i, 19);
				cout << "     ";
				gotoxy(i, 20);
				cout << "     ";
				gotoxy(i, 21);
				cout << "     ";
				gotoxy(i, 22);
				cout << "     ";
				gotoxy(i, 23);
				cout << "     ";
				gotoxy(i, 24);
				cout << "     ";
				gotoxy(i, 25);
				cout << "     ";
				gotoxy(i, 26);
				cout << "     ";

				gotoxy(i + 1, 18);
				cout << "+---+";
				if (currentblock.ifhave == 1 && currentblock.num >= 0 && currentblock.num < 10)
				{
					gotoxy(i + 1, 19);
					cout << "| " << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num >= 10)
				{
					gotoxy(i + 1, 19);
					cout << "| " << currentblock.num << "|";
				}
				else if (currentblock.ifhave == 1 && currentblock.num < 0 && currentblock.num > -10)
				{
					gotoxy(i + 1, 19);
					cout << "|" << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num <= -10)
				{
					gotoxy(i + 1, 19);
					cout << "|" << currentblock.num << "|";
				}
				else
				{
					gotoxy(i + 1, 19);
					cout << "|   |";
				}
				gotoxy(i + 1, 20);
				cout << "+---+";
				gotoxy(i + 1, 21);
				cout << "@   @";
				gotoxy(i + 1, 22);
				cout << "-----";
				gotoxy(i + 1, 23);
				cout << "|@ @|";
				gotoxy(i + 1, 24);
				cout << "  +  ";
				gotoxy(i + 1, 25);
				cout << "/   \\";
				gotoxy(i + 1, 26);
				cout << " | | ";
				Sleep(speed);
			}
			robotcurrentx = 6 * instructionNum.num + 21;
		}
		// 处理add的另外两种异常情况
		if (currentblock.ifhave == 0 || blankspace[instructionNum.num].ifhave == 0)
		{
			clearScreen();
			gotoxy(0, 0);
			cout << "Error on instruction " << instructionIndex;
			Sleep(3000);
			cout << "\033c";
			clear();
			pre();
		}
		else
		{
			// 执行加的操作
			currentblock.num += blankspace[instructionNum.num].num;
			Sleep(speed);
			// 改变机器人手头的数字
			if (currentblock.num >= 0 && currentblock.num < 10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "| " << currentblock.num << " |";
			}
			else if (currentblock.num >= 10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "| " << currentblock.num << "|";
			}
			else if (currentblock.num < 0 && currentblock.num > -10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "|" << currentblock.num << " |";
			}
			else if (currentblock.num <= -10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "|" << currentblock.num << "|";
			}
		}
	}
	else if (instructionname == "sub")
	{
		// 先移动过去
		if (robotcurrentx > (6 * instructionNum.num + 21))
		{
			for (int i = robotcurrentx; i > (6 * instructionNum.num + 21); --i)
			{
				gotoxy(i, 18);
				cout << "     ";
				gotoxy(i, 19);
				cout << "     ";
				gotoxy(i, 20);
				cout << "     ";
				gotoxy(i, 21);
				cout << "     ";
				gotoxy(i, 22);
				cout << "     ";
				gotoxy(i, 23);
				cout << "     ";
				gotoxy(i, 24);
				cout << "     ";
				gotoxy(i, 25);
				cout << "     ";
				gotoxy(i, 26);
				cout << "     ";

				gotoxy(i - 1, 18);
				cout << "+---+";
				if (currentblock.ifhave == 1 && currentblock.num >= 0 && currentblock.num < 10)
				{
					gotoxy(i - 1, 19);
					cout << "| " << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num >= 10)
				{
					gotoxy(i - 1, 19);
					cout << "| " << currentblock.num << "|";
				}
				else if (currentblock.ifhave == 1 && currentblock.num < 0 && currentblock.num > -10)
				{
					gotoxy(i - 1, 19);
					cout << "|" << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num <= -10)
				{
					gotoxy(i - 1, 19);
					cout << "|" << currentblock.num << "|";
				}
				else
				{
					gotoxy(i - 1, 19);
					cout << "|   |";
				}
				gotoxy(i - 1, 20);
				cout << "+---+";
				gotoxy(i - 1, 21);
				cout << "@   @";
				gotoxy(i - 1, 22);
				cout << "-----";
				gotoxy(i - 1, 23);
				cout << "|@ @|";
				gotoxy(i - 1, 24);
				cout << "  +  ";
				gotoxy(i - 1, 25);
				cout << "/   \\";
				gotoxy(i - 1, 26);
				cout << " | | ";
				Sleep(speed);
			}
			robotcurrentx = 6 * instructionNum.num + 21;
		}
		else if (robotcurrentx < (6 * instructionNum.num + 21))
		{
			for (int i = robotcurrentx - 1; i < (6 * instructionNum.num + 21); ++i)
			{
				gotoxy(i, 18);
				cout << "     ";
				gotoxy(i, 19);
				cout << "     ";
				gotoxy(i, 20);
				cout << "     ";
				gotoxy(i, 21);
				cout << "     ";
				gotoxy(i, 22);
				cout << "     ";
				gotoxy(i, 23);
				cout << "     ";
				gotoxy(i, 24);
				cout << "     ";
				gotoxy(i, 25);
				cout << "     ";
				gotoxy(i, 26);
				cout << "     ";

				gotoxy(i + 1, 18);
				cout << "+---+";
				if (currentblock.ifhave == 1 && currentblock.num >= 0 && currentblock.num < 10)
				{
					gotoxy(i + 1, 19);
					cout << "| " << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num >= 10)
				{
					gotoxy(i + 1, 19);
					cout << "| " << currentblock.num << "|";
				}
				else if (currentblock.ifhave == 1 && currentblock.num < 0 && currentblock.num > -10)
				{
					gotoxy(i + 1, 19);
					cout << "|" << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num <= -10)
				{
					gotoxy(i + 1, 19);
					cout << "|" << currentblock.num << "|";
				}
				else
				{
					gotoxy(i + 1, 19);
					cout << "|   |";
				}
				gotoxy(i + 1, 20);
				cout << "+---+";
				gotoxy(i + 1, 21);
				cout << "@   @";
				gotoxy(i + 1, 22);
				cout << "-----";
				gotoxy(i + 1, 23);
				cout << "|@ @|";
				gotoxy(i + 1, 24);
				cout << "  +  ";
				gotoxy(i + 1, 25);
				cout << "/   \\";
				gotoxy(i + 1, 26);
				cout << " | | ";
				Sleep(speed);
			}
			robotcurrentx = 6 * instructionNum.num + 21;
		}
		// 处理add的另外两种异常情况
		if (currentblock.ifhave == 0 || blankspace[instructionNum.num].ifhave == 0)
		{
			clearScreen();
			gotoxy(0, 0);
			cout << "Error on instruction " << instructionIndex;
			Sleep(speed);
			cout << "\033c";
			clear();
			pre();
		}
		else
		{
			// 执行sub的操作
			currentblock.num -= blankspace[instructionNum.num].num;
			Sleep(1000);
			// 改变机器人手头的数字
			if (currentblock.num >= 0 && currentblock.num < 10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "| " << currentblock.num << " |";
			}
			else if (currentblock.num >= 10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "| " << currentblock.num << "|";
			}
			else if (currentblock.num < 0 && currentblock.num > -10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "|" << currentblock.num << " |";
			}
			else if (currentblock.num <= -10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "|" << currentblock.num << "|";
			}
		}
	}
	else if (instructionname == "copyto")
	{
		// 先移动过去
		if (robotcurrentx > (6 * instructionNum.num + 21))
		{
			for (int i = robotcurrentx; i > (6 * instructionNum.num + 21); --i)
			{
				gotoxy(i, 18);
				cout << "     ";
				gotoxy(i, 19);
				cout << "     ";
				gotoxy(i, 20);
				cout << "     ";
				gotoxy(i, 21);
				cout << "     ";
				gotoxy(i, 22);
				cout << "     ";
				gotoxy(i, 23);
				cout << "     ";
				gotoxy(i, 24);
				cout << "     ";
				gotoxy(i, 25);
				cout << "     ";
				gotoxy(i, 26);
				cout << "     ";

				gotoxy(i - 1, 18);
				cout << "+---+";
				if (currentblock.ifhave == 1 && currentblock.num >= 0 && currentblock.num < 10)
				{
					gotoxy(i - 1, 19);
					cout << "| " << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num >= 10)
				{
					gotoxy(i - 1, 19);
					cout << "| " << currentblock.num << "|";
				}
				else if (currentblock.ifhave == 1 && currentblock.num < 0 && currentblock.num > -10)
				{
					gotoxy(i - 1, 19);
					cout << "|" << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num <= -10)
				{
					gotoxy(i - 1, 19);
					cout << "|" << currentblock.num << "|";
				}
				else
				{
					gotoxy(i - 1, 19);
					cout << "|   |";
				}
				gotoxy(i - 1, 20);
				cout << "+---+";
				gotoxy(i - 1, 21);
				cout << "@   @";
				gotoxy(i - 1, 22);
				cout << "-----";
				gotoxy(i - 1, 23);
				cout << "|@ @|";
				gotoxy(i - 1, 24);
				cout << "  +  ";
				gotoxy(i - 1, 25);
				cout << "/   \\";
				gotoxy(i - 1, 26);
				cout << " | | ";
				Sleep(speed);
			}
			robotcurrentx = 6 * instructionNum.num + 21;
		}
		else if (robotcurrentx < (6 * instructionNum.num + 21))
		{
			for (int i = robotcurrentx - 1; i < (6 * instructionNum.num + 21); ++i)
			{
				gotoxy(i, 18);
				cout << "     ";
				gotoxy(i, 19);
				cout << "     ";
				gotoxy(i, 20);
				cout << "     ";
				gotoxy(i, 21);
				cout << "     ";
				gotoxy(i, 22);
				cout << "     ";
				gotoxy(i, 23);
				cout << "     ";
				gotoxy(i, 24);
				cout << "     ";
				gotoxy(i, 25);
				cout << "     ";
				gotoxy(i, 26);
				cout << "     ";

				gotoxy(i + 1, 18);
				cout << "+---+";
				if (currentblock.ifhave == 1 && currentblock.num >= 0 && currentblock.num < 10)
				{
					gotoxy(i + 1, 19);
					cout << "| " << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num >= 10)
				{
					gotoxy(i + 1, 19);
					cout << "| " << currentblock.num << "|";
				}
				else if (currentblock.ifhave == 1 && currentblock.num < 0 && currentblock.num > -10)
				{
					gotoxy(i + 1, 19);
					cout << "|" << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num <= -10)
				{
					gotoxy(i + 1, 19);
					cout << "|" << currentblock.num << "|";
				}
				else
				{
					gotoxy(i + 1, 19);
					cout << "|   |";
				}
				gotoxy(i + 1, 20);
				cout << "+---+";
				gotoxy(i + 1, 21);
				cout << "@   @";
				gotoxy(i + 1, 22);
				cout << "-----";
				gotoxy(i + 1, 23);
				cout << "|@ @|";
				gotoxy(i + 1, 24);
				cout << "  +  ";
				gotoxy(i + 1, 25);
				cout << "/   \\";
				gotoxy(i + 1, 26);
				cout << " | | ";
				Sleep(speed);
			}
			robotcurrentx = 6 * instructionNum.num + 21;
		}
		// 处理copyto的另外一种异常情况
		if (currentblock.ifhave == 0)
		{
			clearScreen();
			gotoxy(0, 0);
			cout << "Error on instruction " << instructionIndex;
			Sleep(3000);
			cout << "\033c";
			clear();
			pre();
		}
		else
		{
			// 执行copyto的操作
			blankspace[instructionNum.num].ifhave = 1;
			blankspace[instructionNum.num].num = currentblock.num;
			Sleep(1000);
			// 改变空地的数字
			if (blankspace[instructionNum.num].num >= 0 && blankspace[instructionNum.num].num < 10)
			{
				gotoxy(6 * instructionNum.num + 21, 31);
				cout << "| " << blankspace[instructionNum.num].num << " |";
			}
			else if (blankspace[instructionNum.num].num >= 10)
			{
				gotoxy(6 * instructionNum.num + 21, 31);
				cout << "| " << blankspace[instructionNum.ifhave].num << "|";
			}
			else if (currentblock.num < 0 && currentblock.num > -10)
			{
				gotoxy(6 * instructionNum.num + 21, 31);
				cout << "|" << blankspace[instructionNum.num].num << " |";
			}
			else if (currentblock.num <= -10)
			{
				gotoxy(6 * instructionNum.num + 21, 31);
				cout << "|" << blankspace[instructionNum.num].num << "|";
			}
		}
	}
	else if (instructionname == "copyfrom")
	{
		// 先移动过去
		if (robotcurrentx > (6 * instructionNum.num + 21))
		{
			for (int i = robotcurrentx; i > (6 * instructionNum.num + 21); --i)
			{
				gotoxy(i, 18);
				cout << "     ";
				gotoxy(i, 19);
				cout << "     ";
				gotoxy(i, 20);
				cout << "     ";
				gotoxy(i, 21);
				cout << "     ";
				gotoxy(i, 22);
				cout << "     ";
				gotoxy(i, 23);
				cout << "     ";
				gotoxy(i, 24);
				cout << "     ";
				gotoxy(i, 25);
				cout << "     ";
				gotoxy(i, 26);
				cout << "     ";

				gotoxy(i - 1, 18);
				cout << "+---+";
				if (currentblock.ifhave == 1 && currentblock.num >= 0 && currentblock.num < 10)
				{
					gotoxy(i - 1, 19);
					cout << "| " << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num >= 10)
				{
					gotoxy(i - 1, 19);
					cout << "| " << currentblock.num << "|";
				}
				else if (currentblock.ifhave == 1 && currentblock.num < 0 && currentblock.num > -10)
				{
					gotoxy(i - 1, 19);
					cout << "|" << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num <= -10)
				{
					gotoxy(i - 1, 19);
					cout << "|" << currentblock.num << "|";
				}
				else
				{
					gotoxy(i - 1, 19);
					cout << "|   |";
				}
				gotoxy(i - 1, 20);
				cout << "+---+";
				gotoxy(i - 1, 21);
				cout << "@   @";
				gotoxy(i - 1, 22);
				cout << "-----";
				gotoxy(i - 1, 23);
				cout << "|@ @|";
				gotoxy(i - 1, 24);
				cout << "  +  ";
				gotoxy(i - 1, 25);
				cout << "/   \\";
				gotoxy(i - 1, 26);
				cout << " | | ";
				Sleep(speed);
			}
			robotcurrentx = 6 * instructionNum.num + 21;
		}
		else if (robotcurrentx < (6 * instructionNum.num + 21))
		{
			for (int i = robotcurrentx - 1; i < (6 * instructionNum.num + 21); ++i)
			{
				gotoxy(i, 18);
				cout << "     ";
				gotoxy(i, 19);
				cout << "     ";
				gotoxy(i, 20);
				cout << "     ";
				gotoxy(i, 21);
				cout << "     ";
				gotoxy(i, 22);
				cout << "     ";
				gotoxy(i, 23);
				cout << "     ";
				gotoxy(i, 24);
				cout << "     ";
				gotoxy(i, 25);
				cout << "     ";
				gotoxy(i, 26);
				cout << "     ";

				gotoxy(i + 1, 18);
				cout << "+---+";
				if (currentblock.ifhave == 1 && currentblock.num >= 0 && currentblock.num < 10)
				{
					gotoxy(i + 1, 19);
					cout << "| " << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num >= 10)
				{
					gotoxy(i + 1, 19);
					cout << "| " << currentblock.num << "|";
				}
				else if (currentblock.ifhave == 1 && currentblock.num < 0 && currentblock.num > -10)
				{
					gotoxy(i + 1, 19);
					cout << "|" << currentblock.num << " |";
				}
				else if (currentblock.ifhave == 1 && currentblock.num <= -10)
				{
					gotoxy(i + 1, 19);
					cout << "|" << currentblock.num << "|";
				}
				else
				{
					gotoxy(i + 1, 19);
					cout << "|   |";
				}
				gotoxy(i + 1, 20);
				cout << "+---+";
				gotoxy(i + 1, 21);
				cout << "@   @";
				gotoxy(i + 1, 22);
				cout << "-----";
				gotoxy(i + 1, 23);
				cout << "|@ @|";
				gotoxy(i + 1, 24);
				cout << "  +  ";
				gotoxy(i + 1, 25);
				cout << "/   \\";
				gotoxy(i + 1, 26);
				cout << " | | ";
				Sleep(speed);
			}
			robotcurrentx = 6 * instructionNum.num + 21;
		}
		// 处理copyfrom的另外一种异常情况
		if (blankspace[instructionNum.num].ifhave == 0)
		{
			clearScreen();
			gotoxy(0, 0);
			cout << "Error on instruction " << instructionIndex;
			Sleep(3000);
			cout << "\033c";
			clear();
			pre();
		}
		else
		{
			// 执行copyfrom的操作
			currentblock.num = blankspace[instructionNum.num].num;
			currentblock.ifhave = 1;
			Sleep(1000);
			// 改变机器人手头的数字
			if (currentblock.num >= 0 && currentblock.num < 10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "| " << currentblock.num << " |";
			}
			else if (currentblock.num >= 10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "| " << currentblock.num << "|";
			}
			else if (currentblock.num < 0 && currentblock.num > -10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "|" << currentblock.num << " |";
			}
			else if (currentblock.num <= -10)
			{
				gotoxy(robotcurrentx, 19);
				cout << "|" << currentblock.num << "|";
			}
		}
	}
}

int selectidentity()
{
	openlevelinformation();

	int j = 11;
	gotoxy(2, 10);
	cout << "ID1:";
	gotoxy(2, j++);
	cout << "level1:" << id1lev1;
	gotoxy(2, j++);
	cout << "level2:" << id1lev2;
	gotoxy(2, j++);
	cout << "level3:" << id1lev3;
	gotoxy(2, j++);
	cout << "level4:" << id1lev4;
	gotoxy(22, 10);
	cout << "ID2:";
	j = 11;
	gotoxy(22, j++);
	cout << "level1:" << id2lev1;
	gotoxy(22, j++);
	cout << "level2:" << id2lev2;
	gotoxy(22, j++);
	cout << "level3:" << id2lev3;
	gotoxy(22, j++);
	cout << "level4:" << id2lev4;
	gotoxy(42, 10);
	cout << "ID3:";
	j = 11;
	gotoxy(42, j++);
	cout << "level1:" << id3lev1;
	gotoxy(42, j++);
	cout << "level2:" << id3lev2;
	gotoxy(42, j++);
	cout << "level3:" << id3lev3;
	gotoxy(42, j++);
	cout << "level4:" << id3lev4;
	int index = 0;
	char ch;
	string select[] = {"identity 1", "identity 2", "identity 3"};
	string str = "-> ";
	gotoxy(2, 1);
	cout << "Select the up or down key on the keyboard to select your identity,game progress between different identities is independent of each other" << endl;
	for (int i = 0; i < 3; i++)
	{
		gotoxy(9, 3 + i);
		cout << select[i];
	}
	gotoxy(5, 3);
	cout << str + "     " + select[0];

	while (true)
	{
		if (_kbhit())
		{
			ch = _getch(); // 这个输入不会在屏幕上显示
			if (ch == 72 || ch == 80 || ch == '\r')
			{
				if (ch == 27)
				{
					// esc
					cout << "exit system" << endl;
					Sleep(2000);
					return -1;
				}
				else if (ch == 72)
				{
					// up
					printf("\r%*c\r", 79, ' ');
					gotoxy(9, 3 + index);
					cout << select[index];
					index--;
				}
				else if (ch == 80)
				{
					// down
					printf("\r%*c\r", 79, ' ');
					gotoxy(9, 3 + index);
					cout << select[index];
					index++;
				}
				if (index < 0)
				{
					index = 2;
				}
				else if (index > 2)
				{
					index = 0;
				}
			}
			gotoxy(5, 3 + index);
			cout << "             ";
			Sleep(100);
			cout << str + select[index];
			if (ch == '\r')
			{
				return index + 1;
			}
		}
	}
}

int main()
{
	openlevelinformation();
	id = selectidentity();
	clearScreen();
	if (id == 1)
	{
		level1str = id1lev1;
		level2str = id1lev2;
		level3str = id1lev3;
		level4str = id1lev4;
	}
	else if (id == 2)
	{
		level1str = id2lev1;
		level2str = id2lev2;
		level3str = id2lev3;
		level4str = id2lev4;
	}
	else if (id == 3)
	{
		level1str = id3lev1;
		level2str = id3lev2;
		level3str = id3lev3;
		level4str = id3lev4;
	}
	// speed=gamespeed();
	pre();
	system("pause");
	return 0;
}

// 这个函数结束之后继续进行pre1之类的函数
void inputofinstructions()
{
	instructionnum = 0; // 在一开始重置
	print();			// 结束状态是等待用户输入指令数或者文件名

	if (inputmethod == "keyboard")
	{
		bool inputValid = false;
		while (!inputValid)
		{
			string input;
			getline(cin, input); // 先按照字符串形式读取整个输入

			try
			{
				// 检查输入是否是一个有效的数字，并且在有效范围内
				if (input.find_first_not_of("0123456789") != string::npos)
				{
					// 如果输入中有非数字字符
					throw invalid_argument("Invalid input.");
				}
				else
				{
					instructionnum = stoi(input); // 将有效数字转换为整数
					if (instructionnum < 2 || instructionnum > 30)
					{
						throw out_of_range("Input out of valid range.");
					}
					inputValid = true; // 输入有效，跳出循环
				}
			}
			catch (const invalid_argument &e)
			{
				gotoxy(0, 0);
				system("cls"); // 清屏
				cout << "Invalid input. Please enter a number between 2 and 30." << endl;
				Sleep(2000);
				system("cls");
				pre();
			}
			catch (const out_of_range &e)
			{
				gotoxy(0, 0);
				system("cls"); // 清屏
				cout << "Invalid input. Please enter a number between 2 and 30." << endl;
				Sleep(2000);
				system("cls");
				pre();
			}
		}

		// 输入有效的指令数量后，继续输入指令
		cout << "Please input your instructions:" << endl;
		int y = 19; // 填充时的纵坐标
		// 读取全部整行输入，然后在运行的时候再判断是否是有效输入
		for (int i = 1; i <= instructionnum; ++i)
		{
			getline(cin, instruction[i].name);
			gotoxy(67, y++);
			cout << i << " " << instruction[i].name;
			gotoxy(0, 13);
			cout << "                               "; // 清除前一行的内容
			gotoxy(0, 13);
		}
	}
	else if (inputmethod == "file")
	{
		getline(cin, filename); // 读取文件名

		fin.open(filename);

		if (!fin.is_open())
		{
			system("cls");
			gotoxy(0, 0);
			cerr << "Error: Could not open file " << filename << endl;
			Sleep(2000);
			system("cls");
			pre();
			return;
		}

		bool inputValid = false;
		// 读取指令数量
		string line;
		getline(fin, line); // 先按照字符串形式读取整个输入

		try
		{
			// 检查输入是否是一个有效的数字，并且在有效范围内
			if (line.find_first_not_of("0123456789") != string::npos)
			{
				// 如果输入中有非数字字符
				throw invalid_argument("Invalid input.");
			}
			else
			{
				instructionnum = stoi(line); // 将有效数字转换为整数
				if (instructionnum < 2 || instructionnum > 30)
				{
					throw out_of_range("Input out of valid range.");
				}
				inputValid = true; // 输入有效，跳出循环
			}
		}
		catch (const invalid_argument &e)
		{
			gotoxy(0, 0);
			system("cls"); // 清屏
			cout << "Invalid input. Please enter a number between 2 and 30." << endl;
			Sleep(2000);
			system("cls");
			pre();
		}
		catch (const out_of_range &e)
		{
			gotoxy(0, 0);
			system("cls"); // 清屏
			cout << "Invalid input. Please enter a number between 2 and 30." << endl;
			Sleep(2000);
			system("cls");
			pre();
		}

		// 读取指令内容
		bool ifInputRightly = true;
		for (int i = 1; i <= instructionnum; ++i)
		{
			if (!getline(fin, instruction[i].name))
			{
				ifInputRightly = false;
				break;
			}
		}

		if (!ifInputRightly)
		{
			cerr << "Error: File contains fewer instructions than specified." << endl;
			Sleep(2000);
			system("cls");
			pre();
			return;
		}

		// 检查文件中的指令数量是否与指定的指令数量一致
		int fileInstructionCount = 0; // 实际上的指令数
		string zhiling;
		while (getline(fin, zhiling))
		{
			fileInstructionCount++;
		}
		fin.clear();
		fin.seekg(0, ios::beg);
		getline(fin, zhiling); // 跳过第一行的指令数量

		if (fileInstructionCount < instructionnum)
		{
			cerr << "Error: File contains more instructions than specified." << endl;
			Sleep(2000);
			system("cls");
			pre();
			return;
		}

		// 输出指令
		int y = 19;
		for (int i = 1; i <= instructionnum; ++i)
		{
			gotoxy(67, y++);
			cout << i << " " << instruction[i].name;
		}
	}
	fin.close();
}
bool isValidInteger(const string &str)
{
	if (str.empty())
		return false;
	for (char ch : str)
	{
		if (!isdigit(ch))
			return false;
	}
	return true;
}
void pre1()
{
	inputofinstructions();

	// 最最重要:开始根据用户输入的指令执行动画//每执行一步就应该判断outboxshuzu里边的当前元素和预期输出是否相同
	int now = 19, previous = 19; // 用>来显示当前进行到哪一条指令

	int i = 1; // 从第一个指令开始执行
	// 终止条件:进行到最后一步且最后一步不是jump        或者inbox但是左边传送带没有数字了
	while (i)
	{
		if (instruction[i].name == "inbox" && geichuinboxshuzuxiabiao >= levelinfo[level - 1].inboxnum)
		{
			// 正常结束的条件之一：inbox时左边没有元素
			break;
		}
		else if (i == instructionnum + 1)
			break; // 这个是正常进行完最后一步，如果是inbox outbox jump1，i从1到2到3到1，不可能到instructionnum+1，所以这种情况只会在inbox时左边没有东西结束
		gotoxy(65, previous);
		cout << " ";
		gotoxy(65, now);
		cout << ">";
		if (instruction[i].name == "inbox" || instruction[i].name == "outbox")
		{
			instruction[i].ifused++; // 注意:第一个指令是instruction[1]，,这个下标是从1开始的
			previous = now;
			now++;
			robotmove(instruction[i].name, i);
			i++;
		}
		else
		{
			ifnormalend = 0; // 如果输入的不在指令集中，就按异常退出处理
			break;
		}
	}
	// 如果中途异常情况退出了
	if (ifnormalend == 0)
	{
		// 先清空
		clear();
		gotoxy(0, 0);
		clearScreen();
		gotoxy(0, 0);
		cout << "Error on instruction " << i;
		Sleep(3000);
		pre();
		return;
	}
	// 如果正常结束，判断输出是否相同
	// 总共进行的指令数
	for (int i = 1; i <= instructionnum; ++i)
	{
		totalstep += instruction[i].ifused;
	}
	// 如果正常结束，检测用户输出是否全部正确，若是，提示成功并且存储通关信息//如果异常结束，应该根本不会进行到这一步才对
	bool iftrulysyccess = 1;
	for (int i = 0; i < levelinfo[level - 1].outboxnum; ++i)
	{
		if (outboxshuzu[i + 1] != levelinfo[level - 1].yuqishuchu[i])
		{
			// 注意outbooxshuzu下标从1开始,yuqishuchu下标从0开始
			iftrulysyccess = 0;
			break;
		}
	}

	if (iftrulysyccess == 0 || outboxshuzuxiabiao - 1 != levelinfo[level - 1].outboxnum)
	{
		// 如果输出与预期输出不同或者输出个数!=预期输出的个数
		clearScreen();
		gotoxy(0, 0);
		cout << "Fail" << endl
			 << "Total step=" << totalstep;
		clear();
		Sleep(3000);
		pre();
	}
	else
	{

		level1str = "succeed";
		if (id == 1)
			id1lev1 = level1str;
		else if (id == 2)
			id2lev1 = level1str;
		else if (id == 3)
			id3lev1 = level1str;
		storelevelinformation();
		clearScreen();
		gotoxy(0, 0);
		cout << "Success" << endl
			 << "Total step=" << totalstep;
		// 非常重要：删去全部
		clear();
		Sleep(2000);
		cout << "\033c";
		pre();
	}
}
void pre234()
{
	inputofinstructions();

	// 最最重要:开始根据用户输入的指令执行动画//每执行一步就应该判断outboxshuzu里边的当前元素和预期输出是否相同
	int now = 19, previous = 19; // 用>来显示当前进行到哪一条指令

	int i = 1; // 从第一个指令开始执行
	// 终止条件:进行到最后一步且最后一步不是jump        或者inbox但是左边传送带没有数字了
	while (true)
	{
		if (instruction[i].name == "inbox" && geichuinboxshuzuxiabiao >= levelinfo[level - 1].inboxnum)
		{
			// 正常结束的条件之一：inbox时左边没有元素
			break;
		}
		else if (i == instructionnum + 1)
			break; // 这个是正常进行完最后一步，如果是inbox outbox jump1，i从1到2到3到1，不可能到instructionnum+1，所以这种情况只会在inbox时左边没有东西结束
		gotoxy(65, previous);
		cout << " ";
		gotoxy(65, now);
		cout << ">";

		// 解析指令名称和操作数
		instructionNum = {};
		size_t pos = instruction[i].name.find(' ');
		string instructionName = instruction[i].name;
		if (pos != string::npos)
		{
			string operand = instruction[i].name.substr(pos + 1);
			if (isValidInteger(operand))
			{
				// 确保操作数是纯整数
				instructionNum.ifhave = 1;
				instructionNum.num = stoi(operand);
				instructionName = instruction[i].name.substr(0, pos);

				// 检查操作数是否在有效范围内
				if (instructionName == "jump" || instructionName == "jumpifzero")
				{
					if (instructionNum.num < 1 || instructionNum.num > instructionnum || instructionNum.num == i)
					{
						gotoxy(0, 0);
						clearScreen();
						gotoxy(0, 0);
						cout << "Error on instruction " << i;
						clear();
						Sleep(3000);
						pre();
						return;
					}
				}
				else if (instructionName == "add" || instructionName == "sub" || instructionName == "copyto" || instructionName == "copyfrom")
				{
					if (instructionNum.num < 0 || instructionNum.num > 2)
					{
						gotoxy(0, 0);
						clearScreen();
						gotoxy(0, 0);
						cout << "Error on instruction " << i;
						clear();
						Sleep(3000);
						pre();
						return;
					}
				}
			}

			else
			{
				// 操作数不是纯数字，报错
				gotoxy(0, 0);
				clearScreen();
				gotoxy(0, 0);
				cout << "Error on instruction " << i << ": Invalid operand." << endl;
				clear();
				Sleep(3000);
				pre();
				return;
			}
		}

		// 检查指令是否合法
		if ((instructionName == "inbox" || instructionName == "outbox") && instructionNum.ifhave == 0)
		{
			instruction[i].ifused++;
			previous = now;
			now++;
			robotmove(instructionName, i);
			i++;
		}
		else if (instructionName == "jump" && instructionNum.ifhave == 1)
		{
			// jump是没有条件地跳
			instruction[i].ifused++;
			previous = now;
			now = instructionNum.num + 18;
			i = instructionNum.num; // 跳转到指定的指令位置
		}
		// jumpifzero是有条件地跳
		else if (instructionName == "jumpifzero" && instructionNum.ifhave == 1)
		{
			if (currentblock.ifhave == 0)
			{
				ifnormalend = 0; // 如果没有当前积木，按异常情况处理
				break;
			}
			if (currentblock.num == 0)
			{
				// 如果有当前积木而且为0，则jump
				instruction[i].ifused++;
				previous = now;
				now = instructionNum.num + 18;
				i = instructionNum.num; // 跳转到指定的指令位置
			}
			else // 如果有当前积木但是不为0，那就不跳

			{
				instruction[i].ifused++;
				previous = now;
				now++;
				i++;
			}
		}
		else if ((instructionName == "add" || instructionName == "sub" || instructionName == "copyto" || instructionName == "copyfrom") && instructionNum.ifhave == 1)
		{
			instruction[i].ifused++;
			previous = now;
			now++;
			robotmove(instructionName, i);
			i++;
		}
		else
		{
			ifnormalend = 0;
			break;
		} // 如果输入的不在指令集中，就按异常退出处理
	}

	// 如果中途异常情况退出了
	if (ifnormalend == 0)
	{
		// 先清空
		clear();
		gotoxy(0, 0);
		clearScreen();
		gotoxy(0, 0);
		cout << "Error on instruction " << i;
		Sleep(3000);
		pre();
		return;
	}
	// 如果正常结束，判断输出是否相同
	// 总共进行的指令数
	for (int i = 1; i <= instructionnum; ++i)
	{
		totalstep += instruction[i].ifused;
	}
	// 如果正常结束，检测用户输出是否全部正确，若是，提示成功并且存储通关信息//如果异常结束，应该根本不会进行到这一步才对
	bool iftrulysyccess = 1;
	for (int i = 0; i < levelinfo[level - 1].outboxnum; ++i)
	{
		if (outboxshuzu[i + 1] != levelinfo[level - 1].yuqishuchu[i])
		{
			// 注意outbooxshuzu下标从1开始,yuqishuchu下标从0开始
			iftrulysyccess = 0;
			break;
		}
	}

	if (iftrulysyccess == 0 || outboxshuzuxiabiao - 1 != levelinfo[level - 1].outboxnum)
	{
		// 如果输出与预期输出不同或者输出个数!=预期输出的个数
		clearScreen();
		gotoxy(0, 0);
		cout << "Fail" << endl
			 << "Total step=" << totalstep;
		clear();
		Sleep(3000);
		pre();
	}
	else
	{
		if (level == 2)
		{
			level2str = "succeed";
			if (id == 1)
				id1lev2 = level2str;
			else if (id == 2)
				id2lev2 = level2str;
			else if (id == 3)
				id3lev2 = level2str;
		}
		else if (level == 3)
		{
			level3str = "succeed";
			if (id == 1)
				id1lev3 = level3str;
			else if (id == 2)
				id2lev3 = level3str;
			else if (id == 3)
				id3lev3 = level3str;
		}

		else if (level == 4)
		{
			level4str = "succeed";
			if (id == 1)
				id1lev4 = level4str;
			else if (id == 2)
				id2lev4 = level4str;
			else if (id == 3)
				id3lev4 = level4str;
		}

		storelevelinformation();
		clearScreen();
		gotoxy(0, 0);
		cout << "Success" << endl
			 << "Total step=" << totalstep;
		// 非常重要：删去全部
		clear();
		Sleep(2000);
		cout << "\033c";
		pre();
	}
}
void clear()
{
	speed = 0;
	instructionNum = {};
	currentblock = {};
	for (int i = 0; i < 30; ++i)
		outboxshuzu[i] = {};
	outboxshuzuxiabiao = 1;
	geichuinboxshuzuxiabiao = 0;
	inputmethod = {};
	filename = {};
	totalstep = 0;
	for (int i = 0; i < 105; ++i)
		instruction[i] = {}; // 清空
}
void custom()
{
	gotoxy(0, 2);
	cout << "Please input your level mission";
	gotoxy(0, 3);
	string mission;
	getline(cin, mission);
	levelinfo[4].levelMission = mission;

	gotoxy(0, 9);
	cout << "Please enter your file name. The file should correctly contain the number of input conveyor belts, input sequence, number of output conveyor belts  target output sequence and the number of available blanks.";
	gotoxy(0, 15);
	getline(cin, customfilename); // 读取文件名

	fin.open(customfilename);

	if (!fin.is_open())
	{
		system("cls");
		gotoxy(0, 0);
		cerr << "Error: Could not open file " << customfilename << endl;
		Sleep(2000);
		system("cls");
		clear();
		pre();
		return;
	}

	fin >> levelinfo[4].inboxnum;

	for (int i = 0; i < levelinfo[4].inboxnum; ++i)
		fin >> levelinfo[4].geichuinboxshuzu[i];
	// fin.ignore(std::numeric_limits< streamsize >::max(),'\n');

	fin >> levelinfo[4].outboxnum;

	for (int i = 0; i < levelinfo[4].outboxnum; ++i)
		fin >> levelinfo[4].yuqishuchu[i]; //

	fin >> levelinfo[4].availileblanknum;
	fin.close();

	clearScreen();
	inputofinstructions();
	// 最最重要:开始根据用户输入的指令执行动画//每执行一步就应该判断outboxshuzu里边的当前元素和预期输出是否相同
	int now = 19, previous = 19; // 用>来显示当前进行到哪一条指令

	int i = 1; // 从第一个指令开始执行
	// 终止条件:进行到最后一步且最后一步不是jump        或者inbox但是左边传送带没有数字了
	while (true)
	{
		if (instruction[i].name == "inbox" && geichuinboxshuzuxiabiao >= levelinfo[level - 1].inboxnum)
		{
			// 正常结束的条件之一：inbox时左边没有元素
			break;
		}
		else if (i == instructionnum + 1)
			break; // 这个是正常进行完最后一步，如果是inbox outbox jump1，i从1到2到3到1，不可能到instructionnum+1，所以这种情况只会在inbox时左边没有东西结束
		gotoxy(65, previous);
		cout << " ";
		gotoxy(65, now);
		cout << ">";

		// 解析指令名称和操作数
		instructionNum = {};
		size_t pos = instruction[i].name.find(' ');
		string instructionName = instruction[i].name;
		if (pos != string::npos)
		{
			string operand = instruction[i].name.substr(pos + 1);
			if (isValidInteger(operand))
			{
				// 确保操作数是纯整数
				instructionNum.ifhave = 1;
				instructionNum.num = stoi(operand);
				instructionName = instruction[i].name.substr(0, pos);

				// 检查操作数是否在有效范围内
				if (instructionName == "jump" || instructionName == "jumpifzero")
				{
					if (instructionNum.num < 1 || instructionNum.num > instructionnum || instructionNum.num == i)
					{
						gotoxy(0, 0);
						clearScreen();
						gotoxy(0, 0);
						cout << "Error on instruction " << i;
						clear();
						Sleep(3000);
						pre();
						return;
					}
				}
				else if (instructionName == "add" || instructionName == "sub" || instructionName == "copyto" || instructionName == "copyfrom")
				{
					if (instructionNum.num < 0 || instructionNum.num > levelinfo[4].availileblanknum - 1)
					{
						gotoxy(0, 0);
						clearScreen();
						gotoxy(0, 0);
						cout << "Error on instruction1 " << i;
						clear();
						Sleep(3000);
						pre();
						return;
					}
				}
			}

			else
			{
				// 操作数不是纯数字，报错
				gotoxy(0, 0);
				clearScreen();
				gotoxy(0, 0);
				cout << "Error on instruction " << i << ": Invalid operand." << endl;
				clear();
				Sleep(3000);
				pre();
				return;
			}
		}

		// 检查指令是否合法
		if ((instructionName == "inbox" || instructionName == "outbox") && instructionNum.ifhave == 0)
		{
			instruction[i].ifused++;
			previous = now;
			now++;
			robotmove(instructionName, i);
			i++;
		}
		else if (instructionName == "jump" && instructionNum.ifhave == 1)
		{
			// jump是没有条件地跳
			instruction[i].ifused++;
			previous = now;
			now = instructionNum.num + 18;
			i = instructionNum.num; // 跳转到指定的指令位置
		}
		// jumpifzero是有条件地跳
		else if (instructionName == "jumpifzero" && instructionNum.ifhave == 1)
		{
			if (currentblock.ifhave == 0)
			{
				ifnormalend = 0; // 如果没有当前积木，按异常情况处理
				break;
			}
			if (currentblock.num == 0)
			{
				// 如果有当前积木而且为0，则jump

				instruction[i].ifused++;
				previous = now;
				now = instructionNum.num + 18;
				i = instructionNum.num; // 跳转到指定的指令位置
			}
			else // 如果有当前积木但是不为0，那就不跳

			{
				instruction[i].ifused++;
				previous = now;
				now++;
				i++;
			}
		}
		else if ((instructionName == "add" || instructionName == "sub" || instructionName == "copyto" || instructionName == "copyfrom") && instructionNum.ifhave == 1)
		{
			instruction[i].ifused++;
			previous = now;
			now++;
			robotmove(instructionName, i);
			i++;
		}
		else
		{
			ifnormalend = 0;
			break;
		} // 如果输入的不在指令集中，就按异常退出处理
	}

	// 如果中途异常情况退出了
	if (ifnormalend == 0)
	{
		// 先清空
		clear();
		gotoxy(0, 0);
		clearScreen();
		gotoxy(0, 0);
		cout << "Error on instruction " << i;
		Sleep(3000);
		pre();
		return;
	}
	// 如果正常结束，判断输出是否相同
	// 总共进行的指令数
	for (int i = 1; i <= instructionnum; ++i)
	{
		totalstep += instruction[i].ifused;
	}
	// 如果正常结束，检测用户输出是否全部正确，若是，提示成功并且存储通关信息//如果异常结束，应该根本不会进行到这一步才对
	bool iftrulysyccess = 1;
	for (int i = 0; i < levelinfo[level - 1].outboxnum; ++i)
	{
		if (outboxshuzu[i + 1] != levelinfo[level - 1].yuqishuchu[i])
		{
			// 注意outbooxshuzu下标从1开始,yuqishuchu下标从0开始
			iftrulysyccess = 0;
			break;
		}
	}

	if (iftrulysyccess == 0 || outboxshuzuxiabiao - 1 != levelinfo[level - 1].outboxnum)
	{
		// 如果输出与预期输出不同或者输出个数!=预期输出的个数
		clearScreen();
		gotoxy(0, 0);
		cout << "Fail" << endl
			 << "Total step=" << totalstep;
		clear();
		Sleep(3000);
		pre();
	}

	clearScreen();
	gotoxy(0, 0);
	cout << "Success" << endl
		 << "Total step=" << totalstep;
	clear();
	Sleep(2000);
	cout << "\033c";
	pre();
}

