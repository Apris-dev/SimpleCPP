#include <iostream>
#include <filesystem>

#include "sarch/FileArchive.h"
#include "sarch/PathArchive.h"

#include "sutil/Paths.h"

namespace fs = std::filesystem;

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

    /*CPathArchive archive;

    archive << SIMPLECPP_ROOT_DIR;
    archive << "modules" << "SimpleArchive" << "test" << "input.res";

    std::cout << "out: " << archive.get() << std::endl;*/

    /*size_t count = 0;
    size_t collisions = 0;

    CFileArchive<EOpenType::READ> input(std::string(SIMPLECPP_ROOT_DIR) + "/modules/SimpleArchive/test/input.res");

    if (!input.isOpen()) return 0;

    std::unordered_set<size_t> seen;
    std::unordered_set<std::string> seenStrings;
    seen.max_load_factor(0.7f);

    size_t hash = 0;

    std::string x;

    while (!input.isEnd()) {
        input >> x;

        CHashArchive archive;

        archive << x;
        hash = archive.get();

        if (seenStrings.find(x) == seenStrings.end()) {
            if (seen.find(hash) != seen.end()) {
                collisions++;
            } else {
                seen.insert(hash);
            }
            seenStrings.insert(x);
        }

        count++;
    }
    std::cout << "Total: " << count << " With " << (count - seenStrings.size()) << " Duplicates" << std::endl;
    std::cout << "Collision # is " << collisions << " of " << seenStrings.size() << std::endl;
*/
    return 0;
}
