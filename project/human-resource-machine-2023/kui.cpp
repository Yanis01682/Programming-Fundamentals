#include"beng.h"
int levelsearch(char str[], int g = 0)
{
	ifstream fin("level.txt"); // 游戏系统文件，无须进行用户错误处理
	if (fin)
	{
		int index = 0;
		while (!fin.eof())
			fin >> str[index++]; // 输入的是ASCII码值！'+48'
	}
	fin.close();
	for (int i = 1; i < 5; i++) if (str[i] - 48) g = i;
	return g;//0,1,2,3,4
}
int click(MOUSEMSG m, int g)//实现按钮操作
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
	else g = 0;
	return g;
}