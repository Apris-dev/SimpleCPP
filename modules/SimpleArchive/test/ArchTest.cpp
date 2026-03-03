#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_set>

#include "sarch/FileArchive.h"
#include "sarch/HashArchive.h"

int main() {

    size_t count = 0;
    size_t collisions = 0;

    CFileArchive<EOpenType::READ> input(R"(C:/Repos/SimpleCPP/modules/SimpleArchive/test/input.res)");

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

    return 0;
}
