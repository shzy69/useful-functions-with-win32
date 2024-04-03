#pragma once

#ifdef _WIN32
#  define NOMINMAX
#  define _WINSOCKAPI_
#  include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <string>
#include <vector>


/*
* Written by shzy.
* Happy for using.
*/


inline bool SetWin32Clipboard(std::string text)
{
	// Allocate a global memory object for the text.
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
	if (hglbCopy == NULL)
	{
		printf("GlobalAlloc failed.\n");
		return false;
	}

	// Lock the handle and copy the text to the buffer.
	void *dst = GlobalLock(hglbCopy);
	memcpy(dst, text.c_str(), text.size());
	((char*)dst)[text.size()] = 0; // null character
	GlobalUnlock(hglbCopy);

	// Open clipboard and place the handle on the clipboard.
	if (!OpenClipboard(NULL))
	{
		printf("OpenClipboard failed.\n");
		return false;
	}
	EmptyClipboard();
	// After SetClipboardData() is called, the system owns the memory handle.
	SetClipboardData(CF_TEXT, hglbCopy);
	CloseClipboard();

	return true;
}


inline std::vector<HWND> GetCurProcessTopLevelWindow()
{
	struct CBData
	{
		uint64_t process_id;
		std::vector<HWND> h_wnds;
	};

	auto EnumWindowsCallBack = [](HWND hWnd, LPARAM lParam)->BOOL
	{
		CBData *cb_data = (CBData*)lParam;
		unsigned long process_id = 0;
		::GetWindowThreadProcessId(hWnd, &process_id);
		if (cb_data->process_id != process_id)
		{
			//To continue enumeration, the callback function must return TRUE;
			//to stop enumeration, it must return FALSE.
			return TRUE;
		}
		cb_data->h_wnds.push_back(hWnd);
		//may not be only one top level window.
		return TRUE;
	};

	//EnumWindows
	CBData cb_data{};
	cb_data.process_id = ::GetCurrentProcessId();
	::EnumWindows(EnumWindowsCallBack, (LPARAM)&cb_data);

	return cb_data.h_wnds;
}


typedef uint16_t RESOURCE_ID;

inline void SetWindowIcon(RESOURCE_ID icon_id)
{
	auto hwnds = GetCurProcessTopLevelWindow();

	for (auto hWnd : hwnds)
	{
		// load icon
		HINSTANCE hInstance = ::GetModuleHandle(NULL);
		HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(icon_id));

		// change window's icon
		::SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		::SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	}

}



inline void OpenExplorer(std::string path) //shoule be ANSI coded
{
	for (auto &ch : path)
	{
		// on Windows, 
		// if use relative path, path which contains '/' would fail. 
		if (ch == '/')
		{
			ch = '\\';
		}
	}

	ShellExecuteA(NULL, "explore", path.c_str(), NULL, NULL, SW_SHOWNORMAL);

}


//Alloc console window with quick edit mode disable
inline void AllocConsoleWindow()
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	//disable Quick Edit mode
	HANDLE h_input = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(h_input, &mode);
	mode &= (~(DWORD)ENABLE_QUICK_EDIT_MODE);
	mode |= ENABLE_EXTENDED_FLAGS;
	SetConsoleMode(h_input, mode);
}



namespace sig_used
{
volatile sig_atomic_t global_sig_running_process = 1;
volatile sig_atomic_t global_sig_one_shot = 0;
void sig_INT_handler(int sig)
{
	switch (sig)
	{
	case SIGINT:
		global_sig_running_process = 0;
		if (global_sig_one_shot)
			signal(sig, SIG_DFL);
		else
			signal(sig, sig_INT_handler);
		break;
	default:
		break;
	}
}
}

/* Wait for Ctrl+C, the interrupt signal. */
/* Register the INTERRUPT signal */
inline void reg_INT_signal(bool one_shot = false)
{
	sig_used::global_sig_one_shot = one_shot;
	signal(SIGINT, sig_used::sig_INT_handler);
}
/* Wait the INTERRUPT signal */
inline void wait_INT_signal()
{
	while (sig_used::global_sig_running_process) std::this_thread::yield();
}




