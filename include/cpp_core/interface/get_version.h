#pragma once
#include "../module_api.h"
#include "../version.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
            *out = cpp_core::kVersion;
        }
    }

#ifdef __cplusplus
}
#endif
