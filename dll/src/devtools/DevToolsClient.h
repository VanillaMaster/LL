#pragma once

#include "include/capi/cef_client_capi.h"

struct DevToolsClient {
	_cef_client_t client {};

	int count = 0;

	DevToolsClient(int parent);
	~DevToolsClient();
};