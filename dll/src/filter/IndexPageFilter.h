#include "include/capi/cef_response_filter_capi.h"

struct IndexPageFilter {
    cef_response_filter_t filter {};

    int count = 0;

    IndexPageFilter();
};