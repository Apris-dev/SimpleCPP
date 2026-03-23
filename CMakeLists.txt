# CMake Parent File Requirements
cmake_minimum_required(VERSION 3.28...4.2)

# Project Definition
project(
        SimpleCPP
        DESCRIPTION "A set of libraries designed to make usage of C++ as simple as possible"
        VERSION 1.0.0
        LANGUAGES CXX C
)

# If not overridden, STL CXX Standard is the same as parent
if(NOT SimpleCPP_CXX_STANDARD)
    set(SimpleCPP_CXX_STANDARD ${CMAKE_CXX_STANDARD})
endif ()

# If CXX Standard is not defined, fallback to c++17
if(NOT SimpleCPP_CXX_STANDARD)
    set(SimpleCPP_CXX_STANDARD 17)
    message(WARNING "SimpleCPP: Could not find parent CXX standard; Using default.")
endif()

message(STATUS "SimpleCPP: Using CXX version ${SimpleCPP_CXX_STANDARD}")

# CMAKE_SOURCE_DIR is the top level cmake file, in this case, we can get the project that depends on simplecpp
set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
# CMAKE_CURRENT_SOURCE_DIR is the current level cmake file, so in this case, the root of simplecpp
set(SIMPLECPP_PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(SIMPLECPP_COMPILER GCC)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(SIMPLECPP_COMPILER CLANG)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(SIMPLECPP_COMPILER MSVC)
else()
    message(FATAL_ERROR "SimpleCPP: Unknown compiler ${CMAKE_CXX_COMPILER_ID}, please use a supported compiler!")
endif ()

# Set the used platform and version for SimpleCPP
string(TOUPPER ${CMAKE_SYSTEM_NAME} SIMPLECPP_PLATFORM)
set(SIMPLECPP_PLATFORM_VERSION ${CMAKE_SYSTEM_VERSION})

message(STATUS "SimpleCPP: Using compiler ${SIMPLECPP_COMPILER} on ${SIMPLECPP_PLATFORM} ${SIMPLECPP_PLATFORM_VERSION}!")

# Function for creating simplecpp modules in subdirectories
function(create_simplecpp_module TARGET_NAME LIBRARY_TYPE)

    # Create library with headers and sources
    add_library(SimpleCPP-${TARGET_NAME} ${LIBRARY_TYPE}
            ${ARGN}
    )

    # Ensure target is using CXX
    set_target_properties(SimpleCPP-${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)

    # Create Macro with CXX Version
    target_compile_definitions(SimpleCPP-${TARGET_NAME} ${LIBRARY_TYPE} CXX_VERSION=${SimpleCPP_CXX_STANDARD})

    # Tell target what compiler we are using
    target_compile_definitions(SimpleCPP-${TARGET_NAME} ${LIBRARY_TYPE} USING_${SIMPLECPP_COMPILER})

    # Tell target the platform and platform version we are using
    target_compile_definitions(SimpleCPP-${TARGET_NAME} ${LIBRARY_TYPE} USING_${SIMPLECPP_PLATFORM})
    target_compile_definitions(SimpleCPP-${TARGET_NAME} ${LIBRARY_TYPE} PLATFORM_VERSION="${SIMPLECPP_PLATFORM_VERSION}")

    target_compile_definitions(SimpleCPP-${TARGET_NAME} INTERFACE
            $<$<CONFIG:Debug>:USING_DEBUG>
    )

    # Get the top level source directory, can be used to determine the executable directory from a build environment
    target_compile_definitions(SimpleCPP-${TARGET_NAME} INTERFACE DEBUG_ROOT_DIR="${CMAKE_SOURCE_DIR}")

    # Get the top level binary directory, can be used to determine the where the binary is and where the source is.
    target_compile_definitions(SimpleCPP-${TARGET_NAME} INTERFACE DEBUG_BINARY_ROOT_DIR="${CMAKE_BINARY_DIR}")

    # Tell target the root directory of the project, (the project using simplecpp)
    target_compile_definitions(SimpleCPP-${TARGET_NAME} INTERFACE PROJECT_ROOT_DIR="${PROJECT_ROOT_DIR}")

    # Get the root directory of simplecpp
    target_compile_definitions(SimpleCPP-${TARGET_NAME} INTERFACE SIMPLECPP_PROJECT_ROOT_DIR="${SIMPLECPP_PROJECT_ROOT_DIR}")

    # This tells other libraries that this target has been linked downstream to enable cross-module compatibility without hard requirements
    string(TOUPPER ${TARGET_NAME} UPPER_TARGET_NAME)
    target_compile_definitions(SimpleCPP-${TARGET_NAME} ${LIBRARY_TYPE} USING_${UPPER_TARGET_NAME})

    # Ensure target is compiled with CXX Version
    target_compile_features(SimpleCPP-${TARGET_NAME} ${LIBRARY_TYPE} "cxx_std_${SimpleCPP_CXX_STANDARD}")

    # Include Headers for this module
    target_include_directories(SimpleCPP-${TARGET_NAME} ${LIBRARY_TYPE}
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endfunction()

# Quickly link two simplecpp modules together
function(link_simplecpp_module TARGET_NAME LIBRARY_TYPE LIBRARY_NAME)
    if (NOT TARGET SimpleCPP-${TARGET_NAME})
        message(FATAL_ERROR "Tried to link a module ${TARGET_NAME} with link_simplecpp_module that is not a simplecpp module")
    endif()
    if (NOT TARGET SimpleCPP-${LIBRARY_NAME})
        message(FATAL_ERROR "Tried to link a module ${LIBRARY_NAME} with link_simplecpp_module that is not a simplecpp module")
    endif()

    target_link_libraries(SimpleCPP-${TARGET_NAME} ${LIBRARY_TYPE} SimpleCPP-${LIBRARY_NAME})
endfunction()

# Quickly link a test to another module
function(link_simplecpp_test TARGET_NAME TEST_NAME LIBRARY_NAME)
    if (NOT TARGET SimpleCPP-${TARGET_NAME}-${TEST_NAME})
        message(FATAL_ERROR "Tried to link a test ${TEST_NAME} (${TARGET_NAME}) with link_simplecpp_module that is not a simplecpp module")
    endif()
    if (NOT TARGET SimpleCPP-${LIBRARY_NAME})
        message(FATAL_ERROR "Tried to link a module ${LIBRARY_NAME} with link_simplecpp_module that is not a simplecpp module")
    endif()

    target_link_libraries(SimpleCPP-${TARGET_NAME}-${TEST_NAME} SimpleCPP-${LIBRARY_NAME})

    # Add test directory of the target module as an include directory
    if (EXISTS "${CMAKE_SOURCE_DIR}/modules/${LIBRARY_NAME}/test")
        target_include_directories(SimpleCPP-${TARGET_NAME}-${TEST_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/modules/${LIBRARY_NAME}/test")
    endif()

endfunction()

# Quickly add an option with a message to a simplecpp module
function(simplecpp_option TARGET_NAME OPTION_NAME OPTION_DESCRIPTION OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_DESCRIPTION} ${OPTION_DEFAULT})

    if(${OPTION_NAME})
        message(STATUS "${TARGET_NAME}: ${OPTION_NAME} Is On")
        target_compile_definitions(SimpleCPP-${TARGET_NAME} INTERFACE ${OPTION_NAME})
    endif()
endfunction()

# Quickly adds a test for simplecpp modules
function(add_simplecpp_test TARGET_NAME TEST_NAME)
    add_executable(SimpleCPP-${TARGET_NAME}-${TEST_NAME}
            ${ARGN}
    )
    target_link_libraries(SimpleCPP-${TARGET_NAME}-${TEST_NAME} SimpleCPP-${TARGET_NAME})
endfunction()

# Simple add module to SimpleCPP
function(add_module MODULE_NAME)
    add_subdirectory(modules/${MODULE_NAME})
    target_link_libraries(SimpleCPP INTERFACE SimpleCPP-${MODULE_NAME})
endfunction()

option(SIMPLECPP_TEST "Enable testing SimpleCPP features" ${PROJECT_IS_TOP_LEVEL})

if(SIMPLECPP_TEST)
    message(STATUS "SimpleCPP: Enabled Testing")
    enable_testing()
endif()

add_library(SimpleCPP INTERFACE)

# Create a list of modules so compile definitions can be created in each module
set(SIMPLECPP_MODULES
        SimpleUtils
        SimplePtr
        SimpleSTL
        SimpleArchive
        SimpleDG
)

foreach (MODULE IN LISTS SIMPLECPP_MODULES)
    add_subdirectory(modules/${MODULE})
    target_link_libraries(SimpleCPP INTERFACE SimpleCPP-${MODULE})
endforeach()

# Enable testing for each module if requested
# We do this after creating the modules so the tests dont need to be ordered
if (SIMPLECPP_TEST)
    foreach (MODULE IN LISTS SIMPLECPP_MODULES)
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/modules/${MODULE}/test")
            add_subdirectory(modules/${MODULE}/test)
        endif()
    endforeach()
endif()