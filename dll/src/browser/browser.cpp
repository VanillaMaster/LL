#include <iostream>
#include <windows.h>
#include <unordered_map>

#include "browser.h"
#include "../proxy.h"

#include "../devtools/devtools.h"

#include "../filter/filter.h"

#include "include/capi/cef_app_capi.h"
#include "include/capi/cef_client_capi.h"
#include "include/capi/cef_browser_capi.h"
#include "include/capi/cef_parser_capi.h"

#include <ios>
#include <fstream>

struct BrowserProcessHandlersItem {
	struct _cef_browser_process_handler_t* (CEF_CALLBACK* getHandler)(struct _cef_app_t* self);

	decltype(_cef_base_ref_counted_t::release) release;
};

struct ContextInitializedCallBacksItem {
	void(CEF_CALLBACK* callBack)(struct _cef_browser_process_handler_t* self);

	decltype(_cef_base_ref_counted_t::release) release;
};

struct ProcessMessageReceivedCallBacksItem {
	decltype(_cef_client_t::on_process_message_received) callBack;
	decltype(_cef_base_ref_counted_t::release) release;
};

std::unordered_map<void*, BrowserProcessHandlersItem> browserProcessHandlers;
std::unordered_map<void*, ContextInitializedCallBacksItem> contextInitializedCallBacks;
std::unordered_map<void*, ProcessMessageReceivedCallBacksItem> processMessageReceivedCalls;

static Proxy<decltype(cef_initialize)> cefInitialize {};

static Proxy<decltype(cef_browser_host_create_browser)> cefBrowserHostCreateBrowser {};

int(CEF_CALLBACK releaseBrowserProcessHandler)(struct _cef_base_ref_counted_t* self) {
	auto [_, release] = browserProcessHandlers[self];
	auto const result = release(self);
	if (result) browserProcessHandlers.erase(self);
	return result;
}

int(CEF_CALLBACK releaseContextInitializedCallBack)(struct _cef_base_ref_counted_t* self) {
	auto [_, release] = contextInitializedCallBacks[self];
	auto const result = release(self);
	if (result) contextInitializedCallBacks.erase(self);
	return result;
}

int(CEF_CALLBACK releaseProcessMessageReceivedCallBack)(struct _cef_base_ref_counted_t* self) {
	auto [_, release] = processMessageReceivedCalls[self];
	auto const result = release(self);
	if (result) processMessageReceivedCalls.erase(self);
	return result;
}

void(CEF_CALLBACK on_context_initialized)(struct _cef_browser_process_handler_t* self) {
	std::wofstream log("D:/log/browser.log", std::ios_base::app | std::ios_base::out);
	log << L"on_context_initialized" << L"\n";

	auto [callBack, _] = contextInitializedCallBacks[self];
	callBack(self);

	log.close();
}

struct _cef_browser_process_handler_t* (CEF_CALLBACK get_browser_process_handler)(struct _cef_app_t* self) {
	auto const [getHandler, _] = browserProcessHandlers[self];
	auto const handler = getHandler(self);

	contextInitializedCallBacks[handler] = { handler->on_context_initialized, handler->base.release };

	handler->base.release = releaseContextInitializedCallBack;
	handler->on_context_initialized = on_context_initialized;

	return handler;
}

int cef_initialize(
	const struct _cef_main_args_t* args,
	const struct _cef_settings_t* settings,
	cef_app_t* application,
	void* windows_sandbox_info
) {
	browserProcessHandlers[application] = { application->get_browser_process_handler, application->base.release };
	
	application->base.release = releaseBrowserProcessHandler;
	application->get_browser_process_handler = get_browser_process_handler;

	return cefInitialize.call(args, settings, application, windows_sandbox_info);
}

int(CEF_CALLBACK on_process_message_received)(
	struct _cef_client_t* self,
	struct _cef_browser_t* browser,
	struct _cef_frame_t* frame,
	cef_process_id_t source_process,
	struct _cef_process_message_t* message
) {
	auto const [callBack, _] = processMessageReceivedCalls[self];

	std::wofstream log("D:/log/browser.log", std::ios_base::app | std::ios_base::out);
	if (source_process == PID_RENDERER) {
		auto name = message->get_name(message);
		std::wstring n(name->str, name->length);

		log << L"[msg] " << n << L"\n";

		if (n == L"__devtools__") {
			openDevTools(browser);
		}
		cef_string_userfree_free(name);
	}
	log.close();

	return callBack(self, browser, frame, source_process, message);
}

int cef_browser_host_create_browser(
	const cef_window_info_t* windowInfo,
	struct _cef_client_t* client,
	const cef_string_t* url,
	const struct _cef_browser_settings_t* settings,
	struct _cef_dictionary_value_t* extra_info,
	struct _cef_request_context_t* request_context
) {

	cef_urlparts_t parts{};
	cef_parse_url(url, &parts);

	std::wstring host(parts.host.str, parts.host.length);
	std::wstring path(parts.path.str, parts.path.length);

	std::wofstream log("D:/log/browser.log", std::ios_base::app | std::ios_base::out);
	
	log << host << ":" << path << "\n";

	if (!host.compare(L"127.0.0.1") && !path.compare(L"/bootstrap.html")) {
		
		hookRequestHandler(client);

		processMessageReceivedCalls[client] = { client->on_process_message_received, client->base.release };

		client->base.release = releaseProcessMessageReceivedCallBack;
		client->on_process_message_received = on_process_message_received;
	}

	log.close();

	return cefBrowserHostCreateBrowser.call(windowInfo, client, url, settings, extra_info, request_context);
}

bool hookOnBrowser() {
	HMODULE libcef = GetModuleHandleA("libcef.dll");
	if (libcef == NULL) return true;
	FARPROC __cef_initialize = GetProcAddress(libcef, "cef_initialize");
	if (__cef_initialize == NULL) return true;
	FARPROC __cef_browser_host_create_browser = GetProcAddress(libcef, "cef_browser_host_create_browser");
	if (__cef_browser_host_create_browser == NULL) return true;

	if (cefInitialize.attach(__cef_initialize, &cef_initialize)) return true;
	if (cefBrowserHostCreateBrowser.attach(__cef_browser_host_create_browser, &cef_browser_host_create_browser)) {
		cefInitialize.detach();
		return true;
	}

	return false;
}