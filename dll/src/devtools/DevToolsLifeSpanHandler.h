#include "include/capi/cef_life_span_handler_capi.h"

struct DevToolsLifeSpanHandler {
    cef_life_span_handler_t handler {};

    int count = 0;

    DevToolsLifeSpanHandler(int parent);
};