# Designed to be used by downstream projects to simplefy c++ dependencies
# Run BEFORE project command to use
# Will use vcpkg installed on system if available
# If you wish to remove the bundled one, remove your binary directory and install on system

include(FetchContent)

function(setup_vcpkg)

	# If the target computer has vcpkg already installed, use that version
    if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED VCPKG_DIR)
        message("Setting vcpkg root to $ENV{VCPKG_ROOT}")
        set(VCPKG_DIR $ENV{VCPKG_ROOT} CACHE PATH "")
    endif ()

    # No vcpkg exists on target computer, so we install locally
    if (NOT DEFINED VCPKG_DIR)
        set(VCPKG_DIR "${CMAKE_BINARY_DIR}/vcpkg" CACHE PATH "")

        # Check for .vcpkg-root (Which is always there even in bundled version with MSVC) If it exists skip download
        if (NOT EXISTS "${VCPKG_DIR}/.vcpkg-root")
            message(STATUS "Downloading vcpkg...")
            FetchContent_Declare(
                    vcpkg
                    GIT_REPOSITORY "https://github.com/microsoft/vcpkg.git"
                    GIT_TAG "master"
                    SOURCE_DIR "${VCPKG_DIR}"
            )
            FetchContent_MakeAvailable(vcpkg)
        endif ()
    endif ()

    # Ensure vcpkg is enabled
    find_program(VCPKG_EXECUTABLE vcpkg PATHS "${VCPKG_DIR}" NO_DEFAULT_PATH)
    if (NOT VCPKG_EXECUTABLE)
        if (WIN32)
            execute_process(COMMAND "${VCPKG_DIR}/bootstrap-vcpkg.bat" WORKING_DIRECTORY "${VCPKG_DIR}")
        else ()
            execute_process(COMMAND  "${VCPKG_DIR}/bootstrap-vcpkg.sh" WORKING_DIRECTORY "${VCPKG_DIR}")
        endif ()
    endif ()
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE PATH "" FORCE)
endfunction()