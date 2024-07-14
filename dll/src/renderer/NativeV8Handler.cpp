#include "NativeV8Handler.h"

#include <iostream>

void(CEF_CALLBACK add_ref)(struct NativeV8Handler* self) {
    self->count += 1;
};

int(CEF_CALLBACK release)(struct NativeV8Handler* self) {
    if (--(self->count) != 0) return false;
    delete self;
    return true;
};

int(CEF_CALLBACK has_one_ref)(struct NativeV8Handler* self) {
    return self->count == 1;
}

int(CEF_CALLBACK has_at_least_one_ref)(struct NativeV8Handler* self) {
    return self->count > 0;
};

int(CEF_CALLBACK execute)(
    struct NativeV8Handler* self,
    const cef_string_t* name,
    struct _cef_v8value_t* object,
    size_t argumentsCount,
    struct _cef_v8value_t* const* arguments,
    struct _cef_v8value_t** retval,
    cef_string_t* exception
) {
    
    auto context = cef_v8context_get_current_context();
    auto frame = context->get_frame(context);

    cef_string_utf16_t msg_text {};
    cef_string_from_utf8("__devtools__", 12, &msg_text);
    cef_string_t msg_name(msg_text);
    auto msg = cef_process_message_create(&msg_name);
    frame->send_process_message(frame, PID_BROWSER, msg);
    

    std::wcout << L"openDevTools" << L"\n";
    return true;
}

NativeV8Handler::NativeV8Handler() {

    this->handler.base.size = sizeof(NativeV8Handler);
    this->handler.base.add_ref = (decltype(cef_base_ref_counted_t::add_ref))&add_ref;
    this->handler.base.release = (decltype(cef_base_ref_counted_t::release))&release;
    this->handler.base.has_one_ref = (decltype(cef_base_ref_counted_t::has_one_ref))&has_one_ref;
    this->handler.base.has_at_least_one_ref = (decltype(cef_base_ref_counted_t::has_at_least_one_ref))&has_at_least_one_ref;

    this->handler.execute = (decltype(cef_v8handler_t::execute))&execute;
}