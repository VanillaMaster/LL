#pragma once

#include <filesystem>

#include "include/capi/cef_parser_capi.h"
#include "include/capi/cef_scheme_capi.h"

struct AssetsSchemeHandlerFactory {

	cef_scheme_handler_factory_t factory{};

	std::filesystem::path root {};

	int count = 0;

	AssetsSchemeHandlerFactory(std::filesystem::path root);
};

void RegisterAssetsSchemeHandlerFactory();