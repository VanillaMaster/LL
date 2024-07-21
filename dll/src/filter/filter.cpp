#include "filter.h"

#include "IndexPageFilter.h"

#include "../utils/cef.h"

#include "include/capi/cef_app_capi.h"
#include "include/capi/cef_client_capi.h"
#include "include/capi/cef_browser_capi.h"
#include "include/capi/cef_parser_capi.h"
#include "include/capi/cef_response_filter_capi.h"

#include <unordered_map>

struct RequestHandlersItem {
    decltype(cef_client_t::get_request_handler) getHandler;

    decltype(_cef_base_ref_counted_t::release) release;
};

struct ResourceRequestHandlersItem {
    decltype(_cef_request_handler_t::get_resource_request_handler) getHandler;

    decltype(_cef_base_ref_counted_t::release) release;
};

struct ResourceResponseFiltersItem {
    decltype(_cef_resource_request_handler_t::get_resource_response_filter) getHandler;

    decltype(_cef_base_ref_counted_t::release) release;
};

/*
struct FiltersItem {
    cef_response_filter_status_t(CEF_CALLBACK* filter)(
        struct _cef_response_filter_t* self,
        void* data_in,
        size_t data_in_size,
        size_t* data_in_read,
        void* data_out,
        size_t data_out_size,
        size_t* data_out_written);

    decltype(_cef_base_ref_counted_t::release) release;
};

static std::unordered_map<void*, FiltersItem> Filters;
*/

static std::unordered_map<void*, RequestHandlersItem> requestHandlers;
static std::unordered_map<void*, ResourceRequestHandlersItem> resourceRequestHandlers;
static std::unordered_map<void*, ResourceResponseFiltersItem> resourceResponseFilters;

int(CEF_CALLBACK releaseRequestHandler)(struct _cef_base_ref_counted_t* self) {
    auto [_, release] = requestHandlers[self];
    auto const result = release(self);
    if (result) requestHandlers.erase(self);
    return result;
};

int(CEF_CALLBACK releaseResourceRequestHandler)(struct _cef_base_ref_counted_t* self) {
    auto [_, release] = resourceRequestHandlers[self];
    auto const result = release(self);
    if (result) resourceRequestHandlers.erase(self);
    return result;
};

int(CEF_CALLBACK releaseResourceResponseFilter)(struct _cef_base_ref_counted_t* self) {
    auto [_, release] = resourceResponseFilters[self];
    auto const result = release(self);
    if (result) resourceResponseFilters.erase(self);
    return result;
};

struct _cef_response_filter_t* (CEF_CALLBACK get_resource_response_filter)(
    struct _cef_resource_request_handler_t* self,
    struct _cef_browser_t* browser,
    struct _cef_frame_t* frame,
    struct _cef_request_t* request,
    struct _cef_response_t* response
) {

    auto _url = request->get_url(request);
    cef_urlparts_t parts{};
    cef_parse_url(_url, &parts);
    cef_string_userfree_free(_url);


    std::wstring host(parts.host.str, parts.host.length);
    std::wstring path(parts.path.str, parts.path.length);

    cef_urlparts_clear(&parts);

    auto [getHandler, _] = resourceResponseFilters[self];
    auto handler = getHandler(self, browser, frame, request, response);

    if (!host.compare(L"127.0.0.1") && !path.compare(L"/index.html")) {
        //index page
        if (handler == nullptr) {
            auto filter = new IndexPageFilter();
            filter->filter.base.add_ref(&filter->filter.base);

            return (cef_response_filter_t*)filter;
        } else {
            //TODO: add proxy filter
        }
    }

    return handler;
};

struct _cef_resource_request_handler_t* (CEF_CALLBACK get_resource_request_handler)(
    struct _cef_request_handler_t* self,
    struct _cef_browser_t* browser,
    struct _cef_frame_t* frame,
    struct _cef_request_t* request,
    int is_navigation,
    int is_download,
    const cef_string_t* request_initiator,
    int* disable_default_handling
) {
    auto [getHandler, _] = resourceRequestHandlers[self];
    auto handler = getHandler(self, browser, frame, request, is_download, is_download, request_initiator, disable_default_handling);

    resourceResponseFilters[handler] = { handler->get_resource_response_filter, handler->base.release };
    handler->base.release = releaseResourceResponseFilter;
    handler->get_resource_response_filter = get_resource_response_filter;

    return handler;
};

struct _cef_request_handler_t* (CEF_CALLBACK get_request_handler)(struct _cef_client_t* self) {
    auto [getHandler, _] = requestHandlers[self];
    auto handler = getHandler(self);

    resourceRequestHandlers[handler] = { handler->get_resource_request_handler, handler->base.release };
    handler->base.release = releaseResourceRequestHandler;
    handler->get_resource_request_handler = get_resource_request_handler;

    return handler;
};

void hookRequestHandler(cef_client_t* client) {
    requestHandlers[client] = { client->get_request_handler, client->base.release };
    client->base.release = releaseRequestHandler;
    client->get_request_handler = get_request_handler;
}