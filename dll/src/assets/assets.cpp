#include "assets.h"
#include "LocalResourceHandler.h"

#include "../location.h"

AssetsSchemeHandlerFactory::AssetsSchemeHandlerFactory(const std::wstring& root) {
    this->root = root;
}

CefRefPtr<CefResourceHandler> AssetsSchemeHandlerFactory::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    return new LocalResourceHandler(this->root);
}

void RegisterAssetsSchemeHandlerFactory() {
    CefRegisterSchemeHandlerFactory("https", "fs.local", new AssetsSchemeHandlerFactory(getLocation()));
}