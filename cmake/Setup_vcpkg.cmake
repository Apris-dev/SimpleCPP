# Designed to be used by downstream projects to simplefy c++ dependencies
# Run BEFORE project command to use
# Will use vcpkg installed on system if available
# If you wish to remove the bundled one, remove your binary directory and install on system

include(FetchContent)

function(setup_vcpkg)

    # If the target computer has vcpkg already installed, use that version
    if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED MY_VCPKG_ROOT)
        message("Setting vcpkg root to $ENV{VCPKG_ROOT}")
        set(MY_VCPKG_ROOT $ENV{VCPKG_ROOT} CACHE PATH "")
    endif ()

    # No vcpkg exists on target computer, so we install locally
    if (NOT DEFINED MY_VCPKG_ROOT)
        set(MY_VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg" CACHE PATH "")

        # Check for .vcpkg-root (Which is always there even in bundled version with MSVC) If it exists skip download
        if (NOT EXISTS "${MY_VCPKG_ROOT}/.vcpkg-root")
            message(STATUS "Downloading vcpkg...")
            FetchContent_Declare(
                    vcpkg
                    GIT_REPOSITORY "https://github.com/microsoft/vcpkg.git"
                    GIT_TAG "master"
                    SOURCE_DIR "${MY_VCPKG_ROOT}"
            )
            FetchContent_MakeAvailable(vcpkg)
        endif ()
    endif ()

    # Ensure vcpkg is enabled
    find_program(VCPKG_EXECUTABLE vcpkg PATHS "${MY_VCPKG_ROOT}" NO_DEFAULT_PATH)
    if (NOT VCPKG_EXECUTABLE)
        if (WIN32)
            execute_process(COMMAND "${MY_VCPKG_ROOT}/bootstrap-vcpkg.bat" WORKING_DIRECTORY "${MY_VCPKG_ROOT}")
        else ()
            execute_process(COMMAND  "${MY_VCPKG_ROOT}/bootstrap-vcpkg.sh" WORKING_DIRECTORY "${MY_VCPKG_ROOT}")
        endif ()
    endif ()
    set(CMAKE_TOOLCHAIN_FILE "${MY_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE PATH "" FORCE)
endfunction()