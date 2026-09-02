#pragma once
#include "../module_api.h"
#include "../version.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

    namespace cpp_core
    {
    /**
     * Build and source metadata for the loaded cpp_core-based library.
     */
    struct Meta
    {
        int major = version::MAJOR;
        int minor = version::MINOR;
        int patch = version::PATCH;
        int commits_since_tag = version::GIT_COMMIT_COUNT;
        int is_dirty = version::GIT_IS_DIRTY ? 1 : 0;

        const char *version_string = version::VERSION;
        const char *prerelease = version::PRERELEASE;
        const char *prerelease_type = version::PRERELEASE_TYPE;
        const char *prerelease_number = version::PRERELEASE_NUMBER;

        const char *git_tag = version::GIT_TAG;
        const char *git_describe_hash = version::GIT_DESCRIBE_HASH;
        const char *git_commit_hash_short = version::GIT_COMMIT_HASH_SHORT;
        const char *git_commit_hash_full = version::GIT_COMMIT_HASH_FULL;
        const char *git_commit_date = version::GIT_COMMIT_DATE;
        const char *git_branch = version::GIT_BRANCH;
        const char *git_dirty_suffix = version::GIT_DIRTY_SUFFIX;
    };
    } // namespace cpp_core

    /**
     * @brief Copy metadata for the loaded library.
     *
     * @param[out] out Structure receiving the metadata. Passing `nullptr` is a no-op.
     */
    MODULE_API void meta(cpp_core::Meta *out);

#ifdef __cplusplus
}
#endif
