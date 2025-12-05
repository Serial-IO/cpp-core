#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    using ErrorCallbackT = void (*)(int error_code, const char *message);

#ifdef __cplusplus
} // extern "C"
#endif
