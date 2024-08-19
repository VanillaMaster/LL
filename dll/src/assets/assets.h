#pragma once
#include "include/cef_scheme.h"

class AssetsSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
    AssetsSchemeHandlerFactory(const std::wstring& root);

    CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request
    ) override;

private:
    std::wstring root{};

    IMPLEMENT_REFCOUNTING(AssetsSchemeHandlerFactory);
    DISALLOW_COPY_AND_ASSIGN(AssetsSchemeHandlerFactory);
};

void RegisterAssetsSchemeHandlerFactory();