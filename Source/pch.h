#pragma once

#undef assert
#define assert(expr) if (!(expr)) { \
    void ShowAssertDialogue(); \
    ShowAssertDialogue(); \
}

#define _HAS_EXCEPTIONS 0

// We include the debug headers early, so we can override the assert macros.
#include <crtdbg.h>
#undef _RPT_BASE
#define _RPT_BASE(...) \
    void ShowAssertDialogue(); \
    ShowAssertDialogue();

#undef _RPT_BASE_W
#define _RPT_BASE_W(...) \
    void ShowAssertDialogue(); \
    ShowAssertDialogue();

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
