#pragma once

#define _L(x) L##x
#undef assert
#define assert(expr, message) if (!(expr)) { \
    void ShowAssertDialogue(const wchar_t*); \
    ShowAssertDialogue(_L(message)); \
}

#define _HAS_EXCEPTIONS 0

// We include the debug headers early, so we can override the assert macros.
#include <crtdbg.h>
#undef _RPT_BASE
#define _RPT_BASE(...) \
    void ShowAssertDialogue(const wchar_t*); \
    ShowAssertDialogue(L"");

#undef _RPT_BASE_W
#define _RPT_BASE_W(...) \
    void ShowAssertDialogue(const wchar_t*); \
    ShowAssertDialogue(L"");

#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1
#undef NDEBUG // Enable asserts (even in release mode)
#include <windows.h>
#undef min
#undef max

#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
