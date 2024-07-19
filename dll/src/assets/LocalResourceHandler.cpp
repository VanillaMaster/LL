#include "LocalResourceHandler.h"

#include "include/capi/cef_resource_handler_capi.h"
#include "include/capi/cef_scheme_capi.h"

#include "include/capi/cef_parser_capi.h"

#include <ios>
#include <fstream>

void(CEF_CALLBACK add_ref)(struct LocalResourceHandler* self) {
    self->count += 1;
}

int(CEF_CALLBACK release)(struct LocalResourceHandler* self) {
    if (--(self->count) != 0) return false;
    std::wofstream log("D:/log/alloc.log", std::ios_base::app | std::ios_base::out);
    log << L"[freed]: " << L"LocalResourceHandler(" << std::to_wstring((uintptr_t)self) << L")\n";
    log.close();
    cef_string_clear(&self->file);
    if (self->reader != NULL) self->reader->base.release(&self->reader->base);
    delete self;
    return true;
}

int(CEF_CALLBACK has_one_ref)(struct LocalResourceHandler* self) {
    return self->count == 1;
}

int(CEF_CALLBACK has_at_least_one_ref)(struct LocalResourceHandler* self) {
    return self->count > 0;
}

void(CEF_CALLBACK get_response_headers)(
    struct LocalResourceHandler* self,
    struct _cef_response_t* response,
    int64* response_length,
    cef_string_t* redirectUrl
) {
    cef_string_t cors_key{};
    cef_string_t cors_value{};

    cef_string_from_ascii("Access-Control-Allow-Origin", 27, &cors_key);
    cef_string_from_ascii("*", 1, &cors_value);

    response->set_header_by_name(response, &cors_key, &cors_value, 0);

    cef_string_clear(&cors_key);
    cef_string_clear(&cors_value);

    if (self->reader == NULL) {
        *response_length = 0;
        response->set_status(response, 404);
        response->set_error(response, ERR_NONE);
        return;
    }

    *response_length = -1;

    response->set_status(response, 200);
    response->set_error(response, ERR_NONE);

    cef_string_t ext{};
    cef_string_from_ascii(".bin", 4, &ext);

    for (size_t i = self->file.length; i-- > 0;) {
        const auto c = self->file.str[i];
        if (c == L'.') {
            auto const j = i + 1;
            if (j < self->file.length) cef_string_copy(self->file.str + j, self->file.length - j, &ext);
            break;
        }
    }

    if (auto mime = cef_get_mime_type(&ext); mime != NULL) {
        response->set_mime_type(response, mime);
        cef_string_userfree_free(mime);
    } else {
        cef_string_t default_mime{};
        cef_string_from_ascii("text/plain", 10, &default_mime);
        response->set_mime_type(response, &default_mime);
        cef_string_clear(&default_mime);
    }

    cef_string_clear(&ext);
    
}

int(CEF_CALLBACK open)(
    struct LocalResourceHandler* self,
    struct _cef_request_t* request,
    int* handle_request,
    struct _cef_callback_t* callback
) {
    *handle_request = 1;

    self->reader = cef_stream_reader_create_for_file(&self->file);
    if (self->reader != NULL) self->reader->base.add_ref(&self->reader->base);

    return true;
}

int(CEF_CALLBACK skip)(
    struct LocalResourceHandler* self,
    int64 bytes_to_skip,
    int64* bytes_skipped,
    struct _cef_resource_skip_callback_t* callback
) {

    auto const code = self->reader->seek(self->reader, bytes_to_skip, SEEK_CUR);

    if (code != 0) {
        *bytes_skipped = -2;
        return false;
    }

    *bytes_skipped = bytes_to_skip;
    return true;
}

int(CEF_CALLBACK read)(
    struct LocalResourceHandler* self,
    void* data_out,
    int bytes_to_read,
    int* bytes_read,
    struct _cef_resource_read_callback_t* callback
) {

    *bytes_read = 0;
    if (self->reader == NULL) return false;

    int read = 0;

    do {
        read = static_cast<int>(self->reader->read(self->reader, (char*)data_out + *bytes_read, 1, bytes_to_read - *bytes_read));
        *bytes_read += read;
    } while (read != 0 && *bytes_read < bytes_to_read);

    return (*bytes_read > 0);
}

void(CEF_CALLBACK cancel)(struct LocalResourceHandler self) {

}

LocalResourceHandler::LocalResourceHandler(std::wstring src) {

    cef_string_from_wide(src.data(), src.length(), &this->file);
    
    this->handler.base.size = sizeof(LocalResourceHandler);
    this->handler.base.add_ref = (decltype(cef_base_ref_counted_t::add_ref))&add_ref;
    this->handler.base.release = (decltype(cef_base_ref_counted_t::release))&release;
    this->handler.base.has_one_ref = (decltype(cef_base_ref_counted_t::has_one_ref))&has_one_ref;
    this->handler.base.has_at_least_one_ref = (decltype(cef_base_ref_counted_t::has_at_least_one_ref))&has_at_least_one_ref;
    
    this->handler.get_response_headers = (decltype(cef_resource_handler_t::get_response_headers))&get_response_headers;
    this->handler.open = (decltype(cef_resource_handler_t::open))&open;
    this->handler.skip = (decltype(cef_resource_handler_t::skip))&skip;
    this->handler.read = (decltype(cef_resource_handler_t::read))&read;
    this->handler.cancel = (decltype(cef_resource_handler_t::cancel))&cancel;

    std::wofstream log("D:/log/alloc.log", std::ios_base::app | std::ios_base::out);
    log << L"[allocated]: " << L"LocalResourceHandler(" << std::to_wstring((uintptr_t)this) << L")\n";
    log.close();
}