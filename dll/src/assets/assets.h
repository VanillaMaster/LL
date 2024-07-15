#include "include/capi/cef_parser_capi.h"
#include "include/capi/cef_scheme_capi.h"
#include "include/capi/cef_stream_capi.h"
#include "include/capi/cef_resource_handler_capi.h"

struct AssetsSchemeHandlerFactory {

	cef_scheme_handler_factory_t factory{};
	
	int count = 0;

	AssetsSchemeHandlerFactory() {
		factory.base.size = sizeof(AssetsSchemeHandlerFactory);

	}
};