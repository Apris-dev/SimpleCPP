#pragma once

#include <string>

#ifdef USING_WINDOWS
#include <windows.h>
#include <libloaderapi.h>
#endif

#ifdef USING_LINUX
#include <unistd.h>
#include <limits.h>
#endif

// TODO, paths cannot be a predefined macro, as project root will change in production environment

extern std::string gSimpleCPPRoot;

#ifdef SIMPLEUTILS_PATH_IMPLEMENTATION
std::string gSimpleCPPRoot = std::filesystem::current_path().string();
#endif

inline std::string gTempPath = std::filesystem::temp_directory_path().string();

// TODO: works for release builds but for testing it needs to get the project root
inline CPathArchive getExecutablePath() {
    CPathArchive path;
#if USING_DEBUG
    path << DEBUG_ROOT_DIR;
    return path;
#else
#ifdef USING_WINDOWS
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    path << std::string(buffer);
#elif USING_LINUX
    char buffer[4097];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    buffer[len] = '\0';
    path << std::string(buffer);
#else //TODO: MacOS
    std::cerr << "Not implemented" << std::endl;
#endif
    path.previous();
    return path;
#endif
}