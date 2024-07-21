#pragma once

#include "include/capi/cef_stream_capi.h"
#include "include/capi/cef_resource_handler_capi.h"

struct LocalResourceHandler {

	cef_resource_handler_t handler{};

	cef_string_t file{};

	cef_stream_reader_t* reader = nullptr;

	int count = 0;

	LocalResourceHandler(const cef_string_t& src);

};