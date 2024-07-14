#include <iostream>
#include <unordered_map>

#include "renderer.h"
#include "../proxy.h"
#include "NativeV8Handler.h"

#include "include/capi/cef_parser_capi.h"
#include "include/capi/cef_app_capi.h"
#include "include/capi/cef_render_process_handler_capi.h"

#include <ios>
#include <fstream>

struct RenderProcessHandlersItem {
    struct _cef_render_process_handler_t* (CEF_CALLBACK* getHandler)(struct _cef_app_t* self);

    int(CEF_CALLBACK* release)(struct _cef_base_ref_counted_t* self);
};

struct ContextCreatedCallBacksItem {
    decltype(_cef_render_process_handler_t::on_context_created) callBack;
    decltype(_cef_base_ref_counted_t::release) release;
};

struct ContextReleasedCallBacksItem {
    decltype(_cef_render_process_handler_t::on_context_released) callBack;
    decltype(_cef_base_ref_counted_t::release) release;
};

std::unordered_map<void*, RenderProcessHandlersItem> renderProcessHandlers;

std::unordered_map<void*, ContextCreatedCallBacksItem> contextCreatedCallBacks;
std::unordered_map<void*, ContextReleasedCallBacksItem> contextReleasedCallBacks;

//std::unordered_map<void*, std::vector<_cef>>;

static Proxy<decltype(cef_execute_process)> CefExecuteProcess{};

int(CEF_CALLBACK releaseRenderProcessHandler)(struct _cef_base_ref_counted_t* self) {
    auto [_, release] = renderProcessHandlers[self];
    auto const result = release(self);
    if (result) renderProcessHandlers.erase(self);
    return result;
}

int(CEF_CALLBACK releaseContextCreatedCallBack)(struct _cef_base_ref_counted_t* self) {
    auto [_, release] = contextCreatedCallBacks[self];
    auto const result = release(self);
    if (result) contextCreatedCallBacks.erase(self);
    return result;
}

void(CEF_CALLBACK on_context_created)(
    struct _cef_render_process_handler_t* self,
    struct _cef_browser_t* browser,
    struct _cef_frame_t* frame,
    struct _cef_v8context_t* context
) {
    auto url = frame->get_url(frame);
    cef_urlparts_t parts{};
    cef_parse_url(url, &parts);
    cef_string_userfree_free(url);

    std::wstring host(parts.host.str, parts.host.length);
    std::wstring path(parts.path.str, parts.path.length);

    std::wofstream log("D:/log/renderer.log", std::ios_base::app | std::ios_base::out);

    log << host << L":" << path << L"\n";

    auto [callBack, _] = contextCreatedCallBacks[self];
    callBack(self, browser, frame, context);

    if (!host.compare(L"127.0.0.1") && !path.compare(L"/index.html")) {
        log << L"LeagueClientUxRender.exe" << L"\n";

        auto window = context->get_global(context);
        window->base.add_ref(&window->base);

        auto obj = cef_v8value_create_object(NULL, NULL);
        obj->base.add_ref(&obj->base);

        cef_string_utf16_t native_text{};
        cef_string_from_utf8("__native", 8, &native_text);
        cef_string_t native_key(native_text);
        window->set_value_bykey(window, &native_key, obj, V8_PROPERTY_ATTRIBUTE_READONLY);

        {
            cef_string_utf16_t devtools_text {};
            cef_string_from_utf8("openDevTools", 12, &devtools_text);
            cef_string_t devtools_name(devtools_text);

            auto handler = new NativeV8Handler();
            handler->handler.base.add_ref(&handler->handler.base);

            auto fn = cef_v8value_create_function(&devtools_name, (cef_v8handler_t*)handler);
            fn->base.add_ref(&fn->base);
            
            obj->set_value_bykey(obj, &devtools_name, fn, V8_PROPERTY_ATTRIBUTE_READONLY);

            handler->handler.base.release(&handler->handler.base);
            fn->base.release(&fn->base);
        }

        obj->base.release(&obj->base);
        window->base.release(&window->base);

        cef_string_utf16_t text {};
        cef_string_from_utf8("window.addEventListener('keydown', function(e) {if (e.code == 'F12') __native.openDevTools();});", 96, &text);
        cef_string_t script(text);

        frame->execute_java_script(frame, &script, nullptr, 1);
    }

    log.close();
}

struct _cef_render_process_handler_t* (CEF_CALLBACK get_render_process_handler)(struct _cef_app_t* self) {
    auto [getHandler, _] = renderProcessHandlers[self];
    auto const handler = getHandler(self);

    contextCreatedCallBacks[handler] = { handler->on_context_created, handler->base.release };

    handler->base.release = releaseContextCreatedCallBack;
    handler->on_context_created = on_context_created;

    return handler;
}

int cef_execute_process(const cef_main_args_t* args, cef_app_t* app, void* windows_sandbox_info) {
    std::wofstream log("D:/log/renderer.log", std::ios_base::app | std::ios_base::out);
    log << L"cef_execute_process\n";

    //renderProcessHandlers[app] = { app->get_render_process_handler, app->base.release };

    //app->base.release = &releaseRenderProcessHandler;
    //app->get_render_process_handler = &get_render_process_handler;

    log.close();

    return CefExecuteProcess.call(args, app, windows_sandbox_info);
}

bool hookOnRenderer() {

    HMODULE libcef = GetModuleHandleA("libcef.dll");
    if (libcef == NULL) return true;
    FARPROC __cef_execute_process = GetProcAddress(libcef, "cef_execute_process");
    if (__cef_execute_process == NULL) return true;
    return CefExecuteProcess.attach(__cef_execute_process, &cef_execute_process);
}