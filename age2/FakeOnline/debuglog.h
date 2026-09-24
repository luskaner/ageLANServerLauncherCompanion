#pragma once

// File logging activo solo en builds DEBUG.
//
// - En Debug (_DEBUG definido) los logs se escriben en un fichero con el mismo
//   nombre de la DLL pero extension .log, ubicado junto a la DLL
//   (p. ej. Age2FakeOnline.dll -> Age2FakeOnline.log), ademas de OutputDebugString.
// - En Release (_DEBUG no definido) DEBUG_LOG no genera codigo, no evalua sus
//   argumentos y no crea ningun fichero (coste cero).

#ifdef _DEBUG

#include <cstdio>
#include <string>

void DebugLogInit(HMODULE hModule);
void DebugLogShutdown();
void DebugLogWrite(const char* func, const char* fmt, ...);

#define DEBUG_LOG(fmt, ...) DebugLogWrite(__FUNCTION__, fmt, ##__VA_ARGS__)

// Helpers solo-DEBUG para volcar valores en los logs. Solo se usan dentro de
// argumentos de DEBUG_LOG, asi que en Release (macro a ((void)0)) no se
// compilan ni se llaman.
inline std::string DbgNarrowW(const wchar_t* ws) {
    if (ws == nullptr) {
        return "(null)";
    }
    int len = WideCharToMultiByte(CP_UTF8, 0, ws, -1, nullptr, 0, nullptr, nullptr);
    if (len <= 0) {
        return "(conv-err)";
    }
    std::string out(static_cast<size_t>(len) - 1, '\0');
    if (!out.empty()) {
        WideCharToMultiByte(CP_UTF8, 0, ws, -1, out.data(), len, nullptr, nullptr);
    }
    return out;
}

inline std::string DbgNarrowWS(const std::wstring& ws) {
    return DbgNarrowW(ws.c_str());
}

inline std::string DbgGuid(const GUID& g) {
    char buf[64] = "";
    snprintf(buf, sizeof(buf), "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        static_cast<unsigned long>(g.Data1), g.Data2, g.Data3,
        g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3],
        g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
    return buf;
}

inline std::string DbgThumbHex(const BYTE* data, size_t len) {
    static constexpr char kHex[] = "0123456789ABCDEF";
    std::string out;
    out.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        out.push_back(kHex[(data[i] >> 4) & 0xF]);
        out.push_back(kHex[data[i] & 0xF]);
    }
    return out;
}

#else

inline void DebugLogInit(HMODULE) {}
inline void DebugLogShutdown() {}
#define DEBUG_LOG(fmt, ...) ((void)0)

#endif
