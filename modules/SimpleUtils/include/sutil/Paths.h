#pragma once

#include <string>

#ifdef USING_WINDOWS
#include <windows.h>
#include <libloaderapi.h>
#endif

#ifdef USING_LINUX
#include <unistd.h>
#endif

// TODO, paths cannot be a predefined macro, as project root will change in production environment

inline std::string gTempPath = std::filesystem::temp_directory_path().string();

inline std::string gExecutablePath = [] {
    std::string path;
#ifdef USING_WINDOWS
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    path = std::string(buffer);
#elif USING_LINUX
    char buffer[4097];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    buffer[len] = '\0';
    path = std::string(buffer);
#else //TODO: MacOS
    std::cerr << "Not implemented" << std::endl;
#endif

#if USING_DEBUG
    // Change binary dir to root instead for debug builds
    for (size_t i = 0; i < path.length(); ++i) {
        if (path.substr(0, i) == DEBUG_BINARY_ROOT_DIR) {
            path = DEBUG_ROOT_DIR + path.substr(i);
            break;
        }
    }
#endif

    CPathArchive archive(path);
    archive.previous();

    return archive.get();
}();