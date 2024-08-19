#pragma once

#include "include/capi/cef_base_capi.h"

void cef_string_concat(const cef_string_t* head, const cef_string_t* tail, cef_string_t* out);

void cef_urlparts_clear(cef_urlparts_t* parts);

void cef_urlparts_free(cef_urlparts_t* parts);

struct _cef_urlparts_free_functor {
    void operator()(cef_urlparts_t* parts) const {
        cef_urlparts_free(parts);
    }
};

struct _cef_string_userfree_free_functor {
    void operator()(cef_string_userfree_t self) const {
        cef_string_userfree_free(self);
    }
};