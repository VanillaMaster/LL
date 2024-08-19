#include <windows.h>
#include <iostream>

#include <ios>
#include <fstream>

#include <thread>

#include "include/cef_version.h"

#include "d3d9/d3d9.h"
#include "renderer/renderer.h"
#include "browser/browser.h"
#include "location.h"

void CreateMessageBox(const wchar_t* lpText, const wchar_t* lpCaption) {
	std::thread msg([](const wchar_t* lpText, const wchar_t* lpCaption) {
		MessageBox(NULL, lpText, lpCaption, MB_OK);
	}, lpText, lpCaption);
	msg.detach();
}

bool endswith(const std::wstring& source, const std::wstring& suffix) {
	return source.compare(source.length() - suffix.length(), suffix.length(), suffix) == 0;
}

void init(HMODULE hModule) {

	HMODULE libcef = GetModuleHandleA("libcef.dll");
	if (libcef == NULL) {
		CreateMessageBox(L"unable to load 'libcef.dll'", L"Error");
		return;
	}
	const auto version_info = reinterpret_cast<decltype(&cef_version_info)>(GetProcAddress(libcef, "cef_version_info"));
	if (version_info == nullptr || version_info(0) != CEF_VERSION_MAJOR) {
		CreateMessageBox(L"'cef_version_info' not found", L"Error");
		return;
	}

	WCHAR pathBuffer[MAX_PATH]{};
	int length = GetModuleFileNameW(nullptr, pathBuffer, MAX_PATH);
	std::wstring path(pathBuffer, length);


	WCHAR locationBuffer[MAX_PATH]{};
	length = GetModuleFileNameW(hModule, locationBuffer, MAX_PATH);
	std::wstring location(locationBuffer, length);
	setLocation(location);


	if (endswith(path, L"LeagueClientUx.exe")) {
		if (hookOnBrowser(libcef)) {
			CreateMessageBox(L"LeagueClientUx", L"Error");
		}
	} else if (endswith(path, L"LeagueClientUxRender.exe")) {
		if (hookOnRenderer(libcef)) {
			CreateMessageBox(L"LeagueClientUxRender", L"Error");
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