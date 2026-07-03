#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
// 绘制界面
//void Show()
//{
//	// 备份环境值
//	int oldlinecolor = getlinecolor();
//	int oldbkcolor = getbkcolor();
//	int oldfillcolor = getfillcolor();
//
//	setlinecolor(LIGHTGRAY);		// 设置画线颜色
//	setbkcolor(0xeeeeee);			// 设置背景颜色
//	setfillcolor(0xeeeeee);			// 设置填充颜色
//	fillrectangle(left, top, right, bottom);
//	outtextxy(left + 10, top + 5, text);
//
//	// 恢复环境值
//	setlinecolor(oldlinecolor);
//	setbkcolor(oldbkcolor);
//	setfillcolor(oldfillcolor);
//}
void OnMessage(char text[])
{
	int left = 0, top = 0, right = 100, bottom = 100, maxlen = 400;
	setlinecolor(BLACK);			// 设置画线颜色
	setbkcolor(WHITE);				// 设置背景颜色
	setfillcolor(WHITE);			// 设置填充颜色
	fillrectangle(left, top, right, bottom);
	outtextxy(left + 10, top + 5, text);

	int width = textwidth(text);	// 字符串总宽度
	bool binput = true;				// 是否输入中

	ExMessage msg;
	while (binput)
	{
		while (binput && peekmessage(&msg, EX_MOUSE | EX_CHAR, false))	// 获取消息，但不从消息队列拿出
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				// 如果鼠标点击文本框外面，结束文本输入
				if (msg.x < left || msg.x > right || msg.y < top || msg.y > bottom)
				{
					binput = false;
					break;
				}
			}
			else if (msg.message == WM_CHAR)
			{
				size_t len = strlen(text);
				switch (msg.ch)
				{
				case '\b':				// 用户按退格键，删掉一个字符
					if (len > 0)
					{
						text[len - 1] = 0;
						width = textwidth(text);
						clearrectangle(left + 10 + width, top + 1, right - 1, bottom - 1);
					}
					break;
				case '\r':				// 用户按回车键，结束文本输入
				case '\n':
					binput = false;
					break;
				default:				// 用户按其它键，接受文本输入
					if (len < maxlen - 1)
					{
						text[len++] = msg.ch;
						text[len] = 0;
						//clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);	// 清除画的光标
						width = textwidth(text);				// 重新计算文本框宽度
						outtextxy(left + 10, top + 5, text);		// 输出新的字符串
					}
				}
			}
		}
		Sleep(20);
	}
	clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);	// 擦光标
}
