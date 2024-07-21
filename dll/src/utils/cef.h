#pragma once

#include "include/capi/cef_base_capi.h"

void cef_string_concat(const cef_string_t* head, const cef_string_t* tail, cef_string_t* out);

void cef_urlparts_clear(cef_urlparts_t* parts);