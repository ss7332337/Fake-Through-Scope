#pragma once

#include <Windows.h>
#include <stdio.h>

#define check(expr) if (!(expr)){PrintErrorMessageToConsole(GetLastError()); DebugBreak(); exit(-1); }

inline void PrintErrorMessageToConsole(DWORD errorCode)
{
	//wchar_t errorBuf[1024];
	wchar_t errorBuf[1024];

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		errorBuf,
		1024,
		NULL);

	wprintf(L"Error: %i : %s", errorCode, errorBuf);
}

inline BOOL GetErrorMessage(DWORD dwErrorCode, LPTSTR pBuffer, DWORD cchBufferLength)
{
	if (cchBufferLength == 0)
	{
		return FALSE;
	}

	DWORD cchMsg = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,  /* (not used with FORMAT_MESSAGE_FROM_SYSTEM) */
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		pBuffer,
		cchBufferLength,
		NULL);
	return (cchMsg > 0);
}

