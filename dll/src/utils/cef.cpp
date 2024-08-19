#include "cef.h"

#include <type_traits>

void string_dtor(decltype(cef_string_t::str) str) {
    delete[] str;
}

void cef_string_concat(const cef_string_t* head, const cef_string_t* tail, cef_string_t* out) {
    const auto length = head->length + tail->length;
    auto buff = new std::remove_pointer<decltype(cef_string_t::str)>::type[length + 1];

    std::copy(&head->str[0], &head->str[head->length], &buff[0]);
    std::copy(&tail->str[0], &tail->str[tail->length], &buff[head->length]);
    buff[length] = 0;

    cef_string_set(buff, length, out, false);
    out->dtor = &string_dtor;
}

void cef_urlparts_clear(cef_urlparts_t* parts) {
    cef_string_clear(&parts->spec);
    cef_string_clear(&parts->scheme);
    cef_string_clear(&parts->username);
    cef_string_clear(&parts->password);
    cef_string_clear(&parts->host);
    cef_string_clear(&parts->port);
    cef_string_clear(&parts->origin);
    cef_string_clear(&parts->path);
    cef_string_clear(&parts->query);
    cef_string_clear(&parts->fragment);
}

void cef_urlparts_free(cef_urlparts_t* parts) {
    cef_urlparts_clear(parts);
    delete parts;
}