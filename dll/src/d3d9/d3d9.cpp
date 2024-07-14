#include "d3d9.h";

#define SUFFIX L"\\d3d9.dll"
#define SUFFIX_LENGTH sizeof(SUFFIX)

static HMODULE d3d9 = nullptr;

static FARPROC _Direct3DCreate9;
static FARPROC _Direct3DCreate9Ex;
static FARPROC _D3DPERF_BeginEvent;
static FARPROC _D3DPERF_EndEvent;
static FARPROC _D3DPERF_GetStatus;
static FARPROC _D3DPERF_SetMarker;

void load_d3d9() {
	WCHAR path[MAX_PATH];
	CopyMemory(path + GetSystemDirectory(path, MAX_PATH - SUFFIX_LENGTH), SUFFIX, SUFFIX_LENGTH);
	d3d9 = LoadLibrary(path);

	if (d3d9 == nullptr) ExitProcess(1);

	_Direct3DCreate9 = GetProcAddress(d3d9, "Direct3DCreate9");
	_Direct3DCreate9Ex = GetProcAddress(d3d9, "Direct3DCreate9Ex");
	_D3DPERF_BeginEvent = GetProcAddress(d3d9, "D3DPERF_BeginEvent");
	_D3DPERF_EndEvent = GetProcAddress(d3d9, "D3DPERF_EndEvent");
	_D3DPERF_GetStatus = GetProcAddress(d3d9, "D3DPERF_GetStatus");
	_D3DPERF_SetMarker = GetProcAddress(d3d9, "D3DPERF_SetMarker");
}

void free_d3d9() {
	FreeLibrary(d3d9);
}

void* WINAPI Direct3DCreate9(UINT SDKVersion) {
	return ((decltype(&Direct3DCreate9))_Direct3DCreate9)(SDKVersion);
}
HRESULT WINAPI Direct3DCreate9Ex(UINT SDKVersion, void** ppEx) {
	return ((decltype(&Direct3DCreate9Ex))_Direct3DCreate9Ex)(SDKVersion, ppEx);
}
int WINAPI D3DPERF_BeginEvent(DWORD col, LPCWSTR wszName) {
	return ((decltype(&D3DPERF_BeginEvent))_D3DPERF_BeginEvent)(col, wszName);
}
int WINAPI D3DPERF_EndEvent() {
	return ((decltype(&D3DPERF_EndEvent))_D3DPERF_EndEvent)();
}
DWORD WINAPI D3DPERF_GetStatus() {
	return ((decltype(&D3DPERF_GetStatus))_D3DPERF_GetStatus)();
}
void WINAPI D3DPERF_SetMarker(DWORD col, LPCWSTR wszName) {
	((decltype(&D3DPERF_SetMarker))_D3DPERF_SetMarker)(col, wszName);
}