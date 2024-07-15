#include "DevToolsClient.h"
#include "DevToolsLifeSpanHandler.h"

#include <ios>
#include <fstream>

void(CEF_CALLBACK add_ref)(struct DevToolsClient* self) {
    self->count += 1;
}

int(CEF_CALLBACK release)(struct DevToolsClient* self) {
    if (--(self->count) != 0) return false;
    delete self;
    return true;
}

int(CEF_CALLBACK has_one_ref)(struct DevToolsClient* self) {
    return self->count == 1;
}

int(CEF_CALLBACK has_at_least_one_ref)(struct DevToolsClient* self) {
    return self->count > 0;
}

struct DevToolsLifeSpanHandler* (CEF_CALLBACK get_life_span_handler)(struct DevToolsClient* self) {
    return new DevToolsLifeSpanHandler(self);
}

DevToolsClient::DevToolsClient(int parent) {

    this->client.base.size = sizeof(DevToolsClient);
    this->client.base.add_ref = (decltype(cef_base_ref_counted_t::add_ref))&add_ref;
    this->client.base.release = (decltype(cef_base_ref_counted_t::release))&release;
    this->client.base.has_one_ref = (decltype(cef_base_ref_counted_t::has_one_ref))&has_one_ref;
    this->client.base.has_at_least_one_ref = (decltype(cef_base_ref_counted_t::has_at_least_one_ref))&has_at_least_one_ref;

    this->client.get_life_span_handler = (decltype(_cef_client_t::get_life_span_handler))&get_life_span_handler;

    std::wofstream log("D:/log/alloc.log", std::ios_base::app | std::ios_base::out);
    log << L"[allocated]: " << L"DevToolsClient(" << std::to_wstring((uintptr_t)this) << L")\n";
    log.close();
};

DevToolsClient::~DevToolsClient() {
    std::wofstream log("D:/log/alloc.log", std::ios_base::app | std::ios_base::out);
    log << L"[freed]: " << L"DevToolsClient(" << std::to_wstring((uintptr_t)this) << L")\n";
    log.close();
}