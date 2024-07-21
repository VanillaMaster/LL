#include "IndexPageState.h"

#include <functional>

SearchState consume_none(char c) {
    switch (c) {
        case '<':
            return SEARCH_STATE_LESS_THAN_SIGN;
        default:
            return SEARCH_STATE_NONE;
    }
}

SearchState consume_less_then_sign(char c) {
    switch (c) {
        case 's':
            return SEARCH_STATE_S;
        default:
            return consume_none(c);
    }
}

SearchState consume_s(char c) {
    switch (c) {
        case 'c':
            return SEARCH_STATE_C;
        default:
            return consume_none(c);
    }
}

SearchState consume_c(char c) {
    switch (c) {
        case 'r':
            return SEARCH_STATE_R;
        default:
            return consume_none(c);
    }
}

SearchState consume_r(char c) {
    switch (c) {
        case 'i':
            return SEARCH_STATE_I;
        default:
            return consume_none(c);
    }
}

SearchState consume_i(char c) {
    switch (c) {
        case 'p':
            return SEARCH_STATE_P;
        default:
            return consume_none(c);
    }
}

SearchState consume_p(char c) {
    switch (c) {
        case 't':
            return SEARCH_STATE_T;
        default:
            return consume_none(c);
    }
}

SearchState consume_t(char c) {
    switch (c) {
        case ' ':
            return SEARCH_STATE_DONE;
        default:
            return consume_none(c);
    }
}

SearchState consume(char c, SearchState state) {
    switch (state) {
        case SEARCH_STATE_NONE: return consume_none(c);
        case SEARCH_STATE_LESS_THAN_SIGN: return consume_less_then_sign(c);
        case SEARCH_STATE_S: return consume_s(c);
        case SEARCH_STATE_C: return consume_c(c);
        case SEARCH_STATE_R: return consume_r(c);
        case SEARCH_STATE_I: return consume_i(c);
        case SEARCH_STATE_P: return consume_p(c);
        case SEARCH_STATE_T: return consume_t(c);
        case SEARCH_STATE_DONE: return SEARCH_STATE_DONE;
        case SEARCH_STATE_ERROR: return SEARCH_STATE_ERROR;
        default: return SEARCH_STATE_ERROR;
    }
}