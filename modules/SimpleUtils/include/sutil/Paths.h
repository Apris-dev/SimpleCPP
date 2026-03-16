#pragma once

#include <string>

// TODO, paths cannot be a predefined macro, as project root will change in production environment

extern std::string gSimpleCPPRoot;

#ifdef SIMPLEUTILS_PATH_IMPLEMENTATION
std::string gSimpleCPPRoot = std::filesystem::current_path().string();
#endif

inline std::string gTempPath = std::filesystem::temp_directory_path().string();