#include "pch.h"
#include "debuglog.h"

#ifdef _DEBUG

#include <windows.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

// File logging using raw Win32 (CreateFileW/WriteFile), not the CRT.
//
// Rationale: the DLL is injected while the process is still suspended and DllMain
// runs under the loader lock, at which point the host CRT may not have finished
// initializing. std::ofstream can fail silently there (no exception thrown, no
// error reported), so here we use CreateFileW/WriteFile, record the exact
// GetLastError of every open, and also write a "canary" file in %TEMP% so we can
// confirm, without a debugger, that initialization ran and where each log was
// attempted.
//
// Paths in order of preference:
//   1. <DLL directory>\<same name>.log
//   2. %TEMP%\<same name>.log
//   3. %USERPROFILE%\<same name>.log (the game process can always write to the
//      user profile, e.g. C:\Users\<user>\)
// Output is always mirrored to OutputDebugString.

namespace {
    CRITICAL_SECTION g_cs = {};
    bool g_csReady = false;
    HANDLE g_hFile = INVALID_HANDLE_VALUE;      // next to the DLL
    HANDLE g_hTempFile = INVALID_HANDLE_VALUE;  // fallback in %TEMP%
    HANDLE g_hHomeFile = INVALID_HANDLE_VALUE;  // fallback in %USERPROFILE%
    bool g_initialized = false;

    void Lock() {
        if (g_csReady) {
            EnterCriticalSection(&g_cs);
        }
    }

    void Unlock() {
        if (g_csReady) {
            LeaveCriticalSection(&g_cs);
        }
    }

    std::wstring ModulePathLong(HMODULE hModule) {
        std::vector<wchar_t> buf(32768, L'\0');
        DWORD len = GetModuleFileNameW(hModule, buf.data(), static_cast<DWORD>(buf.size()));
        if (len == 0 || len >= buf.size()) {
            return L"";
        }
        return std::wstring(buf.data(), len);
    }

    std::wstring StemOf(const std::wstring& modPath) {
        size_t slash = modPath.find_last_of(L"\\/");
        std::wstring fname = (slash == std::wstring::npos) ? modPath : modPath.substr(slash + 1);
        size_t dot = fname.find_last_of(L".");
        if (dot != std::wstring::npos) {
            fname = fname.substr(0, dot);
        }
        return fname;
    }

    std::wstring DirOf(const std::wstring& modPath) {
        size_t slash = modPath.find_last_of(L"\\/");
        return (slash == std::wstring::npos) ? L"" : modPath.substr(0, slash + 1);
    }

    std::wstring TempDir() {
        wchar_t tmp[MAX_PATH] = L"";
        DWORD len = GetTempPathW(MAX_PATH, tmp);
        if (len == 0 || len >= MAX_PATH) {
            return L"";
        }
        return std::wstring(tmp);
    }

    std::wstring UserProfileDir() {
        wchar_t buf[MAX_PATH] = L"";
        DWORD len = GetEnvironmentVariableW(L"USERPROFILE", buf, MAX_PATH);
        if (len == 0 || len >= MAX_PATH) {
            return L"";
        }
        return std::wstring(buf);
    }

    HANDLE OpenLogFile(const std::wstring& path) {
        return CreateFileW(
            path.c_str(),
            FILE_APPEND_DATA,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
    }

    void WriteChannel(const char* s) {
        if (s == nullptr || *s == '\0') {
            return;
        }
        DWORD len = static_cast<DWORD>(std::strlen(s));
        const char eol[] = "\r\n";
        const DWORD eolLen = static_cast<DWORD>(sizeof(eol) - 1);
        HANDLE handles[3] = { g_hFile, g_hTempFile, g_hHomeFile };
        for (HANDLE h : handles) {
            if (h == INVALID_HANDLE_VALUE) {
                continue;
            }
            DWORD written = 0;
            WriteFile(h, s, len, &written, nullptr);
            WriteFile(h, eol, eolLen, &written, nullptr);
            FlushFileBuffers(h);
        }
    }
}

void DebugLogInit(HMODULE hModule) {
    if (!g_csReady) {
        InitializeCriticalSection(&g_cs);
        g_csReady = true;
    }
    Lock();
    if (g_initialized) {
        Unlock();
        return;
    }
    g_initialized = true;

    std::wstring modPath = ModulePathLong(hModule);
    std::wstring stem = modPath.empty() ? L"AgeDebug" : StemOf(modPath);
    std::wstring primary = modPath.empty() ? L"" : DirOf(modPath) + stem + L".log";

    DWORD errPrimary = 0;
    if (!primary.empty()) {
        g_hFile = OpenLogFile(primary);
        if (g_hFile == INVALID_HANDLE_VALUE) {
            errPrimary = GetLastError();
        }
    }

    std::wstring tempLog;
    DWORD errTemp = 0;
    {
        std::wstring tmpDir = TempDir();
        if (!tmpDir.empty()) {
            tempLog = tmpDir + stem + L".log";
            if (tempLog != primary) {
                g_hTempFile = OpenLogFile(tempLog);
                if (g_hTempFile == INVALID_HANDLE_VALUE) {
                    errTemp = GetLastError();
                }
            }
        }
    }

    std::wstring homeLog;
    DWORD errHome = 0;
    {
        std::wstring homeDir = UserProfileDir();
        if (!homeDir.empty()) {
            homeLog = homeDir + L"\\" + stem + L".log";
            if (homeLog != primary && homeLog != tempLog) {
                g_hHomeFile = OpenLogFile(homeLog);
                if (g_hHomeFile == INVALID_HANDLE_VALUE) {
                    errHome = GetLastError();
                }
            }
        }
    }

    // Canary: present as soon as DebugLogInit runs and the process can write,
    // showing the real paths and error codes. It is tried in %TEMP% and in
    // %USERPROFILE%.
    char canaryLine[3072] = "";
    snprintf(canaryLine, sizeof(canaryLine),
        "[pid=%lu] DebugLogInit module='%ls' primary='%ls' err=%lu temp='%ls' err=%lu home='%ls' err=%lu\n",
        static_cast<unsigned long>(GetCurrentProcessId()),
        modPath.c_str(), primary.c_str(), static_cast<unsigned long>(errPrimary),
        tempLog.c_str(), static_cast<unsigned long>(errTemp),
        homeLog.c_str(), static_cast<unsigned long>(errHome));
    {
        std::wstring tmpDir = TempDir();
        if (!tmpDir.empty()) {
            std::wstring canary = tmpDir + L"AgeDebug-canary.log";
            HANDLE h = OpenLogFile(canary);
            if (h != INVALID_HANDLE_VALUE) {
                DWORD written = 0;
                WriteFile(h, canaryLine, static_cast<DWORD>(std::strlen(canaryLine)), &written, nullptr);
                FlushFileBuffers(h);
                CloseHandle(h);
            }
        }
    }
    {
        std::wstring homeDir = UserProfileDir();
        if (!homeDir.empty()) {
            std::wstring canary = homeDir + L"\\AgeDebug-canary.log";
            HANDLE h = OpenLogFile(canary);
            if (h != INVALID_HANDLE_VALUE) {
                DWORD written = 0;
                WriteFile(h, canaryLine, static_cast<DWORD>(std::strlen(canaryLine)), &written, nullptr);
                FlushFileBuffers(h);
                CloseHandle(h);
            }
        }
    }

    char diag[3072] = "";
    snprintf(diag, sizeof(diag),
        "DebugLogInit pid=%lu module='%ls' primary='%ls' open=%d err=%lu temp='%ls' open=%d err=%lu home='%ls' open=%d err=%lu",
        static_cast<unsigned long>(GetCurrentProcessId()),
        modPath.c_str(), primary.c_str(), g_hFile != INVALID_HANDLE_VALUE ? 1 : 0,
        static_cast<unsigned long>(errPrimary),
        tempLog.c_str(), g_hTempFile != INVALID_HANDLE_VALUE ? 1 : 0,
        static_cast<unsigned long>(errTemp),
        homeLog.c_str(), g_hHomeFile != INVALID_HANDLE_VALUE ? 1 : 0,
        static_cast<unsigned long>(errHome));
    OutputDebugStringA(diag);
    OutputDebugStringA("\n");

    SYSTEMTIME st{};
    GetLocalTime(&st);
    char header[256] = "";
    snprintf(header, sizeof(header), "=== ATTACH pid=%lu %04u-%02u-%02u %02u:%02u:%02u.%03u ===",
        static_cast<unsigned long>(GetCurrentProcessId()),
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    OutputDebugStringA(header);
    OutputDebugStringA("\n");
    WriteChannel(header);
    Unlock();
}

void DebugLogShutdown() {
    Lock();
    if (!g_initialized) {
        Unlock();
        return;
    }
    char line[256] = "";
    snprintf(line, sizeof(line), "=== DETACH pid=%lu ===", static_cast<unsigned long>(GetCurrentProcessId()));
    OutputDebugStringA(line);
    OutputDebugStringA("\n");
    WriteChannel(line);

    if (g_hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(g_hFile);
        g_hFile = INVALID_HANDLE_VALUE;
    }
    if (g_hTempFile != INVALID_HANDLE_VALUE && g_hTempFile != g_hFile) {
        CloseHandle(g_hTempFile);
        g_hTempFile = INVALID_HANDLE_VALUE;
    }
    if (g_hHomeFile != INVALID_HANDLE_VALUE && g_hHomeFile != g_hFile && g_hHomeFile != g_hTempFile) {
        CloseHandle(g_hHomeFile);
        g_hHomeFile = INVALID_HANDLE_VALUE;
    }
    g_initialized = false;
    Unlock();
}

void DebugLogWrite(const char* func, const char* fmt, ...) {
    char msg[2048] = "";
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);
    msg[sizeof(msg) - 1] = '\0';

    SYSTEMTIME st{};
    GetLocalTime(&st);
    DWORD pid = GetCurrentProcessId();
    DWORD tid = GetCurrentThreadId();

    char line[2300] = "";
    snprintf(line, sizeof(line), "[%02u:%02u:%02u.%03u][pid=%lu][tid=%lu][%s] %s",
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
        static_cast<unsigned long>(pid), static_cast<unsigned long>(tid),
        func != nullptr ? func : "?", msg);
    line[sizeof(line) - 1] = '\0';

    OutputDebugStringA(line);
    OutputDebugStringA("\n");

    Lock();
    if (g_initialized) {
        WriteChannel(line);
    }
    Unlock();
}

#endif // _DEBUG