#pragma once
/*
#include "include/capi/cef_scheme_capi.h"

struct ModuleResolverSchemeHandlerFactory {

	cef_scheme_handler_factory_t factory{};

	cef_string_t root{};

	int count = 0;

	ModuleResolverSchemeHandlerFactory();
};
*/


#include "include/cef_scheme.h"
class ModuleResolverSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
	ModuleResolverSchemeHandlerFactory();

	CefRefPtr<CefResourceHandler> Create(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& scheme_name,
		CefRefPtr<CefRequest> request
	) override;
private:
	IMPLEMENT_REFCOUNTING(ModuleResolverSchemeHandlerFactory);
	DISALLOW_COPY_AND_ASSIGN(ModuleResolverSchemeHandlerFactory);
};