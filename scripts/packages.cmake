function(define_project
        PROJECT_NAME
        CXX_VERSION
)
    # Keep Packages as child of target, SimpleCPP is project do some other stuff like versioning and CXX version etc here
    # CXX VS C Project

    # Create a library using this project
    # This library has no code and links all other packges in the project to it
    add_library(${PROJECT_NAME} INTERFACE)

    # Set properties of project library
    set_target_properties(${PROJECT_NAME} PROPERTIES PACKAGES "")

    # Set CXX Version
    set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD ${CXX_VERSION}
            CXX_STANDARD_REQUIRED ON
    )

    message(STATUS "${PROJECT_NAME}: Using compiler ${CMAKE_CXX_COMPILER_ID} on ${CMAKE_SYSTEM}!")

    # Set the project of the current scope
    set(CURRENT_SCOPE_PROJECT ${PROJECT_NAME} PARENT_SCOPE)
endfunction()

# Function for creating packages
macro(_add_package_impl TARGET_NAME LIBRARY_TYPE)
    # Ensure we are in project scope
    if (NOT DEFINED CURRENT_SCOPE_PROJECT)
        message(FATAL_ERROR "Package ${TARGET_NAME} was not created in the scope of a project!")
    endif ()

    # Ensure we are NOT in package scope
    if (DEFINED CURRENT_SCOPE_PACKAGE)
        message(FATAL_ERROR "${CURRENT_SCOPE_PROJECT}: Package ${TARGET_NAME} was created in the scope of another package!")
    endif ()

    # Get project packages
    get_target_property(PROJECT_PACKAGES ${CURRENT_SCOPE_PROJECT} PACKAGES)

    # Ensure package does not already exist
    if (${TARGET_NAME} IN_LIST PROJECT_PACKAGES)
        message(FATAL_ERROR "${CURRENT_SCOPE_PROJECT}: Package ${TARGET_NAME} already exists!")
        return()
    endif ()

    # Create library with headers and sources
    add_library(${TARGET_NAME} ${LIBRARY_TYPE}
            ${ARGN}
    )

    # Ensure target is using CXX
    set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)

    # Get cpp version from project and set target properties
    get_target_property(CXX_VERSION ${CURRENT_SCOPE_PROJECT} CXX_STANDARD)
    set_target_properties(${TARGET_NAME} PROPERTIES
            CXX_STANDARD ${CXX_VERSION}
            CXX_STANDARD_REQUIRED ON
    )

    # This tells other libraries that this target has been linked downstream to enable cross-package compatibility without hard requirements
    string(TOUPPER ${TARGET_NAME} UPPER_TARGET_NAME)
    target_compile_definitions(${TARGET_NAME} INTERFACE USING_${UPPER_TARGET_NAME})
    string(TOUPPER ${CURRENT_SCOPE_PROJECT} UPPER_PROJECT_NAME)
    target_compile_definitions(${TARGET_NAME} INTERFACE USING_${UPPER_PROJECT_NAME})

    # If config is using the predefined cmake 'Debug' type, add a definition USING_DEBUG
    target_compile_definitions(${TARGET_NAME} INTERFACE
            $<$<CONFIG:Debug>:USING_DEBUG>
    )

    # Get the top level source directory, can be used to determine the executable directory from a build environment
    if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        string(REPLACE "/" "\\\\" DEBUG_ROOT_DIR "${CMAKE_SOURCE_DIR}")
        string(REPLACE "/" "\\\\" DEBUG_BINARY_ROOT_DIR "${CMAKE_BINARY_DIR}")
    endif()
    target_compile_definitions(${TARGET_NAME} INTERFACE DEBUG_ROOT_DIR="${DEBUG_ROOT_DIR}")

    # Get the top level binary directory, can be used to determine the where the binary is and where the source is.
    target_compile_definitions(${TARGET_NAME} INTERFACE DEBUG_BINARY_ROOT_DIR="${DEBUG_BINARY_ROOT_DIR}")

    # Add to list of project packages
    # Prepend to ensure that the lowest dependency is at the bottom
    set_property(TARGET ${CURRENT_SCOPE_PROJECT} APPEND PROPERTY PACKAGES ${TARGET_NAME})

    # Set the package of the current scope
    set(CURRENT_SCOPE_PACKAGE ${TARGET_NAME} PARENT_SCOPE)
endmacro()

function(add_interface_package TARGET_NAME)
    _add_package_impl(${TARGET_NAME} INTERFACE ${ARGN})

    # Include Headers for this package
    target_include_directories(${TARGET_NAME} INTERFACE
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endfunction()

function(add_static_package TARGET_NAME)
    _add_package_impl(${TARGET_NAME} STATIC ${ARGN})

    # Include Headers for this package
    target_include_directories(${TARGET_NAME} PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endfunction()

function(add_shared_package TARGET_NAME)
    _add_package_impl(${TARGET_NAME} SHARED ${ARGN})

    # Include Headers for this package
    target_include_directories(${TARGET_NAME} PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endfunction()

# Quickly link two packages together with safety
function(link_package LINK_LIBRARY_TYPE LINK_LIBRARY_NAME)
    # Ensure we are in project scope
    if (NOT DEFINED CURRENT_SCOPE_PROJECT)
        message(FATAL_ERROR "Attempted to link package while not in the scope of a project!")
    endif ()

    get_target_property(PROJECT_PACKAGES ${CURRENT_SCOPE_PROJECT} PACKAGES)

    # Ensure target package is part of the same project (TODO: should I do this?)
    if (NOT ${LINK_LIBRARY_NAME} IN_LIST PROJECT_PACKAGES)
        message(FATAL_ERROR "${CURRENT_SCOPE_PROJECT}: Cannot link ${CURRENT_SCOPE_PACKAGE} to ${LINK_LIBRARY_NAME}, Package ${LINK_LIBRARY_NAME} does not exist!")
    endif ()

    # Ensure target package is a valid target
    if (NOT TARGET ${LINK_LIBRARY_NAME})
        message(FATAL_ERROR "${CURRENT_SCOPE_PROJECT}: Cannot link ${CURRENT_SCOPE_PACKAGE} to ${LINK_LIBRARY_NAME}, Package ${LINK_LIBRARY_NAME} is not a target!")
    endif()

    target_link_libraries(${CURRENT_SCOPE_PACKAGE} ${LINK_LIBRARY_TYPE} ${LINK_LIBRARY_NAME})
endfunction()

# Quickly adds a test for packages
function(add_test TEST_NAME)
    # Ensure we are in package scope
    if (NOT DEFINED CURRENT_SCOPE_PACKAGE)
        message(FATAL_ERROR "Tried to add test ${TEST_NAME} while not inside package scope.")
    endif ()

    # Add test as executable and link to package
    add_executable(${CURRENT_SCOPE_PACKAGE}-${TEST_NAME}
            ${ARGN}
    )
    target_link_libraries(${CURRENT_SCOPE_PACKAGE}-${TEST_NAME} ${CURRENT_SCOPE_PACKAGE})

    # Set the test of the current scope
    set(CURRENT_SCOPE_TEST ${TEST_NAME} PARENT_SCOPE)
endfunction()

# Quickly link a test to another package
function(link_test LIBRARY_NAME)
    # Ensure we are in project scope
    if (NOT DEFINED CURRENT_SCOPE_PROJECT)
        message(FATAL_ERROR "Attempted to link test while not in the scope of a project!")
    endif ()

    # Get Packages
    get_target_property(PROJECT_PACKAGES ${CURRENT_SCOPE_PROJECT} PACKAGES)

    # Ensure we are in test scope
    if (NOT DEFINED CURRENT_SCOPE_TEST)
        message(FATAL_ERROR "${CURRENT_SCOPE_PROJECT}: Tried to link test to library ${LIBRARY_NAME} while not inside test scope.")
    endif ()

    # Ensure target package is part of the same project (TODO: should I do this?)
    if (NOT ${LIBRARY_NAME} IN_LIST PROJECT_PACKAGES)
        message(FATAL_ERROR "${CURRENT_SCOPE_PROJECT}: Cannot link test ${CURRENT_SCOPE_TEST} to ${LIBRARY_NAME}, Package ${LIBRARY_NAME} does not exist!")
    endif ()

    # Ensure the target package is a test
    if (NOT TARGET ${LIBRARY_NAME})
        message(FATAL_ERROR "${CURRENT_SCOPE_PROJECT}: Cannot link test ${CURRENT_SCOPE_TEST} to ${LIBRARY_NAME}, Package ${LIBRARY_NAME} is not a target!")
    endif()

    target_link_libraries(${CURRENT_SCOPE_PACKAGE}-${CURRENT_SCOPE_TEST} ${LIBRARY_NAME})

    # Add test directory of the target package as an include directory
    if (EXISTS "${CMAKE_SOURCE_DIR}/packages/${LIBRARY_NAME}/test")
        target_include_directories(${CURRENT_SCOPE_PACKAGE}-${CURRENT_SCOPE_TEST} PRIVATE "${CMAKE_SOURCE_DIR}/packages/${LIBRARY_NAME}/test")
    endif()

endfunction()

# Quickly add an option with a message to a package
function(simplecpp_option TARGET_NAME OPTION_NAME OPTION_DESCRIPTION OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_DESCRIPTION} ${OPTION_DEFAULT})

    if(${OPTION_NAME})
        message(STATUS "${CURRENT_SCOPE_PROJECT} - ${TARGET_NAME}: ${OPTION_NAME} Is On")
        target_compile_definitions(${TARGET_NAME} INTERFACE ${OPTION_NAME})
    endif()
endfunction()