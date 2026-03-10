#include <iostream>
#include <filesystem>

#include "sarch/FileArchive.h"
#include "sarch/HashArchive.h"
#include "sarch/PathArchive.h"

int main() {

    {
        CPathArchive pathTest(SIMPLECPP_ROOT_DIR);
        pathTest << "test";

        std::cout << pathTest.get() << std::endl;

        std::cout << std::endl;

        std::string v;
        pathTest >> v;

        std::cout << pathTest.get() << std::endl;
        std::cout << v << std::endl;

    }


    CPathArchive path(SIMPLECPP_ROOT_DIR);
    path << "modules" << "SimpleArchive" << "test" << "input.dat";

    {
        CFileArchive<EOpenType::BINARY_WRITE> fileArchive(path.get());

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test2";
    }

    {
        CFileArchive<EOpenType::BINARY_READ> fileArchive(path.get());

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
