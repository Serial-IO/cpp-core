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
        int major = version::MAJOR;                        ///< Semantic-version major component.
        int minor = version::MINOR;                        ///< Semantic-version minor component.
        int patch = version::PATCH;                        ///< Semantic-version patch component.
        int commits_since_tag = version::GIT_COMMIT_COUNT; ///< Commits since the closest Git tag.
        int is_dirty = version::GIT_IS_DIRTY ? 1 : 0;      ///< 1 for uncommitted changes, otherwise 0.

        const char *version_string = version::VERSION;              ///< Complete generated version string.
        const char *prerelease = version::PRERELEASE;               ///< Prerelease identifier, or an empty string.
        const char *prerelease_type = version::PRERELEASE_TYPE;     ///< Prerelease kind such as `alpha` or `rc`.
        const char *prerelease_number = version::PRERELEASE_NUMBER; ///< Prerelease number, or an empty string.

        const char *git_tag = version::GIT_TAG;                     ///< Closest Git tag.
        const char *git_describe_hash = version::GIT_DESCRIBE_HASH; ///< Hash component reported by Git describe.
        const char *git_commit_hash_short = version::GIT_COMMIT_HASH_SHORT; ///< Abbreviated commit hash.
        const char *git_commit_hash_full = version::GIT_COMMIT_HASH_FULL;   ///< Full commit hash.
        const char *git_commit_date = version::GIT_COMMIT_DATE;             ///< Commit timestamp including timezone.
        const char *git_branch = version::GIT_BRANCH;                       ///< Branch name used for the build.
        const char *git_dirty_suffix = version::GIT_DIRTY_SUFFIX;           ///< `-dirty` or an empty string.
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
