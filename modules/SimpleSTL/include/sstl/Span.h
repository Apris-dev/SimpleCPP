#pragma once

#include "List.h"

// Generic span, unlike std::span this type supports all container types as input
// Including maps and sets (Maps will be pairs)
// Can only be const since the other types are limited
// (TType is enforced, while TContainerType is interpreted from construction)
template <typename TType, typename TOptValueType = void>
struct TSpan {

    using TTrueType = std::conditional_t<std::is_same_v<TOptValueType, void>, TType, TPair<TType, TOptValueType>>;

    TSpan() = delete;

    template <typename TOtherContainerType,
        std::enable_if_t<std::is_same_v<TTrueType, typename TContainerTraits<TOtherContainerType>::Type>, int> = 0
    >
    TSpan(const TSequenceContainer<TOtherContainerType>& inContainer) {
        for (const auto& elem : inContainer) {
            m_Container.push(elem);
        }
    }

    template <typename TOtherContainerType,
        std::enable_if_t<
            std::conjunction_v<
                std::is_same<TType, typename TContainerTraits<TOtherContainerType>::KeyType>,
                std::is_same<TOptValueType, typename TContainerTraits<TOtherContainerType>::ValueType>
            >
        , int> = 0
    >
    TSpan(const TAssociativeContainer<TOtherContainerType>& inContainer) {
        for (const auto& elem : inContainer) {
            m_Container.push(elem);
        }
    }

    template <typename TOtherContainerType,
        std::enable_if_t<std::is_same_v<TTrueType, typename TContainerTraits<TOtherContainerType>::Type>, int> = 0
    >
    TSpan(const TSelfAssociativeContainer<TOtherContainerType>& inContainer) {
        for (const auto& elem : inContainer) {
            m_Container.push(elem);
        }
    }

    decltype(auto) begin() const {
        return m_Container.begin();
    }

    decltype(auto) end() const {
        return m_Container.end();
    }

private:

    TList<std::reference_wrapper<const TTrueType>> m_Container;
};