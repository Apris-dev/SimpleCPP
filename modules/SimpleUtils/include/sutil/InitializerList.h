#pragma once

#include <type_traits>

namespace sstl {
    template <class TType, class... TArgs>
    struct EnforceConvertible {
        static_assert(std::conjunction_v<std::is_convertible<TType, TArgs>...>, "An argument in TArgs is not convertible to TType!");
        using Type = TType;
    };
}

template <class TType>
using TInitializerList = std::initializer_list<TType>;