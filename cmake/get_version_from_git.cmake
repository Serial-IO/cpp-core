# Function to get version from Git tags
# If a tag exists (e.g., v1.2.3 or 1.2.3), use it (with -dirty suffix if working tree is dirty)
# If no tag exists, use 0.0.0 (with -dirty suffix if working tree is dirty)

function(get_version_from_git)
    # Check if we're in a git repository
    find_package(Git QUIET)
    if(NOT Git_FOUND)
        set(VERSION_MAJOR 0 PARENT_SCOPE)
        set(VERSION_MINOR 0 PARENT_SCOPE)
        set(VERSION_PATCH 0 PARENT_SCOPE)
        set(VERSION_STRING "0.0.0-dev-no-git" PARENT_SCOPE)
        return()
    endif()

    # Get the latest tag
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    # Check if working tree is dirty
    execute_process(
        COMMAND ${GIT_EXECUTABLE} diff --quiet --exit-code
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_DIRTY_RESULT
        ERROR_QUIET
    )

    # Get short commit hash

    if(GIT_TAG)
        # Extract version from tag (remove 'v' prefix if present)
        string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_TAG}")
        string(REGEX REPLACE "^v?[0-9]+\\.([0-9]+)\\..*" "\\1" VERSION_MINOR "${GIT_TAG}")
        string(REGEX REPLACE "^v?[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_TAG}")

        # If regex didn't match, try simpler pattern
        if(NOT VERSION_MAJOR MATCHES "^[0-9]+$")
            string(REGEX REPLACE "^v?([0-9]+)" "\\1" VERSION_MAJOR "${GIT_TAG}")
        endif()
        if(NOT VERSION_MINOR MATCHES "^[0-9]+$")
            set(VERSION_MINOR "0")
        endif()
        if(NOT VERSION_PATCH MATCHES "^[0-9]+$")
            set(VERSION_PATCH "0")
        endif()

        # Check if dirty - if dirty, mark the version with -dirty suffix
        if(GIT_DIRTY_RESULT)
            set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-dirty")
        else()
            set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
        endif()
    else()
        # No tag found, use 0.0.0 (with -dirty if dirty)
        set(VERSION_MAJOR "0")
        set(VERSION_MINOR "0")
        set(VERSION_PATCH "0")
        if(GIT_DIRTY_RESULT)
            set(VERSION_STRING "0.0.0-dirty")
        else()
            set(VERSION_STRING "0.0.0")
        endif()
    endif()

    # Return values to parent scope
    set(VERSION_MAJOR ${VERSION_MAJOR} PARENT_SCOPE)
    set(VERSION_MINOR ${VERSION_MINOR} PARENT_SCOPE)
    set(VERSION_PATCH ${VERSION_PATCH} PARENT_SCOPE)
    set(VERSION_STRING ${VERSION_STRING} PARENT_SCOPE)
endfunction()

