#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"Winmm.lib")
#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
using namespace std;


//定义变量
char filename[100];     // 指令文件名
int guanqia;            // 第几关
char level_[6] = { 0 };   // 记录level
int you[50] = { 0 };      // 右侧输出数组，因指令而变
int zuo[50] = { 0 };      // 左侧传送带
int xia[5] = { 0 };       // 下方空地
int box[2] = { 0 };       // 机器人头顶盒子，box[1]存储盒子的值
int ans[50] = { 0 };      // 每一关期待答案,与you[]进行比对
char instruc[100][15];  // 输入指令集合,从1开始使用
int zhiling[100] = { 0 }; // 指令转数字,从1开始使用
int M = 0;              // 指令数量
int g_imgPosx = 0; //图像x坐标
int g_imgPosy = 0; //图像y坐标
int ren;//身份选择


//函数声明
int click(MOUSEMSG m,int g);//初始页面左键单击按钮进入页面、右键投放有趣的东西
int levelsearch(char str[],int g);//读取level.txt中保存的关卡信息
int click2(MOUSEMSG m, int guanqia,int g1);//选择关卡页面
int renwu(MOUSEMSG m, IMAGE human1, IMAGE human2, IMAGE human3, IMAGE xuanrentishi);//选择人物
bool ifPass(int you[50], int ans[50]); // 过关判断:是true，否false
void baocun(int guanqia, char level[]);// 通关写入文件保存
void error(char instruc[100][15], int zhiling[], int M); //将指令转化成数字形式
int readHow(char instruc[100][15], int M);// 用户选择输入指令方式并进行输入
void daoxu(int you[]); // 把结果中不是-1000的部分倒序，便于与ans[]比较
//第二三四关函数
int mainCase(int guanqia, char instruc[100][15], int zhiling[], int zuo[], int you[], int box[], int xia[], int ans[], int M, int ren, IMAGE ren1, IMAGE ren2, IMAGE ren3, IMAGE case1, IMAGE shi);//第234关主函数
void getin(int zuo[]);  // 输入传送带运转
int getout(int you[], int M);// 输出传送带运转
void inbox(int zuo[], int you[], int xia[], int box[]);// 输入数字
void add(int zuo[], int you[], int xia[], int box[], int gewei);// 加
void sub(int zuo[], int you[], int xia[], int box[], int gewei);// 减
void copyto(int zuo[], int you[], int xia[], int box[], int gewei);// 改变空地数字
void copyfrom(int zuo[], int you[], int xia[], int box[1], int gewei);// 从空地获取
void outbox(int zuo[], int you[], int xia[], int box[]);// 输出
int duoshao(int you[], int n);//计数：右侧有几个非-1000的数
void show(int ren, IMAGE ren1, IMAGE ren2, IMAGE ren3, IMAGE case1, int zuo[], int you[], int xia[], int box[], int x1, int x2, int guanqia, int zhiling[], int i, int M, IMAGE shi);//显示
void showbefore(int zhiling[], int i, int M, IMAGE shi);//打印指令和标志符号


//主函数
int main() 
{
   mciSendString(_T("open music.mp3 alias bkmusic"), NULL, 0, NULL);
    mciSendString(_T("play bkmusic repeat"), NULL, 0, NULL);
	initgraph(640, 480, EW_SHOWCONSOLE);
    setbkcolor(BLACK);
    settextcolor(WHITE);
    settextstyle(26, 0, "楷体");
//必须保留项目------------------------------------------------------------------------------
//图片+欢迎文字（生硬，加音效/换图）
	IMAGE img1,img2; //创建图像对象
	loadimage(&img1, "images/humanresource.png"); //加载图像：地址、名称、（x大小、y大小）
	loadimage(&img2, "images/welcome.png");//项目-属性P-高级-字符集-使用Unicobe字符集
	putimage(0, 0, &img1); //在(x, y)绘制humanresources，之后可能有一种模式
	putimage(170, 370, &img2);
	Sleep(10000);//休眠1000毫秒后进入游戏
	cleardevice();
CHUSHI:
//图片加载
	IMAGE button,shi,choose0,choose1,choose2,choose3,choose4,help,developer,human1,human2,human3,xuanrentishi,case1,ren1,ren2,ren3;
	loadimage(&button, "images/button1.png", 640, 480);//图片占据全页面
	loadimage(&shi, "images/shi.png", 30, 30);//图片小小的
	loadimage(&choose0, "images/0.png", 640, 480);
	loadimage(&choose1, "images/1.png", 640, 480);
	loadimage(&choose2, "images/2.png", 640, 480);
	loadimage(&choose3, "images/3.png", 640, 480);
	loadimage(&choose4, "images/4.png", 640, 480);
	loadimage(&help, "images/help.png", 640, 1708);
	loadimage(&developer, "images/developer.png", 640, 854);
	loadimage(&human1, "images/human1.png", 100, 90);
	loadimage(&human2, "images/human2.png", 100, 90);
	loadimage(&human3, "images/human3.png", 100, 90);
	loadimage(&xuanrentishi, "images/xuanrentishi.png", 640, 360);
    loadimage(&case1, "images/case.png", 640, 480);
    loadimage(&ren1, "images/ren1.png", 45, 45);
    loadimage(&ren2, "images/ren2.png", 45, 45);
    loadimage(&ren3, "images/ren3.png", 45, 45);
    putimage(0, 0, &button, SRCINVERT);//从原点开始放置
	MOUSEMSG m;
	m = GetMouseMsg();//创建鼠标对象
//交互式按钮	
   int g = 0;//初始化页面
		while (!g)//按下左键跳转页面
		{
			m = GetMouseMsg();
			if (m.uMsg == WM_RBUTTONDOWN) //按下右键放置依托勾史
				putimage(m.x, m.y, &shi, SRCINVERT);
			g=click(m,0);
		 }
		cleardevice();
//进入选择关卡/HELP/开发者界面
	switch (g)
	{
	case 5://选择关卡：成/败后goto返回此处
	{
    XUANZEGUANQIA:
        cleardevice();//clear
		guanqia = 0;//初始化
		int g2 = levelsearch(level_,0);
		if (g2 == 0) putimage(0, 0, &choose0, SRCINVERT);
		else if(g2==1) putimage(0, 0, &choose1, SRCINVERT);
		else if(g2==2) putimage(0, 0, &choose2, SRCINVERT);
		else if(g2==3) putimage(0, 0, &choose3, SRCINVERT);
		else putimage(0, 0, &choose4, SRCINVERT);
		while (!guanqia)
		{
			m = GetMouseMsg();
			if (m.uMsg == WM_RBUTTONDOWN) //按下右键放置依托勾史
				putimage(m.x, m.y, &shi, SRCINVERT);
			guanqia = click2(m, 0, g2);
		}
		Sleep(100);//可改
		cleardevice();
		break;
	}
	case 6://HELP：游戏按钮功能介绍、关卡介绍：用户需要则goto返回此处，看完则goto进入初始页面
	{
		bool panduan = true;
		while (panduan)
		{
			if (GetAsyncKeyState(VK_UP) & 0x8000)//键盘"^"键
				g_imgPosy--;
			putimage(g_imgPosx, g_imgPosy, &help); //在起始坐标为(0, 0)的位置绘制图像img
			Sleep(16);
			if (g_imgPosy < -1228) panduan = false;//如果图像看完，就退出循环
		}
		g_imgPosx = 0, g_imgPosy = 0;//用完及时清零
		cleardevice();
		goto CHUSHI;
	}
	case 7://关于开发者、开发环境、开发经历介绍：看完则goto进入初始页面
	{
		bool panduan = true;
		while (panduan)
		{
			if (GetAsyncKeyState(VK_UP) & 0x8000)//键盘"^"键
				g_imgPosy--;
			putimage(g_imgPosx, g_imgPosy, &developer); //在起始坐标为(0, 0)的位置绘制图像img
			Sleep(16);
			if (g_imgPosy < -374) panduan = false;//如果图像看完，就退出循环
		}
		g_imgPosx = 0, g_imgPosy = 0;//用完及时清零
		cleardevice();
		goto CHUSHI;
	}
    }
//四大关卡
    if (guanqia != -1)
    { 
        ren = renwu(m, human1, human2, human3, xuanrentishi); 
    HAHA:
        putimage(0, 0, &case1, SRCINVERT);//每一次清屏后得重新贴上！！
        outtextxy(0,168, "接下来请移步终端进行指令的输入等操作");
        outtextxy(0, 196, "遇到难题请移步HELP寻找TIPS");
    }
    if (guanqia == 1)
        {
            for (int i = 0; i < 50; i++)
                you[i] = -1000, zuo[i] = -1000, ans[i] = -1000;
            box[1] = -1000;
            zuo[1] = 1, zuo[2] = 2, ans[1] = 1, ans[2] = 2;
            outtextxy(0, 0, "输入序列为1，2");
            outtextxy(0, 28, "让机器人取出输入序列上的每个积木放进输出序列中");
            outtextxy(0, 56, "可用指令集为inbox,outbox");
            outtextxy(0, 84, "可用空地数为0");
            Sleep(10000);
            cleardevice();
            int suibian = 1;
            while (suibian)
            {
                putimage(0, 0, &case1, SRCINVERT);
                M = readHow(instruc, M);
                if (M > 999)
                {
                    cleardevice();
                    outtextxy(0, 0, "代码量过大！退回重来！");
                }
                else if (M < 1)
                {
                    cleardevice();
                    outtextxy(0, 0, "耍我呢？退回重来！");
                }
                else suibian = 0;
            }
            error(instruc, zhiling, M); // 指令转数字，从zhiling[1]开始的
            show(ren, ren1, ren2, ren3, case1, zuo,you, xia, box, 120, 120, 1, zhiling, 0, M,shi);
            for (int i = 1; i <= M; i++)
            {
                if ((zhiling[i] / 1000 == 1) || (zhiling[i] / 1000 == 2 && box[1] != -1000))
                {
                    if (zhiling[i] / 1000 == 1)
                    {
                        if (zuo[1] == -1000) // 执行 inbox 指令时，输入传送带上没有积木
                            break;
                        else
                        {
                            inbox(zuo, you, xia, box);
                            getin(zuo);
                            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 120, 120, 1, zhiling, i, M, shi);
                        }
                    }
                    else
                    {
                        M=getout(you,M);
                       show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 120, 480 , 1, zhiling, i, M, shi);
                       outbox(zuo, you, xia, box);
                       show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 480, 120, 1, zhiling, i, M, shi);
                    }
                }
                else // 3~8指令/outbox但box为空
                {
                    outtextxy(0, 0, "Error on instruction ");
                    char temp[20];
                    sprintf(temp, "%d", i);
                    outtextxy(300, 0, temp);//变量转为char
                    outtextxy(0, 40, "认真工作！退回重来！");
                    Sleep(3000);
                    cleardevice();
                    goto HAHA;
                }
            }
            daoxu(you);// 3. 结果倒序
            bool k = ifPass(you, ans); // 检验成败+用户决定是否继续玩
            if (k)
            {
                baocun(guanqia, level_);
                outtextxy(0, 0, "Sucess");
            }
            else
            {
                outtextxy(0,0, "Fail");
            }
            Sleep(3000);
            cleardevice();
            goto XUANZEGUANQIA;
        }
    else if (guanqia == 2)
        {
            for (int i = 0; i < 50; i++)
                you[i] = -1000,
                zuo[i] = -1000, ans[i] = -1000;
            box[1] = -1000;
            zuo[1] = 3, zuo[2] = 9, zuo[3] = 5, zuo[4] = 1, zuo[5] = -2, zuo[6] = -2, zuo[7] = 9, zuo[8] = -9;
            ans[1] = -6, ans[2] = 6, ans[3] = 4, ans[4] = -4, ans[5] = 0, ans[6] = 0, ans[7] = 18, ans[8] = -18;
            xia[1] = -1000, xia[2] = -1000, xia[3] = -1000, xia[0] = -1000;
            outtextxy(0, 0, "输入序列为3,9,5,1,-2,-2,9,-9");
            outtextxy(0, 28, "对于输入序列中的每两个东西，先把第一个减去第二个，并把结果放在输出序列中；");
            outtextxy(0, 56, "然后把第二个减去第一个，把结果放在输出序列中，重复");
            outtextxy(0, 84, "可用指令集为inbox,outbox,add,sub");
            outtextxy(0, 112, "jump, jumpifzero, copyto, copyfrom");
            outtextxy(0, 140, "可用空地数为3");
            Sleep(10000);
            cleardevice();
            putimage(0, 0, &case1, SRCINVERT);
            //大函数
            mainCase(guanqia, instruc, zhiling, zuo, you, box, xia, ans, M, ren, ren1, ren2, ren3, case1, shi);
            Sleep(10000);
            cleardevice();
            goto XUANZEGUANQIA;
        }
    else if (guanqia == 3)
        {
            for (int i = 0; i < 50; i++)
                you[i] = -1000,
                zuo[i] = -1000, ans[i] = -1000;
            box[1] = -1000;
            zuo[1] = 6, zuo[2] = 2, zuo[3] = 7, zuo[4] = 7, zuo[5] = -9, zuo[6] = 3, zuo[7] = -3, zuo[8] = -3;
            ans[1] = 7, ans[2] = -3;
            xia[1] = -1000, xia[2] = -1000, xia[3] = -1000;
            outtextxy(0, 0, "输入序列为6,2,7,7,-9,-3,-3");
            outtextxy(0, 28, "从输入序列中依次取两个数，相等则输出一个，否则扔掉");
            outtextxy(0, 56, "可用指令集为inbox,outbox,add,sub");
            outtextxy(0, 84, "jump, jumpifzero, copyto, copyfrom");
            outtextxy(0, 112, "可用空地数为3");
            Sleep(10000);
            cleardevice();
            putimage(0, 0, &case1, SRCINVERT);
            //大函数
            mainCase(guanqia, instruc, zhiling, zuo, you, box, xia, ans, M, ren, ren1, ren2, ren3, case1, shi);
            Sleep(10000);
            cleardevice();
            goto XUANZEGUANQIA;
        }
    else if (guanqia == 4)
        {
            for (int i = 0; i < 50; i++)
                you[i] = -1000, zuo[i] = -1000, ans[i] = -1000;
            box[1] = -1000;
            zuo[1] = 1, zuo[2] = 2, zuo[3] = 3, zuo[4] = 4, zuo[5] = 6, zuo[6] = 7, zuo[7] = -1, zuo[8] = 2, zuo[9] = 1, zuo[10] = 0, zuo[11] = -10, zuo[12] = 8;
            ans[1] = 3, ans[2] = 1;
            xia[1] = -1000, xia[2] = -1000, xia[3] = -1000, xia[4] = -1000, xia[0] = -1000;
            outtextxy(0, 0, "输入序列为1，2，3，4，6，7，-1，2，1，0，-10，8");
            outtextxy(0, 28, "依次取三个数，前两个相加等于第三个则输出第三个数，否则扔掉");
            outtextxy(0, 56, "可用指令集为inbox,outbox,add,sub");
            outtextxy(0, 84, "jump, jumpifzero, copyto, copyfrom");
            outtextxy(0, 112, "可用空地数为3");
            Sleep(10000);
            cleardevice();
            putimage(0, 0, &case1, SRCINVERT);
            //大函数
            mainCase(guanqia, instruc, zhiling, zuo, you, box, xia, ans, M, ren, ren1, ren2, ren3, case1, shi);
            Sleep(10000);
            cleardevice();
            goto XUANZEGUANQIA;
        }
    else
        {
            //预计再来一幅退出图片
            return 0;
        }
//必须保留项目------------------------------------------------------------------------------
   mciSendString(_T("close bkmusic"), NULL, 0, NULL);
    int abcdefghijklmn=_getch();//解决返回值被忽略报警，无意义，只为安心
	closegraph();
	return 0;
}


//函数定义
int renwu(MOUSEMSG m, IMAGE human1, IMAGE human2, IMAGE human3,IMAGE xuanrentishi)
{
	int g = 0,haha=1;
	putimage(0, 0, &xuanrentishi);
	putimage(30, 360, &human1);
	putimage(270, 360, &human2);
	putimage(510, 360, &human3);
	while (haha)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONUP && m.y > 360 && m.y < 450)
		{
			if (m.x > 30 && m.x < 130) 
			{ 
				g = 1, haha = 0;
				fillrectangle(30, 360, 130, 450);
			}
			if (m.x > 270 && m.x < 370)
			{
				g = 2, haha = 0;
				fillrectangle(270, 360, 370, 450);
			}
			if (m.x > 510 && m.x < 610)
			{
				g = 3, haha = 0;
				fillrectangle(510, 360, 610, 450);
			}
		}
	}
	Sleep(2000);
	cleardevice();//万事先清屏
	return g;
}
int levelsearch(char str[],int g=0)
{
	ifstream fin("level.txt"); // 游戏系统文件，无须进行用户错误处理
	if (fin)
	{
		int index = 0;
		while (!fin.eof())
			fin >> str[index++]; // 输入的是ASCII码值！'+48'
	}
	fin.close();
	for (int i = 1; i < 5; i++) if (str[i]-48) g = i;
	return g;//0,1,2,3,4
}
int click2(MOUSEMSG m, int guanqia, int g1)
{
	if(m.y>110 && m.y<200&& m.uMsg == WM_LBUTTONUP)
	{ 
		if(m.x<120 && m.x>10) 
		{
			fillrectangle(10, 110, 110, 200);
			Sleep(3000);
			guanqia=1;
		}
		if(m.x<245 && m.x>150 && g1>0)
		{
			fillrectangle(150, 110, 245, 200);
			Sleep(3000);
			guanqia = 2;
		}
		if (m.x < 370 && m.x>275 && g1 > 1)
		{
			fillrectangle(275, 110, 370, 200);
			Sleep(3000);
			guanqia = 3;
		}
		if (m.x < 500 && m.x>400 && g1 > 2)
		{
			fillrectangle(400,110, 500, 200);
			Sleep(3000);
			guanqia = 4;
		}
		if (m.x<625 && m.x>560)
		{
			fillrectangle(560, 110, 625, 200);
			Sleep(3000);
			guanqia = -1;
		}
	}
	return guanqia;
}
int click(MOUSEMSG m,int g)//实现按钮操作
{
	if (m.y < 120 && m.y > 20 && m.x < 540 && m.x>100)
	{
		if (m.uMsg == WM_LBUTTONUP)//不可删，否则死循环
		{
			fillrectangle(100, 25, 540, 125);//x起、y起、x终、y终
			Sleep(3000);//3秒后跳转
			g = 5;
			//outtextxy(100, 100, "窗口一尝试");//项目-属性P-高级-字符集-使用多字节字符集
		}
	}
	else if (m.y < 285 && m.y > 185 && m.x < 540 && m.x>100)
	{
		if (m.uMsg == WM_LBUTTONUP)
		{
			fillrectangle(100, 185, 540, 285);
			Sleep(3000);
			g = 6;
		}
	}
	else if (m.y > 350 && m.y < 450 && m.x < 540 && m.x>100)
	{
		if (m.uMsg == WM_LBUTTONUP)
		{
			fillrectangle(100, 350, 540, 450);
			Sleep(3000);
			g = 7;
		}
	}
	else g=0;
	return g;
}
void getin(int zuo[]) // 传送带运转
{
    for (int i = 1; i < 50; i++)
        zuo[i] = zuo[i + 1];
}
void inbox(int zuo[], int you[], int xia[], int box[])
{
    // robot
    box[1] = zuo[1];
    // robot
}
void add(int zuo[], int you[], int xia[], int box[], int gewei)
{
    // robot
    box[1] = box[1] + xia[gewei];
    // robot
}
void sub(int zuo[], int you[], int xia[], int box[], int gewei)
{
    // robot
    box[1] = box[1] - xia[gewei];
    // robot
}
void copyto(int zuo[], int you[], int xia[], int box[], int gewei)
{
    // robot
    xia[gewei] = box[1];
    // robot
}
void copyfrom(int zuo[], int you[], int xia[], int box[1], int gewei)
{
    // robot
    box[1] = xia[gewei];
    // robot
}
void outbox(int zuo[], int you[], int xia[], int box[])
{
    // robot
    you[1] = box[1];
    box[1] = -1000; // 给出后box为空，否则无法满足题设"错误情况"
    // robot
}
int mainCase(int guanqia,char instruc[100][15], int zhiling[], int zuo[], int you[], int box[], int xia[], int ans[], int M,int ren,IMAGE ren1,IMAGE ren2,IMAGE ren3,IMAGE case1,IMAGE shi)
{
    int pan = 1;
    int suibian = 1;
    while (suibian)
    {
        M = readHow(instruc, M);
        if (M > 999)
        {
            cleardevice();
            outtextxy(0, 0, "代码量过大！退回重来！");
        }
        else if (M < 1)
        {
            cleardevice();
            outtextxy(0, 0, "耍我呢？退回重来！");
        }
        else suibian = 0;
    }
    error(instruc, zhiling, M); // 指令转数字，从zhiling[1]开始的
    // 2. 逐条运行指令
    show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 120, 120, guanqia, zhiling, 0, M, shi);
    for (int i = 1; i <= M;)
    {
        int shiwei = zhiling[i] / 1000, gewei = zhiling[i] % 1000;
        if (shiwei == 1) // inbox
        {
            if (zuo[1] == -1000) // 执行 inbox 指令时，输入传送带上没有积木
                break;
            else
            {
                inbox(zuo, you, xia, box);
                getin(zuo);
                show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 120, 120, guanqia, zhiling, i, M, shi);
            }
        }
        else if (shiwei == 2 && box[1] != -1000) // outbox
        {
            M=getout(you,M);
            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 120, 480, guanqia, zhiling, i, M, shi);
            outbox(zuo, you, xia, box);            
            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 480, 120, guanqia, zhiling, i, M, shi);
        }
        else if (shiwei == 3 && (gewei >= 0 && gewei <= 2) && box[1] != -1000 && xia[gewei] != -1000) // add
        {
            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 120, 200+gewei*40, guanqia, zhiling, i, M, shi);
            add(zuo, you, xia, box, gewei);
            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 200 + gewei * 40, 120, guanqia, zhiling, i, M, shi);
        }
        else if (shiwei == 4 && (gewei >= 0 && gewei <= 2) && box[1] != -1000 && xia[gewei] != -1000) // sub
        {
            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 120, 200 + gewei * 40, guanqia, zhiling, i, M, shi);
            sub(zuo, you, xia, box, gewei);
            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 200 + gewei * 40, 120, guanqia, zhiling, i, M, shi);
        }
        else if (shiwei == 5 && (gewei >= 0 && gewei <= 2) && box[1] != -1000) // copyto
        {
            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 120, 200 + gewei * 40, guanqia, zhiling, i, M, shi);
           copyto(zuo, you, xia, box, gewei);
            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 200 + gewei * 40, 120, guanqia, zhiling, i, M, shi);
        }
        else if (shiwei == 6 && (gewei >= 0 && gewei <= 2) && xia[gewei] != -1000) // copyfrom
        {
            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 120, 200 + gewei * 40, guanqia, zhiling, i, M, shi);
            copyfrom(zuo, you, xia, box, gewei);
            show(ren, ren1, ren2, ren3, case1, zuo, you, xia, box, 200 + gewei * 40, 120, guanqia, zhiling, i, M, shi);
        }
        else if (shiwei == 7 && (gewei <= M && gewei >= 1)) // jump
        {
            daoxu(you);
            bool duan = ifPass(you, ans);
            if (duan)
            {
                daoxu(you);
                break;
            }
            else
            {
                daoxu(you);
                i = gewei-1;
            }
        }
        else if (shiwei == 8 && (gewei <= M && gewei >= 1) && box[1] != -1000) // jumpifzero
        {
            if (box[1] == 0)
            {
                daoxu(you);
                bool duan = ifPass(you, ans);
                if (duan)
                {
                    daoxu(you);
                    break;
                }
                else
                {
                    daoxu(you);
                    i = gewei - 1;
                }
            }
        }
        else
        {
            outtextxy(0, 0, "Error on instruction ");
            char temp[20];
            sprintf(temp, "%d", i);
            outtextxy(300, 0, temp);//变量转为char
            outtextxy(0, 40, "认真工作！退回重来！");
            Sleep(50);
            pan = 0;
            return 0;
        }
        i++;
    }
    if (pan)
    {
        daoxu(you);
        bool k = ifPass(you, ans);
        if (k)
        {
            baocun(guanqia, level_);
            outtextxy(0, 0, "Sucess");
        }
        else
        {
            outtextxy(0, 0, "Fail");
        }
    }
    return 1;
}
bool ifPass(int you[50], int ans[50])
{
    int g = 1;
    for (int i = 0; i < 50; i++)
        if (you[i] != ans[i])
            g = 0;
    if (g == 1)
        return true;
    else
        return false;
}
void baocun(int guanqia, char level[])
{
    level[guanqia] = (guanqia + 48);
    string file_name = "level.txt";
    ofstream out(file_name, ios_base::out);
    for (int i = 0; i < 6; i++)
        out << level[i] << endl;
    out.close();
}
int readHow(char instruc[100][15], int M)
{
    int m = 0;
    char go[50] = { 0 };
   cout << "亲亲(づ￣ 3￣)づ,想怎么输入指令呢？" << endl;
    cout << "***键盘输入请按1 , 文件输入请按2***" << endl;
    cin >> go;
    while (go[0] < 48 || go[0] > 57)
    {
        cout << "只能输入数字呦~~" << endl;
        cin >> go;
    }
    m= go[0] - 48;
    while (m != 1 && m != 2)
    {
        cout << "输入指令不对捏~(￣▽￣)~*请重新输入" << endl;
        cin >> go;
        while (go[0] < 48 || go[0] > 57)
        {
            cout << "只能输入数字呦~~" << endl;
            cin >> go;
        }
        m = go[0] - 48;
    }
    if (m == 1) // 键盘输入
    {
        cout << "公主请键入指令~o(=∩ω∩=)m" << endl;
        cout << "请输入指令数量为:" << endl;
        cin >> instruc[0];
        while (instruc[0][0] < 48 || instruc[0][0] > 57 || (instruc[0][1] != '\0' && (instruc[0][1] < 48 || instruc[0][1] > 57)))
        {
            cout << "只能输入数字呦~~" << endl;
            cin >> instruc[0];
        }
        if (instruc[0][1] == '\0' && instruc[0][0] >= 48 && instruc[0][0] <= 57)
            M = instruc[0][0] - 48;
        else if (instruc[0][0] >= 48 && instruc[0][0] <= 57 && instruc[0][1] >= 48 && instruc[0][1] <= 57)
            M = (instruc[0][0] - 48) * 10 + instruc[0][1] - 48;
        cout << "现在输入指令:指令之间用回车隔开,指令内部允许空格呦,请使用小写字母和自然数！" << endl;
        for (int i = 1; i <= M; i++) // 注意，从1开始使用数组!
            scanf("\n%[^\n]", instruc[i]);
    }
    else // 文件读入
    {
        
        cout << "输入的文件需要由 M+1 行组成,第一行为指令数量M;第 2 至 M+1 行,每行包含一条指令,允许指令内部的空格呦;仅支持小写字母和自然数" << endl;
    WENJIAN:
        cout << "请输入你要打开的文件名及路径,如temp.txt" << endl;
        gets_s(filename, sizeof(filename));

        FILE* fin;
        gets_s(filename, sizeof(filename));

        fin = fopen(filename, "r");
        while (!fin) // 文件打开不成功，则提示用户重新输入文件名
        {
            cout << "┗|`O`|┛ 嗷~~,文件名错误或者文件不存在哦,请重新输入" << endl;
            gets_s(filename, sizeof(filename));

            fin = fopen(filename, "r");
        }
        int i = 0;
        while (fscanf(fin, "\n%[^\n]", instruc[i]) != EOF)
            i++;
        fclose(fin);
        if (instruc[0][1] == '\0' && instruc[0][0] >= 48 && instruc[0][0] <= 57)
            M = instruc[0][0] - 48;
        else if (instruc[0][0] >= 48 && instruc[0][0] <= 57 && instruc[0][1] >= 48 && instruc[0][1] <= 57)
            M = (instruc[0][0] - 48) * 10 + instruc[0][1] - 48;
        else 
        { 
            cout << "指令形式不合法，请重新输入" << endl; 
            goto WENJIAN;
        }
    }
    system("cls");
    return M;
}
void error(char instruc[100][15], int zhiling[], int M)
{
    int j = 1;
    for (int i = 1; i <= M; i++)
    {
        if (strcmp(instruc[i], "inbox") == 0)
            zhiling[j] = 1000;
        else if (strcmp(instruc[i], "outbox") == 0)
            zhiling[j] = 2000;
        else if (instruc[i][0] == 'a' && instruc[i][1] == 'd' && instruc[i][2] == 'd' && instruc[i][5] == '\0')
            zhiling[j] = 3000 + instruc[i][4] - 48;
        else if (instruc[i][0] == 's' && instruc[i][1] == 'u' && instruc[i][2] == 'b' && instruc[i][5] == '\0')
            zhiling[j] = 4000 + instruc[i][4] - 48;
        else if (instruc[i][0] == 'c' && instruc[i][1] == 'o' && instruc[i][2] == 'p' && instruc[i][3] == 'y' && instruc[i][4] == 't' && instruc[i][5] == 'o' && instruc[i][8] == '\0')
            zhiling[j] = 5000 + instruc[i][7] - 48;
        else if (instruc[i][0] == 'c' && instruc[i][1] == 'o' && instruc[i][2] == 'p' && instruc[i][3] == 'y' && instruc[i][4] == 'f' && instruc[i][5] == 'r' && instruc[i][6] == 'o' && instruc[i][7] == 'm' && instruc[i][10] == '\0')
            zhiling[j] = 6000 + instruc[i][9] - 48;
        else if (instruc[i][0] == 'j' && instruc[i][1] == 'u' && instruc[i][2] == 'm' && instruc[i][3] == 'p' && (instruc[i][5] == '\0' || instruc[i][6] == '\0'))
        {
            if (instruc[i][6] == '\0' && instruc[i][5] <= '9' && instruc[i][5] >= '1')
                zhiling[j] = 7000 + instruc[i][5] - 48;
            else if (instruc[i][7] == '\0' && instruc[i][5] <= '9' && instruc[i][5] >= '1' && instruc[i][6] <= '9' && instruc[i][6] >= '1')
                zhiling[j] = 7000 + (instruc[i][5] - 48) * 10 + instruc[i][6] - 48; \
            else zhiling[j] = 0;
        }
        else if (instruc[i][0] == 'j' && instruc[i][1] == 'u' && instruc[i][2] == 'm' && instruc[i][3] == 'p' && instruc[i][4] == 'i' && instruc[i][5] == 'f' && instruc[i][6] == 'z' && instruc[i][7] == 'e' && instruc[i][8] == 'r' && instruc[i][9] == 'o' && (instruc[i][12] == '\0' || instruc[i][13] == '\0'))
        {
            if (instruc[i][12] == '\0' && instruc[i][11] <= '9' && instruc[i][11] >= '1')
                zhiling[j] = 8000 + instruc[i][11] - 48;
            else if (instruc[i][13] == '\0' && instruc[i][11] <= '9' && instruc[i][11] >= '1' && instruc[i][12] <= '9' && instruc[i][12] >= '1')
                zhiling[j] = 8000 + (instruc[i][11] - 48) * 10 + instruc[i][12] - 48;
            else zhiling[j] = 0;
        }
        else
            zhiling[j] = 0;
        if ((zhiling[j] % 1000 > 3 || zhiling[j] % 1000 < 0) && zhiling[j]/1000!=7 && zhiling[j]/1000!=8)
            zhiling[j] = 0;
        j++;
    }
}
int duoshao(int you[], int n)
{
    for (int i = 1; i < 50; i++)
        if (you[i] != -1000)
            n++;
    return n;
}
void daoxu(int you[])
{
    int shu = duoshao(you, 0);
    for (int i = 1; i <= (shu / 2); i++)
    {
        int temp = you[i];
        you[i] = you[shu - i + 1];
        you[shu - i + 1] = temp;
    }
}
void showbefore(int zhiling[],int i,int M, IMAGE shi)//指令打在屏幕上不要变
{
    for (int k = 1; k <= M; k++)
    {
        int shiwei = zhiling[k] / 1000;
        int gewei = zhiling[k] % 1000;
        char num[20];
        sprintf(num, "%d", gewei);
        if (shiwei == 1) outtextxy(350, 28 * (k - 1), "inbox");
        else if (shiwei == 2) outtextxy(350, 28*(k-1), "outbox");
        else if (shiwei == 3) { outtextxy(350, 28 * (k - 1), "add"); outtextxy(470, 28 * (k - 1), num);}
        else if (shiwei == 4) { outtextxy(350, 28 * (k - 1), "sub"); outtextxy(470, 28 * (k - 1), num); }
        else if (shiwei == 5) { outtextxy(350, 28 * (k - 1), "copyto"); outtextxy(470, 28 * (k - 1), num); }
        else if (shiwei == 6) { outtextxy(350, 28 * (k - 1), "copyfrom");  outtextxy(470, 28 * (k - 1), num); }
        else if (shiwei == 7) { outtextxy(350, 28 * (k - 1), "jump");  outtextxy(470, 28 * (k - 1), num);}
        else if (shiwei == 8) { outtextxy(350, 28 * (k - 1), "jumpifzero");  outtextxy(470, 28 * (k - 1),num);}
    }
    for(int k=1;k<=M;k++) if (k == i) putimage(320, 28 * (i - 1), &shi, SRCINVERT);//i是从1开始的
}
void show(int ren,IMAGE ren1,IMAGE ren2,IMAGE ren3,IMAGE case1, int zuo[], int you[], int xia[], int box[], int x1,int x2,int guanqia, int zhiling[], int i,int M,IMAGE shi)//从何处到何处
{
    putimage(0, 0, &case1);
        int flagvx = 0;
        if (x2 - x1 <= 0)
            flagvx = -1;
        if (x2 - x1 > 0)
            flagvx = 1;
        double x = x1;
        int aaaa = 1;
        while (aaaa)
        {
            cleardevice();
            char num[20];
            putimage(0, 0, &case1);
            if(ren==1)putimage(x, 240, &ren1);
            else if (ren == 2)putimage(x, 240, &ren2);
            else putimage(x, 240, &ren3);
            sprintf(num, "%d", box[1]);//打印box
            if (box[1]!=-1000) { outtextxy(x + 10, 200, num); }
            else outtextxy(x + 10, 200, "X");
            showbefore(zhiling, i, M, shi);
            for (int i = 0; i < 5;i++)//打印zuo
            { 
                fillrectangle(60, 230 + i * 40, 100, 270 + i * 40);
                if (zuo[i + 1] != -1000)
                {
                    char num1[20];
                    sprintf(num1, "%d", zuo[i + 1]);
                    outtextxy(67, 230 + i * 40, num1);
                }
                else outtextxy(67, 230 + i * 40, "X");
            }
            for (int i = 0; i < 5; i++)//打印you
            {
                fillrectangle(540, 230 + i * 40, 580, 270 + i * 40);
                if (you[i+1]!=-1000)
                {
                    char num2[20];
                    sprintf(num2, "%d", you[i + 1]);
                    outtextxy(547, 230 + i * 40, num2);
                }
                else outtextxy(547, 230 + i * 40, "X");
            }
            if (guanqia!=1)
            {
                for (int i = 0; i < 3;i++)//打印xia
                {
                    fillrectangle(200+i*40, 350, 240+i*40, 390);
                    if (xia[i] != -1000)
                    {
                        char num3[20];
                        sprintf(num3, "%d", xia[i]);
                        outtextxy(200+i*40, 357, num3);
                    }
                    else outtextxy(200 + i * 40, 357, 'X');
                }
            }
            Sleep(40);
            x += 5*flagvx;
            //判断循环结束条件
            if (flagvx == 1)
            {
                if (x >= x2)
                    aaaa = 0;
            }
            else if (flagvx == -1)
            {
                if (x <= x2)
                    aaaa = 0;
            }
        }
        Sleep(2000);
}
int getout(int you[], int M)
{
    for (int i = 50; i >= 1; i--)
        you[i] = you[i - 1];
    return M;
}