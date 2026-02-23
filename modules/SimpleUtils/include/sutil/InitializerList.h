#pragma once

#include <type_traits>

#include "../../../SimpleArchive/include/sarch/HashArchive.h"

namespace sstl {
    template <class TType, class... TArgs>
    struct EnforceConvertible {
        static_assert(std::conjunction_v<std::is_convertible<TType, TArgs>...>, "An argument in TArgs is not convertible to TType!");
        using Type = TType;
    };
}

template <class TType>
using TInitializerList = std::initializer_list<TType>;

template <typename TType>
size_t getHash(const TInitializerList<TType>& list) {
    std::size_t hash = list.size();
    for (const auto& obj : list)
        shash::combine(hash, obj);
    return hash;
}