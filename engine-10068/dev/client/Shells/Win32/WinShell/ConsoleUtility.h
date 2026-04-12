#pragma once

#include <Windows.h>

//   0 = 黑色      8 = 灰色
//   1 = 蓝色      9 = 淡蓝色
//   2 = 绿色      A = 淡绿色
//   3 = 浅绿色    B = 淡浅绿色
//   4 = 红色      C = 淡红色
//   5 = 紫色      D = 淡紫色
//   6 = 黄色      E = 淡黄色
//   7 = 白色      F = 亮白色

//   控制台颜色
enum class ConsoleColor
{
    enmCFC_Red = FOREGROUND_INTENSITY | FOREGROUND_RED,
    enmCFC_Green = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
    enmCFC_Blue = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
    enmCFC_Yellow = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
    enmCFC_Purple = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
    enmCFC_Cyan = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
    enmCFC_Gray = FOREGROUND_INTENSITY,
    enmCFC_White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    enmCFC_HighWhite = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    enmCFC_Black = 0,
};

class ConsoleUtility
{
public:
    static bool CreateConsole();
    static bool DestroyConsole();

private:
	static HANDLE ms_hOutput;
};