#pragma once

#include <type_traits>

#ifndef USING_MSVC
#include <bits/stl_pair.h>
#else
#include <utility>
#endif

#ifndef USING_MSVC
template <typename TType>
using implicitly_default_constructible = std::__is_implicitly_default_constructible<TType>;
#else
template <typename TType>
using implicitly_default_constructible = std::_Is_implicitly_default_constructible<TType>;
#endif
// To keep it simpler than the default std::pair, we rewrite it to be more usable.
// Not used internally in maps, just for Container use for implicit conversion and easy usage
template <typename TKeyType, typename TValueType>
struct TPair {

	using KeyType = TKeyType;
	using ValueType = TValueType;

    template <class TOtherKeyType = TKeyType, class TOtherValueType = TValueType,
        std::enable_if_t<std::conjunction_v<std::is_default_constructible<TOtherKeyType>, std::is_default_constructible<TOtherValueType>>, int> = 0
	>
#if CXX_VERSION >= 20
	constexpr explicit(!std::conjunction_v<implicitly_default_constructible<TOtherKeyType>, implicitly_default_constructible<TOtherValueType>>)
#endif
	TPair()
	noexcept(std::is_nothrow_default_constructible_v<TOtherKeyType> && std::is_nothrow_default_constructible_v<TOtherValueType>) // strengthened
	: first(), second() {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, const TOtherKeyType&>, std::is_constructible<TValueType, const TOtherValueType&>>, int> = 0
	>
#if CXX_VERSION >= 20
	constexpr explicit(!std::conjunction_v<std::is_convertible<const TOtherKeyType&, TKeyType>, std::is_convertible<const TOtherValueType&, TValueType>>)
#endif
	TPair(const TPair<TOtherKeyType, TOtherValueType>& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, const TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, const TOtherValueType&>)
	: first(pair.first), second(pair.second) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType&>, std::is_constructible<TValueType, TOtherValueType&>>, int> = 0
	>
#if CXX_VERSION >= 20
	constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType&, TKeyType>, std::is_convertible<TOtherValueType&, TValueType>>)
#endif
	TPair(TPair<TOtherKeyType, TOtherValueType>& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, TOtherValueType&>)
	: first(pair.first), second(pair.second) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType>, std::is_constructible<TValueType, TOtherValueType>>, int> = 0
	>
#if CXX_VERSION >= 20
	constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType, TKeyType>, std::is_convertible<TOtherValueType, TValueType>>)
#endif
	TPair(TPair<TOtherKeyType, TOtherValueType>&& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType> && std::is_nothrow_constructible_v<TValueType, TOtherValueType>)
	: first(std::forward<TOtherKeyType>(pair.first)), second(std::forward<TOtherValueType>(pair.second)) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, const TOtherKeyType&>, std::is_constructible<TValueType, const TOtherValueType&>>, int> = 0
	>
#if CXX_VERSION >= 20
    constexpr explicit(!std::conjunction_v<std::is_convertible<const TOtherKeyType&, TKeyType>, std::is_convertible<const TOtherValueType&, TValueType>>)
#endif
	TPair(const std::pair<TOtherKeyType, TOtherValueType>& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, const TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, const TOtherValueType&>)
	: first(pair.first), second(pair.second) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType&>, std::is_constructible<TValueType, TOtherValueType&>>, int> = 0
	>
#if CXX_VERSION >= 20
	constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType&, TKeyType>, std::is_convertible<TOtherValueType&, TValueType>>)
#endif
	TPair(std::pair<TOtherKeyType, TOtherValueType>& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, TOtherValueType&>)
	: first(pair.first), second(pair.second) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType>, std::is_constructible<TValueType, TOtherValueType>>, int> = 0
	>
#if CXX_VERSION >= 20
    constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType, TKeyType>, std::is_convertible<TOtherValueType, TValueType>>)
#endif
	TPair(std::pair<TOtherKeyType, TOtherValueType>&& pair)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType> && std::is_nothrow_constructible_v<TValueType, TOtherValueType>)
	: first(std::forward<TOtherKeyType>(pair.first)), second(std::forward<TOtherValueType>(pair.second)) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, const TOtherKeyType&>, std::is_constructible<TValueType, const TOtherValueType&>>, int> = 0
	>
#if CXX_VERSION >= 20
    constexpr explicit(!std::conjunction_v<std::is_convertible<const TOtherKeyType&, TKeyType>, std::is_convertible<const TOtherValueType&, TValueType>>)
#endif
	TPair(const TOtherKeyType& fst, const TOtherValueType& snd)
	noexcept(std::is_nothrow_constructible_v<TKeyType, const TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, const TOtherValueType&>)
	: first(fst), second(snd) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType&>, std::is_constructible<TValueType, TOtherValueType&>>, int> = 0
	>
#if CXX_VERSION >= 20
	constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType&, TKeyType>, std::is_convertible<TOtherValueType&, TValueType>>)
#endif
	TPair(TOtherKeyType& fst, TOtherValueType& snd)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType&> && std::is_nothrow_constructible_v<TValueType, TOtherValueType&>)
	: first(fst), second(snd) {}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TKeyType, TOtherKeyType>, std::is_constructible<TValueType, TOtherValueType>>, int> = 0
	>
#if CXX_VERSION >= 20
    constexpr explicit(!std::conjunction_v<std::is_convertible<TOtherKeyType, TKeyType>, std::is_convertible<TOtherValueType, TValueType>>)
#endif
	TPair(TOtherKeyType&& fst, TOtherValueType&& snd)
	noexcept(std::is_nothrow_constructible_v<TKeyType, TOtherKeyType> && std::is_nothrow_constructible_v<TValueType, TOtherValueType>)
	: first(std::forward<TOtherKeyType>(fst)), second(std::forward<TOtherValueType>(snd)) {}

	operator std::pair<TKeyType, TValueType>() const {
		return std::make_pair(first, second);
	}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, const TOtherKeyType&>, std::is_assignable<TValueType&, const TOtherValueType&>>, int> = 0
	>
	TPair& operator=(const TPair<TOtherKeyType, TOtherValueType>& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, const TOtherKeyType&> && std::is_nothrow_assignable_v<TValueType&, const TOtherValueType&>) {
		first = otr.first;
		second = otr.second;
		return *this;
	}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, TOtherKeyType&>, std::is_assignable<TValueType&, TOtherValueType&>>, int> = 0
	>
	TPair& operator=(TPair<TOtherKeyType, TOtherValueType>& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, TOtherKeyType&> && std::is_nothrow_assignable_v<TValueType&, TOtherValueType&>) {
		first = otr.first;
		second = otr.second;
		return *this;
	}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, TOtherKeyType>, std::is_assignable<TValueType&, TOtherValueType>>, int> = 0
	>
	TPair& operator=(TPair<TOtherKeyType, TOtherValueType>&& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, TOtherKeyType> && std::is_nothrow_assignable_v<TValueType&, TOtherValueType>) {
		first = std::forward<TOtherKeyType>(otr.first);
		second = std::forward<TOtherValueType>(otr.second);
		return *this;
	}

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, const TOtherKeyType&>, std::is_assignable<TValueType&, const TOtherValueType&>>, int> = 0
	>
	TPair& operator=(const std::pair<TOtherKeyType, TOtherValueType>& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, const TOtherKeyType&> && std::is_nothrow_assignable_v<TValueType&, const TOtherValueType&>) {
    	first = otr.first;
    	second = otr.second;
    	return *this;
    }

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, TOtherKeyType&>, std::is_assignable<TValueType&, TOtherValueType&>>, int> = 0
	>
	TPair& operator=(std::pair<TOtherKeyType, TOtherValueType>& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, TOtherKeyType&> && std::is_nothrow_assignable_v<TValueType&, TOtherValueType&>) {
    	first = otr.first;
    	second = otr.second;
    	return *this;
    }

	template <typename TOtherKeyType = TKeyType, typename TOtherValueType = TValueType,
		std::enable_if_t<std::conjunction_v<std::is_assignable<TKeyType&, TOtherKeyType>, std::is_assignable<TValueType&, TOtherValueType>>, int> = 0
	>
	TPair& operator=(std::pair<TOtherKeyType, TOtherValueType>&& otr)
	noexcept(std::is_nothrow_assignable_v<TKeyType&, TOtherKeyType> && std::is_nothrow_assignable_v<TValueType&, TOtherValueType>) {
    	first = std::forward<TOtherKeyType>(otr.first);
    	second = std::forward<TOtherValueType>(otr.second);
    	return *this;
    }

	template <class TOtherKeyType, class TOtherValueType>
	friend constexpr bool operator==(const TPair& lhs, const TPair<TOtherKeyType, TOtherValueType>& rhs) {
		return lhs.first == rhs.first && lhs.second == rhs.second;
	}

	template <class TOtherKeyType, class TOtherValueType>
	friend constexpr bool operator==(const TPair& lhs, const std::pair<TOtherKeyType, TOtherValueType>& rhs) {
    	return lhs.first == rhs.first && lhs.second == rhs.second;
    }
	
	TKeyType first;
	TValueType second;
	
private:

	template<typename, typename>
	friend struct TPair;
};

template <typename TKeyType, typename TValueType>
TPair(const std::pair<TKeyType, TValueType>&) -> TPair<TKeyType, TValueType>;

template <typename TKeyType, typename TValueType>
TPair(std::pair<TKeyType, TValueType>&) -> TPair<TKeyType, TValueType>;

template <typename TKeyType, typename TValueType>
TPair(std::pair<TKeyType, TValueType>&&) -> TPair<TKeyType, TValueType>;

template <typename TKeyType, typename TValueType>
TPair(const TKeyType& fst, const TValueType& snd) -> TPair<TKeyType, TValueType>;
