# Simple CPP

A collection of libraries designed to make c++ usage less complicated and easier

## Requirements

SimpleCPP requires at least C++17, though it is recommended to use the latest c++ release for newer features.

## Setup

The recommended method is to use git submodules, but copying into your project should also work fine.

### Copy into project

You can copy SimpleCPP (or any of its modules, but be wary as they may depend on each other) into your project, and add subdirectory with cmake.

### Git Submodules

This setup may be more complex initially, but allows for a lot of freedom.  I recommend setting up the project as a subdirectory to avoid too much clutter in your main project.

Run git submodule like so, replace third_party/simplecpp with any name you want.

```
git submodule add https://github.com/StrideStudios/SimpleCPP third_party/simplecpp
```

If using CMake, use add_subdirectory in your project.
```
add_subdirectory(SimpleCPP)
```

Then link using either method:
```
# Link the overall library, this just includes all the modules at once
target_link_libraries(your_target SimpleCPP)
```

```
# Link an individual module.  This can be useful for larger projects that do not need each at once
target_link_libraries(your_target SimpleCPP-SimpleUtils)
target_link_libraries(your_target SimpleCPP-SimpleDG)
target_link_libraries(your_target SimpleCPP-SimpleSTL)
...
```