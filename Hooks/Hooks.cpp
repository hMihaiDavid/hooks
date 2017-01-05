#include "stdafx.h"
#include "Hooks.h"

static LPVOID savedAddr; // the saved address of the original function hooked.

extern HMODULE hModule; // base address of module to hook. We get this in DllMain. It is the original EXE of the process.


VOID WINAPI hook_ExitProcess(
	_In_ UINT uExitCode
) {
	MessageBoxA(NULL, "ExitProcess was hooked!", NULL, MB_ICONINFORMATION | MB_OK);
	
	FUNC_ExitProcess originalExitProcess = (FUNC_ExitProcess)savedAddr;
	originalExitProcess(uExitCode); // Call the original ExitProcess
}


BOOL WINAPI PerformHooks()
{

	PIMAGE_IMPORT_DESCRIPTOR FirstImportDescriptor = GetFirstImportDescriptor();
	if (!FirstImportDescriptor)
		return FALSE;

	hookFunctionByName(FirstImportDescriptor, "ExitProcess", (DWORD) &hook_ExitProcess, &savedAddr);
	
	/* 
		As you can see, in order to hook a function you first need to get a pointer to the firt import descriptor of the module you want to hook.
		Then to perform each hook you only need to call hookFunctionByName and pass to it the name of the import you want to hook, a DWORD with
		the address of the function to execute instead of the original one and last but not least a pointer to a DWORD where the original oaddress
		of the hooked function is going to be stored. If this parameter is NULL, the address is not stored anywhere.
		If you want your hook function to be able to call the original function you can use this saved address, or you could import that function in this
		DLL and call it, since the IAT table of this module is not touched, only that of the main executable in the process adress space. However,
		you need to save the original address if you want to restore it later in order to disable the hook.

		To disable a hook and restore the function to the original one, you would need to call hookFunctionByName again, this time passing 
		the saved address you got from the first call as the address of the hook. For example, to disable the hook for ExitProcess you would need to
		call now:

		hookFunctionByName(FirstImportDescriptor, "ExitProcess", (DWORD) savedAddr, NULL);
	*/
	
	return TRUE;
}

PIMAGE_IMPORT_DESCRIPTOR WINAPI GetFirstImportDescriptor() {

	PIMAGE_NT_HEADERS ntHeader;
	PIMAGE_IMPORT_DESCRIPTOR FirstImportDescriptor;

	if (((PIMAGE_DOS_HEADER)hModule)->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	ntHeader = (PIMAGE_NT_HEADERS)((BYTE*)hModule + ((PIMAGE_DOS_HEADER)hModule)->e_lfanew); // hModule is a global variable
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE)											// it refers to EXE module
		return NULL;
	FirstImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	if (!FirstImportDescriptor)
		return NULL;
	FirstImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)FirstImportDescriptor + (UINT_PTR)hModule);
	return FirstImportDescriptor;
}

void hookFunctionByName(PIMAGE_IMPORT_DESCRIPTOR importDescriptor, char *functionName, DWORD hookAddress, LPVOID *savedAddress) {

	PIMAGE_THUNK_DATA originalFirstThunk;		// RVA to name
	PIMAGE_THUNK_DATA firstThunk;				// RVA to IAT ENTRY
	PIMAGE_IMPORT_BY_NAME importByName;
	LPCSTR importName;
	DWORD oldProtectionFlags;

	while (importDescriptor->OriginalFirstThunk) {

		originalFirstThunk = (PIMAGE_THUNK_DATA)importDescriptor->OriginalFirstThunk;
		originalFirstThunk = (PIMAGE_THUNK_DATA)((BYTE*)originalFirstThunk + (UINT_PTR)hModule); // pointer arithmetic is a bitch
		firstThunk = (PIMAGE_THUNK_DATA)importDescriptor->FirstThunk;
		firstThunk = (PIMAGE_THUNK_DATA)((BYTE*)firstThunk + (UINT_PTR)hModule);

		while (originalFirstThunk->u1.Function) {
			importByName = (PIMAGE_IMPORT_BY_NAME)originalFirstThunk->u1.AddressOfData;
			importByName = (PIMAGE_IMPORT_BY_NAME)((BYTE*)importByName + ((UINT_PTR)hModule));
			importName = (LPCSTR)((BYTE*)importByName + sizeof(WORD));

			if (strcmp(importName, functionName ) == 0) {
				if (savedAddress)
					*savedAddress = (LPVOID)firstThunk->u1.Function;

				VirtualProtect(&firstThunk->u1.Function, sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &oldProtectionFlags);
				firstThunk->u1.Function = hookAddress;
				VirtualProtect(&firstThunk->u1.Function, sizeof(LPVOID), oldProtectionFlags, NULL);
			}


			originalFirstThunk++;
			firstThunk++;
		}

		importDescriptor++;
	}
}

__declspec(dllexport) __declspec(noinline) VOID test(VOID) { return; }