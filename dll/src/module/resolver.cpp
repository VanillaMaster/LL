#include "resolver.h"

#include "ModuleResolverSchemeHandlerFactory.h"

void RegisterModuleResolverSchemeHandlerFactory() {

    CefRegisterSchemeHandlerFactory("https", "module.local", new ModuleResolverSchemeHandlerFactory());
    /*
    cef_string_t scheme_name{};
    cef_string_from_ascii("https", 5, &scheme_name);

    cef_string_t domain_name{};
    cef_string_from_ascii("module.local", 12, &domain_name);

    auto factory = new ModuleResolverSchemeHandlerFactory();
    factory->factory.base.add_ref(&factory->factory.base);

    auto code = cef_register_scheme_handler_factory(&scheme_name, &domain_name, (cef_scheme_handler_factory_t*)factory);

    //factory->factory.base.release(&factory->factory.base);

    cef_string_clear(&scheme_name);
    cef_string_clear(&domain_name);
    */
}