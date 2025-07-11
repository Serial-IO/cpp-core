#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../../version.h"
#include "../get_version.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc getVersion
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API void getVersionSequential(cpp_core::Version *out)
    {
        cpp_core::detail::seq::call([=] { getVersion(out); });
    }

#ifdef __cplusplus
}
#endif
