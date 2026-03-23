#include <iostream>

#include "sarch/FileArchive.h"
#include "sarch/PathArchive.h"

#include "sutil/Paths.h"

int main() {

    CPathArchive path(gExecutablePath);
    path << "input.dat";

    std::cout << path.get() << std::endl;

    {
        CFileArchive<EOpenType::BINARY_WRITE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
    }

    {
        CFileArchive<EOpenType::BINARY_READ> fileArchive(path);

        size_t v;
        fileArchive >> v;
        std::string s;
        fileArchive >> s;
        std::string s2;
        fileArchive >> s2;

        std::cout << v << std::endl;
        std::cout << s << std::endl;
        std::cout << s2 << std::endl;
    }

    return 0;
}
