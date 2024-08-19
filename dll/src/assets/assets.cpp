#include "assets.h"
#include "LocalResourceHandler.h"

#include "../location.h"

#include "../utils/cef.h"

#include <ios>
#include <fstream>

void(CEF_CALLBACK add_ref)(struct AssetsSchemeHandlerFactory* self) {
    self->count += 1;
}

int(CEF_CALLBACK release)(struct AssetsSchemeHandlerFactory* self) {
    if (--(self->count) != 0) return false;
    std::wofstream log("D:/log/alloc.log", std::ios_base::app | std::ios_base::out);
    log << L"[freed]: " << L"AssetsSchemeHandlerFactory(" << std::to_wstring((uintptr_t)self) << L")\n";
    log.close();
    delete self;
    return true;
}

int(CEF_CALLBACK has_one_ref)(struct AssetsSchemeHandlerFactory* self) {
    return self->count == 1;
}

int(CEF_CALLBACK has_at_least_one_ref)(struct AssetsSchemeHandlerFactory* self) {
    return self->count > 0;
}

struct _cef_resource_handler_t* (CEF_CALLBACK create)(
    struct AssetsSchemeHandlerFactory* self,
    struct _cef_browser_t* browser,
    struct _cef_frame_t* frame,
    const cef_string_t* scheme_name,
    struct _cef_request_t* request
) {
    auto _method = request->get_method(request);
    std::wstring method(_method->str, _method->length);
    auto const is_not_get = (method != L"GET");
    cef_string_userfree_free(_method);
        
    if (is_not_get) return NULL;

    auto url = request->get_url(request);
    cef_urlparts_t parts{};
    cef_parse_url(url, &parts);
    
    std::wstring _url(url->str, url->length);

    cef_string_userfree_free(url);

    cef_string_t location{};
    cef_string_concat(&self->root, &parts.path, &location);
    cef_urlparts_clear(&parts);

    std::wofstream log("D:/log/fs.log", std::ios_base::app | std::ios_base::out);
    std::wstring loc(location.str, location.length);
    log << L"[create]" << loc << L":" << _url << L"\n";
    log.close();
    
    auto handler = new LocalResourceHandler(location);
    handler->handler.base.add_ref(&handler->handler.base);

    cef_string_clear(&location);

    return (_cef_resource_handler_t*)handler;
}

AssetsSchemeHandlerFactory::AssetsSchemeHandlerFactory(const cef_string_t& root) {
    cef_string_copy(root.str, root.length, &this->root);

    this->factory.base.size = sizeof(AssetsSchemeHandlerFactory);
    this->factory.base.add_ref = (decltype(cef_base_ref_counted_t::add_ref))&add_ref;
    this->factory.base.release = (decltype(cef_base_ref_counted_t::release))&release;
    this->factory.base.has_one_ref = (decltype(cef_base_ref_counted_t::has_one_ref))&has_one_ref;
    this->factory.base.has_at_least_one_ref = (decltype(cef_base_ref_counted_t::has_at_least_one_ref))&has_at_least_one_ref;

    this->factory.create = (decltype(cef_scheme_handler_factory_t::create))&create;

    std::wofstream log("D:/log/alloc.log", std::ios_base::app | std::ios_base::out);
    log << L"[allocated]: " << L"AssetsSchemeHandlerFactory(" << std::to_wstring((uintptr_t)this) << L")\n";
    log.close();
}


void RegisterAssetsSchemeHandlerFactory() {
    cef_string_t scheme_name{};
    cef_string_from_ascii("https", 5, &scheme_name);

    cef_string_t domain_name{};
    cef_string_from_ascii("fs.local", 8, &domain_name);

    cef_string_t root{};
    auto const location = getLocation();
    cef_string_from_wide(location.data(), location.length(), &root);

    auto factory = new AssetsSchemeHandlerFactory(root);
    factory->factory.base.add_ref(&factory->factory.base);

    auto code = cef_register_scheme_handler_factory(&scheme_name, &domain_name, (cef_scheme_handler_factory_t*)factory);

    std::wofstream log("D:/log/fs.log", std::ios_base::app | std::ios_base::out);
    log << L"[register] " << std::to_wstring(code) << L"\n";
    log.close();

    //factory->factory.base.release(&factory->factory.base);

    cef_string_clear(&scheme_name);
    cef_string_clear(&domain_name);
    cef_string_clear(&root);
}