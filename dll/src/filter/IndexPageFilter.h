#include "include/capi/cef_response_filter_capi.h"

#include "IndexPageState.h"

enum FilterState {
    FILTER_STATE_SEARCH,
    FILTER_STATE_COPY,
    FILTER_STATE_FLUSH
};

struct IndexPageFilter {
    cef_response_filter_t filter {};

    int count = 0;

    int offset = 0;

    SearchState search_state = SEARCH_STATE_NONE;
    FilterState filter_state = FILTER_STATE_SEARCH;

    IndexPageFilter();
};