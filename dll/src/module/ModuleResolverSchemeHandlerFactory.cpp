#include "ModuleResolverSchemeHandlerFactory.h"

#include "include/cef_parser.h"
#include "include/capi/cef_parser_capi.h"

#include "../location.h"
#include "../utils/cef.h"

#include <numeric>

#include <filesystem>
#include <iostream>

#include <fstream>

#include <ios>


enum RESOLVE_STATUS_CODE {
    RESOLVE_OK,
    RESOLVE_INTERNAL_SERVER_ERROR,
    RESOLVE_INVALID_MODULE_SPECIFIER,
    RESOLVE_MODULE_NOT_FOUND,
    RESOLVE_INTERNAL_ERROR,
    RESOLVE_INVALID_PACKAGE_CONFIGURATION,
    RESOLVE_PACKAGE_PATH_NOT_EXPORTED,
    RESOLVE_INVALID_PACKAGE_TARGET
};

bool isValidURL(const CefString& url) {
    CefURLParts parts{};
    return CefParseURL(url, parts);
}


size_t find_first_of(const std::wstring& self, const std::vector<std::wstring>& searchValues, size_t offset, size_t& index) {
    size_t j = 0;
    size_t position = std::wstring::npos;
    for (size_t i = 0; i < searchValues.size(); i++) {
        auto& searchValue = searchValues.at(i);
        if (auto const pos = self.find(searchValue, offset); pos != std::wstring::npos) {
            if (position > pos) {
                position = pos;
                j = i;
            }
        }
    }
    if (position != std::wstring::npos) index = j;
    return position;
}

std::vector<std::wstring> Split(const std::wstring& self, const std::vector<std::wstring>& delimiters) {
    std::vector<std::wstring> elements;

    size_t offset = 0;

    size_t previous = 0;

    size_t index = 0;
    while ((offset = find_first_of(self, delimiters, previous, index)) != std::wstring::npos) {
        elements.push_back(self.substr(previous, offset - previous));
        previous = offset + delimiters.at(index).length();
    }
    elements.push_back(self.substr(previous));

    return elements;
}

std::wstring ReplaceAll(std::wstring self, const std::wstring& searchValue, const std::wstring& replaceValue) {
    if (searchValue.empty()) return self;

    size_t i = 0;
    while ((i = self.find(searchValue, i)) != std::wstring::npos) {
        self.replace(i, searchValue.length(), replaceValue);
        i += replaceValue.length();
    }

    return self;
}


class ModuleResolverResourceHandler : public CefResourceHandler {
public:
    ModuleResolverResourceHandler() {};

    bool Open(
        CefRefPtr<CefRequest> request,
        bool& handle_request,
        CefRefPtr<CefCallback> callback
    ) {
        handle_request = true;

        auto url = request->GetURL();

        auto const wurl = url.ToWString();
        if (wurl.find_last_of(L'?') != std::wstring::npos) {
            this->status = 400;
            this->statusText = "Query Parameters Not Allowed";
            return true;
        }
        if (wurl.find_last_of(L'#') != std::wstring::npos) {
            this->status = 400;
            this->statusText = "Fragment Not Allowed";
            return true;
        }

        CefURLParts parts;
        if (!CefParseURL(url, parts)) {
            this->status = 500;
            this->statusText = "Invalid URL";
            return true;
        }

        if (parts.path.length < 2) {
            //422 Invalid Module Specifier
            this->status = 422;
            this->statusText = "Invalid Module Specifier";

            return true;
        }

        //bare specifier
        std::wstring specifier = std::wstring(&(parts.path.str[1]), parts.path.length - 1);
        
        std::wstring path;
        /*
        if (auto const code = this->PACKAGE_RESOLVE(specifier, path); code != RESOLVE_OK) {
        
        }
        */

        this->status = 501;
        this->statusText = "Not Implemented";

        return true;
    }

    void GetResponseHeaders(
        CefRefPtr<CefResponse> response,
        int64_t& response_length,
        CefString& redirectUrl
    ) override {

        response->SetStatus(this->status);
        response->SetStatusText(this->statusText);
        response->SetHeaderByName("Access-Control-Allow-Origin", "*", true);

        response_length = 0;
    }
    
    void Cancel() override { }

private:
    /*
    RESOLVE_STATUS_CODE READ_PACKAGE_JSON(const std::wstring& packageURL, CefRefPtr<CefDictionaryValue>& pjson) {

        std::ifstream file(packageURL + L"/package.json", std::ios::binary | std::ios::ate);

        if (!file.is_open()) return RESOLVE_MODULE_NOT_FOUND;

        const auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::unique_ptr<char[]> buffer(new char[size]);
        file.read(buffer.get(), size);
        file.close();

        if (!file) return RESOLVE_INTERNAL_ERROR;

        auto const json = CefParseJSON(buffer.get(), size, JSON_PARSER_RFC);

        if (json == NULL) return RESOLVE_INVALID_PACKAGE_CONFIGURATION;

        if (json->GetType() != VTYPE_DICTIONARY) return RESOLVE_INVALID_PACKAGE_CONFIGURATION;

        pjson = json->GetDictionary();

        return RESOLVE_OK;
    }

    RESOLVE_STATUS_CODE PACKAGE_RESOLVE(const std::wstring packageSpecifier, const std::optional<std::wstring> parentURL, std::optional<std::wstring>& ret) {
        std::wstring packageName;

        if (!packageSpecifier.starts_with(L"@")) {
            auto const i = packageSpecifier.find_first_of(L'/');
            packageName = packageSpecifier.substr(0, i);
        } else {
            auto const i = packageSpecifier.find_first_of(L'/');
            if (i == std::wstring::npos) return RESOLVE_INVALID_MODULE_SPECIFIER;
            auto const j = packageSpecifier.find_first_of(L'/', i + 1);
            packageName = packageSpecifier.substr(0, j);
        }

        std::wstring packageSubpath = L"." + packageSpecifier.substr(packageName.length());

        if (packageSubpath.back() == L'/') return RESOLVE_INVALID_MODULE_SPECIFIER;

        const std::wstring packageURL = getLocation() + L"/node_modules/" + packageName;

        auto pjson = CefRefPtr<CefDictionaryValue>();

        if (auto const code = READ_PACKAGE_JSON(packageURL, pjson); code != RESOLVE_OK) return code;

        auto const exports = pjson->GetValue("exports");

        return PACKAGE_EXPORTS_RESOLVE(packageSubpath, exports, ret);
    }

    RESOLVE_STATUS_CODE isConditionalExportsMainSugar(CefRefPtr<CefValue> exports, bool& isConditionalSugar) {
        switch (exports->GetType()) {
            case VTYPE_INVALID:
            case VTYPE_NULL:
            case VTYPE_BOOL:
            case VTYPE_INT:
            case VTYPE_DOUBLE:
            case VTYPE_BINARY:
                return RESOLVE_INVALID_PACKAGE_CONFIGURATION;

            case VTYPE_STRING:
            case VTYPE_LIST:
                isConditionalSugar = true;
                break;

            case VTYPE_DICTIONARY: {
                CefDictionaryValue::KeyList keys{};
                exports->GetDictionary()->GetKeys(keys);
                bool hasdotkey = false;
                bool hasnondotkey = false;
                for (auto& key : keys) {
                    bool dotkey = key.GetStruct()->length > 0 && key.GetStruct()->str[0] == L'.';
                    hasdotkey |= dotkey;
                    hasnondotkey |= !dotkey;
                    if (hasdotkey && hasnondotkey) return RESOLVE_INVALID_PACKAGE_CONFIGURATION;
                }
                isConditionalSugar = hasnondotkey;
            } break;

            default:
                return RESOLVE_INTERNAL_ERROR;
        }

        return RESOLVE_OK;
    }
    
    RESOLVE_STATUS_CODE PACKAGE_EXPORTS_RESOLVE(const std::wstring& subpath, CefRefPtr<CefValue> exports, std::optional<std::wstring>& ret) {
        
        bool isConditionalSugar = false;
        if (const auto code = isConditionalExportsMainSugar(exports, isConditionalSugar); code != RESOLVE_OK) return code;

        if (subpath == L".") {
            CefRefPtr<CefValue> mainExport;
            if (isConditionalSugar) {
                mainExport = exports;
            } else {
                mainExport = exports->GetDictionary()->GetValue(".");
            }

            if (mainExport != NULL) {
                //PACKAGE_TARGET_RESOLVE
            }
        } else {
            if (!subpath.starts_with(L"./")) return RESOLVE_INVALID_MODULE_SPECIFIER;
            //PACKAGE_IMPORTS_EXPORTS_RESOLVE
        }

        return RESOLVE_PACKAGE_PATH_NOT_EXPORTED;
    }

    RESOLVE_STATUS_CODE PACKAGE_TARGET_RESOLVE(
        const std::wstring& packageURL,
        const CefRefPtr<CefValue> target,
        const std::optional<std::wstring> patternMatch,
        const bool isImports,
        const std::vector<std::wstring>& conditions,
        std::optional<std::wstring>& ret
    ) {
        switch (target->GetType()) {
            case VTYPE_STRING: {
                const auto targetC = CefURIDecode(target->GetString(), false, UU_NORMAL);
                const auto targetW = targetC.ToWString();

                if (!targetW.starts_with(L"./")) {
                    if (isImports || targetW.starts_with(L"../") || targetW.starts_with(L"/") || isValidURL(targetC)) {
                        return RESOLVE_INVALID_PACKAGE_TARGET;
                    }
                    if (patternMatch.has_value()) {
                        return PACKAGE_RESOLVE(ReplaceAll(targetW, L"*", patternMatch.value()), packageURL + L"/", ret);
                    }
                    return PACKAGE_RESOLVE(targetW, packageURL + L"/", ret);
                } else {
                    std::wstring targetWLD;
                    std::transform(targetW.begin(), targetW.end(), targetWLD.begin(), std::tolower);

                    auto const segments = Split(targetWLD, { L"/", L"\\" });
                    if (
                        std::find(segments.begin(), segments.end(), L"") == segments.end() ||
                        std::find(segments.begin(), segments.end(), L".") == segments.end() ||
                        std::find(segments.begin(), segments.end(), L"..") == segments.end() ||
                        std::find(segments.begin() + 1, segments.end(), L"node_modules") == segments.end()
                    ) {
                        return RESOLVE_INVALID_PACKAGE_TARGET;
                    }
                }

            } break;
            case VTYPE_DICTIONARY: {

            } break;
            case VTYPE_LIST: {

            } break;
            default: return RESOLVE_INVALID_PACKAGE_TARGET;
        }
    }
    */

    int status = 200;
    CefString statusText{};

    IMPLEMENT_REFCOUNTING(ModuleResolverResourceHandler);
    DISALLOW_COPY_AND_ASSIGN(ModuleResolverResourceHandler);
};

CefRefPtr<CefResourceHandler> ModuleResolverSchemeHandlerFactory::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    return new ModuleResolverResourceHandler();
}

ModuleResolverSchemeHandlerFactory::ModuleResolverSchemeHandlerFactory() {

}