#include "stdafx.h"
#pragma once
BOOL WINAPI PerformHooks();
__declspec(dllexport) __declspec(noinline) VOID test(VOID);
void hookFunctionByName(PIMAGE_IMPORT_DESCRIPTOR importDescriptor, char *functionName, DWORD hookAddress, LPVOID * savedAddress);
PIMAGE_IMPORT_DESCRIPTOR WINAPI GetFirstImportDescriptor();

// It is recommended to typedef the function poiinter for the functions you hook
typedef VOID(WINAPI *FUNC_ExitProcess) (UINT);