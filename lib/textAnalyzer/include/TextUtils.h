#pragma once
#include <string>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <locale>
#include <codecvt>

inline std::string cleanWord(const std::string& input) {
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide;
    try {
        wide = converter.from_bytes(input);
    }
    catch (...) {
        return "";
    }

    std::wstring filtered;
    for (wchar_t c : wide) {
        if (iswalnum(c) || c == L'-') {
            filtered += c;
        }
    }
    return converter.to_bytes(filtered);
}
