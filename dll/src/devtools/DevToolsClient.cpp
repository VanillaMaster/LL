#include "DevToolsClient.h"
#include "DevToolsLifeSpanHandler.h"

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
    return new DevToolsLifeSpanHandler(self->parent);
}

DevToolsClient::DevToolsClient(int parent) {
	this->parent = parent;

    this->client.base.size = sizeof(DevToolsClient);
    this->client.base.add_ref = (decltype(cef_base_ref_counted_t::add_ref))&add_ref;
    this->client.base.release = (decltype(cef_base_ref_counted_t::release))&release;
    this->client.base.has_one_ref = (decltype(cef_base_ref_counted_t::has_one_ref))&has_one_ref;
    this->client.base.has_at_least_one_ref = (decltype(cef_base_ref_counted_t::has_at_least_one_ref))&has_at_least_one_ref;

    //this->client.get_keyboard_handler = (decltype(_cef_client_t::get_keyboard_handler))&get_keyboard_handler;
    //this->client.get_life_span_handler = (decltype(_cef_client_t::get_life_span_handler))&get_life_span_handler;
};