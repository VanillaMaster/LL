#include <windows.h>
#include <iostream>

#include "d3d9/d3d9.h"
#include "renderer/renderer.h"
#include "browser/browser.h"

#include <ios>
#include <fstream>

#include "location.h"

bool endswith(const std::wstring& source, const std::wstring& suffix) {
	return source.compare(source.length() - suffix.length(), suffix.length(), suffix) == 0;
}

void init(HMODULE hModule) {
	WCHAR pathBuffer[MAX_PATH]{};
	int length = GetModuleFileNameW(nullptr, pathBuffer, MAX_PATH);
	std::wstring path(pathBuffer, length);

	
	WCHAR locationBuffer[MAX_PATH]{};
	length = GetModuleFileNameW(hModule, locationBuffer, MAX_PATH);
	std::wstring location(locationBuffer, length);
	setLocation(location);
	

	if (endswith(path, L"LeagueClientUx.exe")) {
		if (hookOnBrowser()) {
			MessageBox(NULL, L"???", L"err", MB_OK);
		}
	} else if (endswith(path, L"LeagueClientUxRender.exe")) {
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
			init(hModule);
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
    }
    return TRUE;
}