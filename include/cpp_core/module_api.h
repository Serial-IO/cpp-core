#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef cpp_bindings_windows_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif
#else
#define MODULE_API __attribute__((visibility("default")))
#endif
