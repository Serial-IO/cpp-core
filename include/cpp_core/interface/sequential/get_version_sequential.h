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
     * @note Sequential variant: call executes strictly in the order it was enqueued across threads.
     */
    inline MODULE_API void getVersionSequential(cpp_core::Version *out)
    {
        cpp_core::detail::seq::call([=] { getVersion(out); });
    }

#ifdef __cplusplus
}
#endif
