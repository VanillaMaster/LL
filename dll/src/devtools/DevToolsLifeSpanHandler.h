#pragma once

#include "include/capi/cef_life_span_handler_capi.h"
#include "DevToolsClient.h"

struct DevToolsLifeSpanHandler {
    cef_life_span_handler_t handler {};

    int count = 0;

    DevToolsClient* parent;

    DevToolsLifeSpanHandler(DevToolsClient* parent);
    ~DevToolsLifeSpanHandler();
};