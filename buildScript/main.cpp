#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "json.h"

#if defined(_WIN32)
typedef std::wstring sbsString;
#define SBS_STR(x) L##x
#define SBS_OUT std::wcout
#else
typedef std::string sbsString;
#define SBS_STR(x) x
#define SBS_OUT std::cout
#endif

int runMain(const std::vector<sbsString>& args);

// Windows
#if defined(_WIN32)
#include <windows.h>

int wmain(int, const wchar_t* const* const);

// MinGW
#if !defined(_MSC_VER)
#include <shellapi.h>
int main(int argc, const char* const* const) {
    wchar_t** wargv;
    wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    return wmain(argc, wargv);
}
#endif

int wmain(const int argc, const wchar_t* const* const argv)
#else
int main(const int argc, const char* const* const argv)
#endif
{
    runMain(std::vector<sbsString>{argv, argv + argc});
    return 0;
}

void printHelp();
int runLaunch(const std::vector<sbsString>& args);

int runMain(const std::vector<sbsString>& args) {
    if (args.size() > 1 && args[1] == SBS_STR("launch")) {
        return runLaunch(args);
    }

    SBS_OUT << SBS_STR("No arguments given!") << std::endl << std::endl;
    printHelp();
    return 0;
}

void printHelp() {
    SBS_OUT << SBS_STR("usage: sbs <command> [arguments]") << std::endl << std::endl;
    SBS_OUT << SBS_STR("launch  Launches a program by ") << std::endl;
}

void readModuleInfo(const sbsString& location) {
    FILE* file;

    std::filesystem::path path(location.c_str());
    path = std::filesystem::absolute(path);
    path.append("module_info.json");

#if defined(_WIN32)
    _wfopen_s(&file, path.c_str(), L"r");
#else
    _fopen_s(&file, path.c_str(), "r");
#endif

    if (file == nullptr) {
        SBS_OUT << SBS_STR("Could not open file at ") << path << std::endl;
        return;
    }

    nlohmann::json data = nlohmann::json::parse(file);

    if (!data.contains("__info_version")) {
        SBS_OUT << SBS_STR("No version present in info file!") << std::endl;
        return;
    }

    if (!data.contains("module_name")) {
        SBS_OUT << SBS_STR("Could not find module name") << std::endl;
        return;
    }
    const auto ver = data.at("__info_version").get<uint32_t>();
    const auto str = data.at("module_name").get<std::string>();

    std::cout << "Info Ver: " << ver << std::endl;
    std::cout << "Program: " << str << std::endl;

    const auto dependencies = data.at("dependencies");

    const auto pubdeps = dependencies.at("public").get<std::vector<std::string>>();

    std::cout << "Public Deps:" << std::endl;
    for (auto& publicdep : pubdeps) {
        std::cout << publicdep << std::endl;
    }

    const auto prideps = dependencies.at("private").get<std::vector<std::string>>();

    std::cout << "Private Deps:" << std::endl;
    for (auto& pridep : prideps) {
        std::cout << pridep << std::endl;
    }

    fclose(file);
}

int runLaunch(const std::vector<sbsString>& args) {
    if (args.size() <= 2) {
        SBS_OUT << SBS_STR("Not enough arguments for program launch!") << std::endl;
        SBS_OUT << SBS_STR("usage: sbs launch [program_directory]") << std::endl;
        return 0;
    }

    //launch thing
    SBS_OUT << SBS_STR("Launch program ") << args[2] << SBS_STR("!") << std::endl;

    readModuleInfo(args[2]);

    return 0;
}