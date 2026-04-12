#include "stdafx.h"
#include "ConsoleUtility.h"
#include <corecrt_io.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>

HANDLE ConsoleUtility::ms_hOutput{nullptr};

bool ConsoleUtility::CreateConsole()
{
    if (AllocConsole())
    {
		ms_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

        freopen("conout$", "w", stdout);
        freopen("conout$", "w", stderr);

        TCHAR title[] = "Output Debug Window";
        SetConsoleTitle(title);
        SetConsoleTextAttribute((HANDLE)ms_hOutput,
            ((DWORD)ConsoleColor::enmCFC_Black | (DWORD)ConsoleColor::enmCFC_Black));

        HWND hwnd = NULL;
        while (NULL == hwnd)
            hwnd = ::FindWindow(NULL, (LPCTSTR)title);

        HMENU hmenu = ::GetSystemMenu(hwnd, FALSE);
        DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);

        return true;
    }

    return false;
}

bool ConsoleUtility::DestroyConsole()
{
    FreeConsole();
    return true;
}