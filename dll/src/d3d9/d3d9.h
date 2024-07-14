#include <windows.h>

void* WINAPI Direct3DCreate9(UINT SDKVersion);
HRESULT WINAPI Direct3DCreate9Ex(UINT SDKVersion, void** ppEx);
int WINAPI D3DPERF_BeginEvent(DWORD col, LPCWSTR wszName);
int WINAPI D3DPERF_EndEvent();
DWORD WINAPI D3DPERF_GetStatus();
void WINAPI D3DPERF_SetMarker(DWORD col, LPCWSTR wszName);

void load_d3d9();

void free_d3d9();