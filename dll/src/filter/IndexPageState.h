#pragma once

enum SearchState {
    SEARCH_STATE_NONE,
    SEARCH_STATE_LESS_THAN_SIGN,
    SEARCH_STATE_S,
    SEARCH_STATE_C,
    SEARCH_STATE_R,
    SEARCH_STATE_I,
    SEARCH_STATE_P,
    SEARCH_STATE_T,
    SEARCH_STATE_DONE,
    SEARCH_STATE_ERROR
};

SearchState consume(char c, SearchState state);