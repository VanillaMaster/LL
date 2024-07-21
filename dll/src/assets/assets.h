#pragma once

#include "include/capi/cef_parser_capi.h"
#include "include/capi/cef_scheme_capi.h"

struct AssetsSchemeHandlerFactory {

	cef_scheme_handler_factory_t factory{};

	cef_string_t root{};

	int count = 0;

	AssetsSchemeHandlerFactory(const cef_string_t& root);
};

void RegisterAssetsSchemeHandlerFactory();