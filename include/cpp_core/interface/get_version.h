#pragma once
#include "../module_api.h"
#include "../version.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

    namespace cpp_core
    {
    struct Version
    {
        int major = version::MAJOR;
        int minor = version::MINOR;
        int patch = version::PATCH;
    };
    } // namespace cpp_core

    /**
     * @brief Copy the compile-time version of the cpp_core library.
     *
     * Writes the version baked into the library at build-time to the structure
     * pointed to by @p out. If @p out is `nullptr` the call is a no-op.
     *
     * @param[out] out Pointer to a `cpp_core::Version` structure that receives the version information. May be
     * `nullptr`.
     */
    inline MODULE_API void getVersion(cpp_core::Version *out)
    {
        if (out != nullptr)
        {
            *out = cpp_core::Version();
        }
    }

#ifdef __cplusplus
}
#endif
