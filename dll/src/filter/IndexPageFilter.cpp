#include "IndexPageFilter.h"

void(CEF_CALLBACK add_ref)(struct IndexPageFilter* self) {
    self->count += 1;
};

int(CEF_CALLBACK release)(struct IndexPageFilter* self) {
    if (--(self->count) != 0) return false;
    delete self;
    return true;
};

int(CEF_CALLBACK has_one_ref)(struct IndexPageFilter* self) {
    return self->count == 1;
}

int(CEF_CALLBACK has_at_least_one_ref)(struct IndexPageFilter* self) {
    return self->count > 0;
};

int(CEF_CALLBACK init_filter)(struct IndexPageFilter* self) {
    return true;
};

static const char* fragment = "src=\"https://fs.local/preload.js\"></script><script ";
static const int fragment_length = 51;

cef_response_filter_status_t(CEF_CALLBACK filter__)(
    struct IndexPageFilter* self,
    void* data_in,
    size_t data_in_size,
    size_t* data_in_read,
    void* data_out,
    size_t data_out_size,
    size_t* data_out_written
) {
    while (*data_in_read < data_in_size && *data_out_written < data_out_size) {
        switch (self->filter_state) {
            case FILTER_STATE_SEARCH:
                self->search_state = consume(((char*)data_in)[*data_in_read], self->search_state);
                ((char*)data_out)[(*data_out_written)++] = ((char*)data_in)[(*data_in_read)++];

                if (self->search_state == SEARCH_STATE_DONE) self->filter_state = FILTER_STATE_COPY;
                if (self->search_state == SEARCH_STATE_ERROR) self->filter_state = FILTER_STATE_FLUSH;
                break;
            case FILTER_STATE_COPY:
                ((char*)data_out)[(*data_out_written)++] = fragment[(self->offset)++];
                if (self->offset >= fragment_length) self->filter_state = FILTER_STATE_FLUSH;
                break;
            case FILTER_STATE_FLUSH:
                ((char*)data_out)[(*data_out_written)++] = ((char*)data_in)[(*data_in_read)++];
                break;
        }

        
    }
    if (self->filter_state == FILTER_STATE_COPY) return RESPONSE_FILTER_NEED_MORE_DATA;
    if (*data_in_read != data_in_size) return RESPONSE_FILTER_NEED_MORE_DATA;
    return RESPONSE_FILTER_DONE;
};

IndexPageFilter::IndexPageFilter() {

    this->filter.base.size = sizeof(IndexPageFilter);
    this->filter.base.add_ref = (decltype(cef_base_ref_counted_t::add_ref))&add_ref;
    this->filter.base.release = (decltype(cef_base_ref_counted_t::release))&release;
    this->filter.base.has_one_ref = (decltype(cef_base_ref_counted_t::has_one_ref))&has_one_ref;
    this->filter.base.has_at_least_one_ref = (decltype(cef_base_ref_counted_t::has_at_least_one_ref))&has_at_least_one_ref;

    this->filter.init_filter = decltype(cef_response_filter_t::init_filter)(&init_filter);
    this->filter.filter = decltype(cef_response_filter_t::filter)(&filter__);
}