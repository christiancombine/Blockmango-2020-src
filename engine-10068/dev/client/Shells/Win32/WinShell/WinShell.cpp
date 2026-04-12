#include "stdafx.h"
#include "WinShell.h"
#include "Game.h"
#include "UI/Touch.h"
#include "Core.h"
#include "Object/Root.h"
#include "Win32SoundEngine.h"
#include "Audio/SoundSystem.h"
#include "ShellInterfaceWin32.h"
#include "ConsoleUtility.h"
#include <dbghelp.h>   
#include "Test.h"

#define MAX_LOADSTRING 100
//#define _ITERATOR_DEBUG_LEVEL 0
//#define MULTITHREA_RENDER

#if defined(_DEBUG) && (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#define USE_CONSOLE
#endif

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

float fAspectRaido = 2.0f / 3.0f;

int g_nWindowWidth = 896;
int g_nWindowHeight = 504;// int(800 * fAspectRaido + 10);

unsigned int g_hWnd = 0;
GameClient::CGame* g_pGame = NULL;
LORD::SoundEngine* g_pSoundEngine = NULL;
GameClient::ShellInterfaceWin32* shellInterface = NULL;

bool g_bMousePressed = false;
bool g_bMultiTouch = false;

HINSTANCE g_hInstance = NULL;
float g_TouchDownPositionX = 0;
float g_TouchDownPositionY = 0;

struct EVENTINFO
{
	EVENTINFO(UINT msg, WPARAM wP, LPARAM lP)
		: message(msg)
		, wParam(wP)
		, lParam(lP)
	{}

	UINT message;
	WPARAM wParam;
	LPARAM lParam;
};
typedef std::vector<EVENTINFO> EVENTLIST;
EVENTLIST g_EventList_first;
EVENTLIST g_EventList_second;
EVENTLIST g_EventList_thread;
EVENTLIST* g_CurEventList = &g_EventList_first;
EVENTLIST* g_SwapEventList = &g_EventList_second;
bool g_SwapEvent = false;
CRITICAL_SECTION g_CriticalSection;

using namespace LORD;

bool g_bDestroy = false;
HANDLE mainThread = 0;
HANDLE hThread = 0;
HANDLE hUpdateThread = 0;

static unsigned __stdcall UpdateThreadCallBackProc(void *pvArg)
{
	g_pGame->UpdateFiles();
	hUpdateThread = 0;
	return 0;
}

#ifdef MULTITHREA_RENDER

static unsigned __stdcall ThreadCallBackProc(void *pvArg)
{
	// ��ʼ��client
	g_pGame = LordNew GameClient::CGame;
	g_pGame->InitConfig("./");
	// 	g_pGame->CheckVersion("./");
	// 	g_pGame->UpdateFiles("./");
	if (g_pGame->CheckVersion("./") == 2)
	{
		hUpdateThread = (HANDLE)_beginthreadex(
			NULL,			        	// SD
			0,				        	// initial stack size
			UpdateThreadCallBackProc,			// thread function
			NULL, 				// thread argument
			0,				        	// creation option
			NULL						// thread identifier
			);
		if (!hUpdateThread)
		{
			MessageBox(0, "_beginthreadex fail! Thread::Init", "Error", 0);
			return false;
		}
	}

	while (hUpdateThread)
	{
		Sleep(10);
	}

	g_pSoundEngine = LordNew LORD::Win32SoundEngine;
	g_pGame->InitGame("./", "./", g_nWindowWidth, g_nWindowHeight, g_hWnd, g_pSoundEngine);
	while (!g_bDestroy)
	{
		g_pGame->MainTick();

		EnterCriticalSection(&g_CriticalSection);
		if (g_SwapEvent)
		{
			g_EventList_thread.assign(g_SwapEventList->begin(), g_SwapEventList->end());
			g_SwapEvent = false;
		}
		LeaveCriticalSection(&g_CriticalSection);

		for (EVENTLIST::iterator it = g_EventList_thread.begin(); it != g_EventList_thread.end(); ++it)
		{
			switch (it->message)
			{
			case WM_SIZE:
			{
				int nWidth = LOWORD(it->lParam);
				int nHeight = HIWORD(it->lParam);

				if (g_pGame)
				{
					g_pGame->OnSize(nWidth, nHeight);
				}
			}
			break;
			case WM_KEYDOWN:
			{
				if (it->wParam == VK_F1)
				{
					g_pSoundEngine->pauseBackgroundMusic();

				}
				if (it->wParam == VK_F2)
				{
					g_pSoundEngine->resumeBackgroundMusic();
				}
			}
			break;
			case WM_LBUTTONDOWN:
			{
				float nPositionX = (float)LOWORD(it->lParam);
				float nPositionY = (float)HIWORD(it->lParam);

				if (g_pGame)
				{
					//g_pGame->OnTouchDown(0, nPositionX, nPositionY);


					if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000))
					{
						int num = 2;
						intptr_t ids[] = { 0, 1 };

						float centerX = (float)g_nWindowWidth / 2;
						float centerY = (float)g_nWindowHeight / 2;

						float nPositionX2 = centerX + centerX - nPositionX;
						float nPositionY2 = centerY + centerY - nPositionY;

						float x[] = { nPositionX, nPositionX2 };
						float y[] = { nPositionY, nPositionY2 };

						g_pGame->handleTouchesBegin(num, ids, x, y);

						g_bMultiTouch = true;
					}
					else
					{
						int num = 1;
						intptr_t ids[] = { 0 };
						float x[] = { nPositionX };
						float y[] = { nPositionY };

						g_pGame->handleTouchesBegin(num, ids, x, y);

						g_bMultiTouch = false;
					}

					g_TouchDownPositionX = nPositionX;
					g_TouchDownPositionY = nPositionY;
					g_bMousePressed = true;
				}
			}
			break;

			case WM_LBUTTONUP:
			{
				float nPositionX = (float)LOWORD(it->lParam);
				float nPositionY = (float)HIWORD(it->lParam);

				if (g_pGame && g_bMousePressed)
				{
					//g_pGame->OnTouchUp(0, nPositionX, nPositionY);

					if (g_bMultiTouch)
					{
						int num = 2;
						intptr_t ids[] = { 0, 1 };

						float centerX = (float)g_nWindowWidth / 2;
						float centerY = (float)g_nWindowHeight / 2;

						float nPositionX2 = centerX + centerX - nPositionX;
						float nPositionY2 = centerY + centerY - nPositionY;

						float x[] = { nPositionX, nPositionX2 };
						float y[] = { nPositionY, nPositionY2 };

						g_pGame->handleTouchesEnd(num, ids, x, y);
					}
					else
					{
						int num = 1;
						intptr_t ids[] = { 0 };
						float x[] = { nPositionX };
						float y[] = { nPositionY };

						g_pGame->handleTouchesEnd(num, ids, x, y);
					}

					g_bMultiTouch = false;
					g_bMousePressed = false;
				}
			}
			break;

			case WM_MOUSEMOVE:
			{
				float nPositionX = (float)LOWORD(it->lParam);
				float nPositionY = (float)HIWORD(it->lParam);

				if (g_pGame && g_bMousePressed)
				{
					if (g_bMultiTouch)
					{
						int num = 2;
						intptr_t ids[] = { 0, 1 };

						float centerX = (float)g_nWindowWidth / 2;
						float centerY = (float)g_nWindowHeight / 2;

						float nPositionX2 = centerX + centerX - nPositionX;
						float nPositionY2 = centerY + centerY - nPositionY;

						float x[] = { nPositionX, nPositionX2 };
						float y[] = { nPositionY, nPositionY2 };

						g_pGame->handleTouchesMove(num, ids, x, y);
					}
					else
					{
						int num = 1;
						intptr_t ids[] = { 0 };
						float x[] = { nPositionX };
						float y[] = { nPositionY };

						g_pGame->handleTouchesMove(num, ids, x, y);
					}
				}
			}
			break;
			case WM_MOUSELEAVE:
			{
				float nPositionX = (float)LOWORD(it->lParam);
				float nPositionY = (float)HIWORD(it->lParam);

				if (g_pGame && g_bMousePressed)
				{
					if (g_bMultiTouch)
					{
						int num = 2;
						intptr_t ids[] = { 0, 1 };

						float centerX = (float)g_nWindowWidth / 2;
						float centerY = (float)g_nWindowHeight / 2;

						float nPositionX2 = centerX + centerX - nPositionX;
						float nPositionY2 = centerY + centerY - nPositionY;

						float x[] = { nPositionX, nPositionX2 };
						float y[] = { nPositionY, nPositionY2 };

						g_pGame->handleTouchesCancel(num, ids, x, y);
					}
					else
					{
						int num = 1;
						intptr_t ids[] = { 0 };
						float x[] = { nPositionX };
						float y[] = { nPositionY };

						g_pGame->handleTouchesCancel(num, ids, x, y);
					}

					g_bMousePressed = false;
				}
			}
			break;
			}
		}
		g_EventList_thread.clear();
	}

	g_pGame->DestroyGame();

	g_pSoundEngine->end();
	LordSafeDelete(g_pSoundEngine);

	LordDelete g_pGame;
	g_pGame = NULL;

	SetEvent(mainThread);

	return 0;
}
#endif // MULTITHREA_RENDER

VOID GenerateDump(PEXCEPTION_POINTERS pException) throw()
{
	// ������ʱ���ӡlua��ջ
	if (g_pGame)
	{
		g_pGame->printLuaStack();
	}

	LORD::String dumpPath = "CrashDump";
	CreateDirectory(dumpPath.c_str(), NULL);
	LORD::String curTime = LORD::StringUtil::Format("%s %s", __DATE__, __TIME__);
	curTime = LORD::StringUtil::Replace(curTime, ':', '_');
	LORD::String strDumpFile = dumpPath + "\\" + curTime + ".dmp";
	HANDLE   hFile = CreateFile(strDumpFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION   ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pException;
		ExInfo.ClientPointers = NULL;

		//   write   the   dump
		BOOL   bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		CloseHandle(hFile);
	}
}

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void constructgame()
{
	//	// ��ʼ��client
	g_pGame = new GameClient::CGame;

	// test��Ӧserverlist�ı�ע��test�ķ����������б���ֻ�ܿ���test�ķ�����
	g_pGame->setPlatformInfo("test");

	if (!PathUtil::IsDirExist("./document"))
	{
		PathUtil::CreateDir("./document");
	}


	g_pSoundEngine = LordNew LORD::Win32SoundEngine;
	shellInterface = LordNew GameClient::ShellInterfaceWin32;
	shellInterface->setWndHandle(g_hWnd);
}

void destructgame()
{
	g_pGame->DestroyGame();
	Sleep(200);
	g_pSoundEngine->end();
	LordSafeDelete(g_pSoundEngine);

	LordSafeDelete(shellInterface);
	g_pGame->setShellInterface(NULL);

	delete g_pGame;
	g_pGame = NULL;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

#ifndef _DEBUG
	__try
	{
#endif
		g_hInstance = hInstance;

		// ��ʼ��ȫ���ַ���
		LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadString(hInstance, IDC_WINSHELL, szWindowClass, MAX_LOADSTRING);
		MyRegisterClass(hInstance);

		// ִ��Ӧ�ó����ʼ��:
		if (!InitInstance(hInstance, nCmdShow))
		{
			return FALSE;
		}

		// 	MessageBox((HWND)g_hWnd, "��ʹ���ƶ�����������������", "����", 0);
		// 	return 0;

		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINSHELL));

#ifdef MULTITHREA_RENDER
		mainThread = CreateEvent(NULL, true, 0, NULL);

		InitializeCriticalSection(&g_CriticalSection);

		hThread = (HANDLE)_beginthreadex(
			NULL,			        	// SD
			0,				        	// initial stack size
			ThreadCallBackProc,			// thread function
			NULL, 				// thread argument
			0,				        	// creation option
			NULL						// thread identifier
			);
		if (!hThread)
		{
			MessageBox(0, "_beginthreadex fail! Thread::Init", "Error", 0);
			return false;
		}
#else

#ifdef _DEBUG_WINDOWS
		ConsoleUtility::CreateConsole();
		Test t;
#endif

		constructgame();

		g_pGame->InitConfig("./");
		/*g_pGame->CheckVersion("./");
		g_pGame->UpdateFiles("./");*/
		if (g_pGame->CheckVersion("./") == 2)
		{
			hUpdateThread = (HANDLE)_beginthreadex(
				NULL,			        	// SD
				0,				        	// initial stack size
				UpdateThreadCallBackProc,			// thread function
				NULL, 				// thread argument
				0,				        	// creation option
				NULL						// thread identifier
				);
			if (!hUpdateThread)
			{
				MessageBox(0, "_beginthreadex fail! Thread::Init", "Error", 0);
				return false;
			}
		}

		while (hUpdateThread)
		{
			Sleep(10);
		}

		g_pGame->setShellInterface(shellInterface);
		g_pGame->InitGame("./", "./document/", g_nWindowWidth, g_nWindowHeight, g_hWnd, g_pSoundEngine);

#endif //MULTITHREA_RENDER
		while (true)
		{
			// Handle the windows messages.
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					break;

				if (msg.message == WM_KEYDOWN && msg.wParam == VK_F8)
					break;
			}
			// If windows signals to end the application then exit out.
#ifdef MULTITHREA_RENDER
			if (msg.message == WM_QUIT)
			{
				g_bDestroy = true;  
				WaitForSingleObject(mainThread, INFINITE); 

				
				WaitForSingleObject(hThread, INFINITE);
				CloseHandle(hThread);
				CloseHandle(mainThread);
				hThread = 0;
				mainThread = 0;
				break;
			}

			EnterCriticalSection(&g_CriticalSection);
			if (!g_SwapEvent)
			{
				g_SwapEventList->assign(g_CurEventList->begin(), g_CurEventList->end());
				g_CurEventList->clear();
				g_SwapEvent = true;
			}
			LeaveCriticalSection(&g_CriticalSection);

			Sleep(10);
		}
		DeleteCriticalSection(&g_CriticalSection);
#else
			if (msg.message == WM_QUIT)
			{
				break;
			}

			if (msg.message == WM_KEYDOWN && msg.wParam == VK_F8)
			{
				g_pGame->DestroyGame();
				delete g_pGame;
				delete g_pSoundEngine;
				delete shellInterface;
				constructgame();
				g_pGame->InitConfig("./");
				g_pGame->setShellInterface(shellInterface);
				g_pGame->InitGame("./", "./document/", g_nWindowWidth, g_nWindowHeight, g_hWnd, g_pSoundEngine);
			}
			
			g_pGame->MainTick();
		}
		destructgame();
#endif

#ifdef _DEBUG_WINDOWS
		ConsoleUtility::DestroyConsole();
#endif

#ifndef _DEBUG
}
	__except (GenerateDump(GetExceptionInformation()), TerminateProcess(GetCurrentProcess(), 0), EXCEPTION_EXECUTE_HANDLER)
	{
	}
#endif

	return (int)msg.wParam;
}


void  relaunch()
{
	TCHAR moduleName[MAX_PATH];
	ZeroMemory(moduleName, sizeof(moduleName));
	GetModuleFileName(NULL, moduleName, MAX_PATH);

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	if (CreateProcess(NULL, moduleName, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{
		ExitProcess(0);
	}
}

//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINSHELL));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	std::ifstream _if("win_config.txt");
	String width, height;
	_if >> width;
	_if >> height;
	int pWidth = StringUtil::ParseI32(LORD::StringUtil::Split(width, "=")[1]);
	int pHeight = StringUtil::ParseI32(LORD::StringUtil::Split(height, "=")[1]);
	g_nWindowWidth = pWidth;
	g_nWindowHeight = pHeight;
	_if.close();

	HWND hWnd;

	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	int nPositionX = 0;
	int nPositionY = 0;

	// Place the window in the middle of the screen.
	nPositionX = (GetSystemMetrics(SM_CXSCREEN) - g_nWindowWidth) / 2;
	nPositionY = (GetSystemMetrics(SM_CYSCREEN) - g_nWindowHeight) / 2;

	RECT loRt;
	loRt.left = 0;
	loRt.top = 0;
	loRt.right = g_nWindowWidth;
	loRt.bottom = g_nWindowHeight;
	AdjustWindowRect(&loRt, WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_POPUP, FALSE);

	// Create the window with the screen settings and get the handle to it.
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, szWindowClass, "WinShell-Blockman",
		WS_OVERLAPPEDWINDOW,
		nPositionX, nPositionY, loRt.right - loRt.left, loRt.bottom - loRt.top,
		NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hWnd = (unsigned int)hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
	{
#ifdef MULTITHREA_RENDER
		EVENTINFO info(message, wParam, lParam);
		g_CurEventList->push_back(info);
#else
		int nWidth = LOWORD(lParam);
		int nHeight = HIWORD(lParam);

		if (g_pGame)
		{
			g_pGame->OnSize(nWidth, nHeight);
		}
#endif
	}
	break;
	case WM_KEYDOWN:
	{
		{
#ifdef MULTITHREA_RENDER
			EVENTINFO info(message, wParam, lParam);
			g_CurEventList->push_back(info);
#else
			if (wParam == VK_F1)
			{
				g_pSoundEngine->pauseBackgroundMusic();

			}
			if (wParam == VK_F2)
			{
				g_pSoundEngine->resumeBackgroundMusic();

			}
#endif
		}
		g_pGame->handleKeyDown(wParam);
	}
	break;
	case WM_KEYUP:
	{
		g_pGame->handleKeyUp(wParam);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (g_pGame)
		{
			float nPositionX = (float)LOWORD(lParam);
			float nPositionY = (float)HIWORD(lParam);

			int num = 1;
			intptr_t ids[] = { 0 };
			float x[] = { nPositionX };
			float y[] = { nPositionY };

			g_pGame->handleTouchesBegin(num, ids, x, y);
		}
	}
	break;

	case WM_LBUTTONUP:
	{
		float nPositionX = (float)LOWORD(lParam);
		float nPositionY = (float)HIWORD(lParam);

		if (g_pGame)
		{
			int num = 1;
			intptr_t ids[] = { 0 };
			float x[] = { nPositionX };
			float y[] = { nPositionY };

			g_pGame->handleTouchesEnd(num, ids, x, y);
		}
	}
	break;

	case WM_MOUSEMOVE:
	{
		float nPositionX = (float)LOWORD(lParam);
		float nPositionY = (float)HIWORD(lParam);
		bool hoveUI = false;

		if (g_pGame)
		{
			int num = 1;
			intptr_t ids[] = { 0 };
			float x[] = { nPositionX };
			float y[] = { nPositionY };

			g_pGame->handleTouchesMove(num, ids, x, y);

		}
		TRACKMOUSEEVENT lpET;
		lpET.cbSize = sizeof(TRACKMOUSEEVENT);
		lpET.dwFlags = TME_LEAVE;
		lpET.dwHoverTime = NULL;
		lpET.hwndTrack = (HWND)g_hWnd;
		TrackMouseEvent(&lpET);
	}
	break;
	case WM_MOUSELEAVE:
	{
		float nPositionX = (float)LOWORD(lParam);
		float nPositionY = (float)HIWORD(lParam);

		if (g_pGame)
		{
			int num = 1;
			intptr_t ids[] = { 0 };
			float x[] = { nPositionX };
			float y[] = { nPositionY };

			g_pGame->handleTouchesCancel(num, ids, x, y);
		}
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
