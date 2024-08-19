#pragma once
#include "include/cef_resource_handler.h"

class LocalResourceHandler : public CefResourceHandler {
public:
    LocalResourceHandler(const std::wstring& root);

    bool Open(
        CefRefPtr<CefRequest> request,
        bool& handle_request,
        CefRefPtr<CefCallback> callback
    ) override;

    void GetResponseHeaders(
        CefRefPtr<CefResponse> response,
        int64& response_length,
        CefString& redirectUrl
    ) override;

    bool Skip(
        int64 bytes_to_skip,
        int64& bytes_skipped,
        CefRefPtr<CefResourceSkipCallback> callback
    ) override;

    bool Read(
        void* data_out,
        int bytes_to_read,
        int& bytes_read,
        CefRefPtr<CefResourceReadCallback> callback
    ) override;

    void Cancel() override;

private:
    CefRefPtr<CefStreamReader> reader = NULL;
    CefString mime{};

    std::wstring root{};
    int status = 500;

    IMPLEMENT_REFCOUNTING(LocalResourceHandler);
    DISALLOW_COPY_AND_ASSIGN(LocalResourceHandler);
};