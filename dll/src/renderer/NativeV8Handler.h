#include "include/capi/cef_v8_capi.h"

struct NativeV8Handler {
	cef_v8handler_t handler {};

	int count = 0;

	NativeV8Handler();
};