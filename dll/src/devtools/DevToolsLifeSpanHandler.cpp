#include "DevToolsLifeSpanHandler.h"

void(CEF_CALLBACK add_ref)(struct DevToolsLifeSpanHandler* self) {
    self->count += 1;
};

int(CEF_CALLBACK release)(struct DevToolsLifeSpanHandler* self) {
    if (--(self->count) != 0) return false;
    delete self;
    return true;
};

int(CEF_CALLBACK has_one_ref)(struct DevToolsLifeSpanHandler* self) {
    return self->count == 1;
}

int(CEF_CALLBACK has_at_least_one_ref)(struct DevToolsLifeSpanHandler* self) {
    return self->count > 0;
};

void(CEF_CALLBACK on_after_created)(struct DevToolsLifeSpanHandler* self, struct _cef_browser_t* browser) {

}

void(CEF_CALLBACK on_before_close)(struct DevToolsLifeSpanHandler* self, struct _cef_browser_t* browser) {

}

DevToolsLifeSpanHandler::DevToolsLifeSpanHandler(int parent) {

	this->handler.base.size = sizeof(DevToolsLifeSpanHandler);
    this->handler.base.add_ref = (decltype(cef_base_ref_counted_t::add_ref))&add_ref;
    this->handler.base.release = (decltype(cef_base_ref_counted_t::release))&release;
    this->handler.base.has_one_ref = (decltype(cef_base_ref_counted_t::has_one_ref))&has_one_ref;
    this->handler.base.has_at_least_one_ref = (decltype(cef_base_ref_counted_t::has_at_least_one_ref))&has_at_least_one_ref;

    this->handler.on_after_created = (decltype(cef_life_span_handler_t::on_after_created))&on_after_created;
    this->handler.on_before_close = (decltype(cef_life_span_handler_t::on_before_close))on_before_close;
    /*
    this->handler.do_close = NULL;
    this->handler.on_before_popup = NULL;
    */
}

