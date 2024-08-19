#include "LocalResourceHandler.h"
#include "include/cef_parser.h"

std::wstring GetMimeType(const std::wstring& resource_path) {
    std::wstring mime_type;
    size_t sep = resource_path.find_last_of(L"./");
    if (sep != std::string::npos && resource_path[sep] != L'/') {
        mime_type = CefGetMimeType(resource_path.substr(sep + 1));
        if (!mime_type.empty())
            return mime_type;
    }
    return L"application/octet-stream";
}

LocalResourceHandler::LocalResourceHandler(const std::wstring& root) {
    this->root = root;
}

bool LocalResourceHandler::Open(
    CefRefPtr<CefRequest> request,
    bool& handle_request,
    CefRefPtr<CefCallback> callback
) {
    handle_request = true;

    auto method = request->GetMethod();
    if (method == "GET") {
        CefURLParts parts{};
        CefParseURL(request->GetURL(), parts);
        std::wstring path(parts.path.str, parts.path.length);
        std::wstring location = this->root + path;

        this->reader = CefStreamReader::CreateForFile(location);
        if (this->reader != NULL) {
            this->status = 200;
            this->mime = GetMimeType(location);
        } else {
            this->status = 404;
        }
    } else {
        this->status = 405;
    }

    return true;
}

void LocalResourceHandler::GetResponseHeaders(
    CefRefPtr<CefResponse> response,
    int64& response_length,
    CefString& redirectUrl
) {
    response->SetHeaderByName("Access-Control-Allow-Origin", "*", true);
    response->SetStatus(this->status);

    if (this->status == 200) {
        response->SetMimeType(this->mime);
        response_length = -1;
    } else {
        response_length = 0;
    }
}

bool LocalResourceHandler::Skip(
    int64 bytes_to_skip,
    int64& bytes_skipped,
    CefRefPtr<CefResourceSkipCallback> callback
) {
    auto const code = this->reader->Seek(bytes_to_skip, SEEK_CUR);

    if (code != 0) {
        bytes_skipped = -2;
        return false;
    }

    bytes_skipped = bytes_to_skip;
    return true;
}

bool LocalResourceHandler::Read(
    void* data_out,
    int bytes_to_read,
    int& bytes_read,
    CefRefPtr<CefResourceReadCallback> callback
) {
    bytes_read = 0;
    if (this->reader == NULL) return false;
    int read = 0;
    do {
        read = static_cast<int>(
            this->reader->Read(static_cast<char*>(data_out) + bytes_read, 1,
                bytes_to_read - bytes_read));
        bytes_read += read;
    } while (read != 0 && bytes_read < bytes_to_read);

    return (bytes_read > 0);
}

void LocalResourceHandler::Cancel() {

}