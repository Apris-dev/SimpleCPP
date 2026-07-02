add_subdirectory(util)
add_subdirectory(memory)
add_subdirectory(container)
add_subdirectory(archive)
add_subdirectory(dependencyGraph)

if (CPPNS_TEST)
    add_subdirectory(testing)
endif ()