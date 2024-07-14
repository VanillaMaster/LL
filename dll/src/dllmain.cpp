#include <windows.h>
#include <iostream>

#include "d3d9/d3d9.h"
#include "renderer/renderer.h"
#include "browser/browser.h"

bool endswith(const std::wstring& source, const std::wstring& suffix) {
	return source.compare(source.length() - suffix.length(), suffix.length(), suffix) == 0;
}

void init() {
	WCHAR path[MAX_PATH]{};
	int length = GetModuleFileNameW(nullptr, path, MAX_PATH);
	std::wstring str(path, length);
	
	if (endswith(str, L"LeagueClientUx.exe")) {
		/*
		if (hookOnBrowser()) {
			MessageBox(NULL, L"???", L"err", MB_OK);
		}
		*/
	} else if (endswith(str, L"LeagueClientUxRender.exe")) {
		if (hookOnRenderer()) {
			MessageBox(NULL, L"???", L"err", MB_OK);
        }
	}
	
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {

    switch (ul_reason_for_call) {
		case DLL_PROCESS_DETACH:
            free_d3d9();
			break;
		
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hModule);
			load_d3d9();
			init();
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
    }
    return TRUE;
}