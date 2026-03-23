#pragma once

#include "Common.h"
#include "Shared.h"
#include "Weak.h"
#include "Unique.h"
#include "Frail.h"
#include "SharedFrom.h"

template <typename TType>
struct TUnfurled {
	using Type = TType;
	constexpr static bool isManaged = false;

	template <typename TOtherType = TType, typename... TArgs,
		std::enable_if_t<std::is_convertible_v<TOtherType, TType>, int> = 0
	>
	constexpr static TType create(TArgs&&... args)
#if CXX_VERSION >= 20
noexcept(std::is_nothrow_convertible_v<TOtherType, TType>) {
#else
noexcept {
#endif
		TOtherType obj{std::forward<TArgs>(args)...};
		if constexpr (sstl::is_initializable_v<TOtherType>) {
			obj.init();
		}
		return obj;
	}
};

template <typename TType>
struct TUnfurled<TShared<TType>> {
	using Type = TType;
	constexpr static bool isManaged = true;
	constexpr static auto get = &TShared<TType>::get;

	template <typename TOtherType = TType, typename... TArgs,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	_CONSTEXPR23 static TShared<TType> create(TArgs&&... args)
#if CXX_VERSION >= 20
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
	noexcept {
#endif
		return TShared<TOtherType>(std::forward<TArgs>(args)...);
	}
};

template <typename TType>
struct TUnfurled<TUnique<TType>> {
	using Type = TType;
	constexpr static bool isManaged = true;
	constexpr static auto get = &TUnique<TType>::get;

	template <typename TOtherType = TType, typename... TArgs,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	_CONSTEXPR23 static TUnique<TType> create(TArgs&&... args)
#if CXX_VERSION >= 20
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
	noexcept {
#endif
		return TUnique<TOtherType>(std::forward<TArgs>(args)...);
	}
};

template <typename TType>
struct TUnfurled<TWeak<TType>> {
	using Type = TType;
	constexpr static bool isManaged = true;
	constexpr static auto get = &TWeak<TType>::get;
};

template <typename TType>
struct TUnfurled<TFrail<TType>> {
	using Type = TType;
	constexpr static bool isManaged = true;
	constexpr static auto get = &TFrail<TType>::get;
};

/*
 * Determine whether TType is a managed pointer or not
 */

namespace sstl {
	template <typename TType>
	constexpr bool is_managed_v = TUnfurled<TType>::isManaged;

	template <typename TType>
	struct is_managed : std::bool_constant<is_managed_v<TType>> {};

	template <typename TType>
	_CONSTEXPR23 TType* getUnfurled(TType* type) {
		return type;
	}

	template <typename TType>
	_CONSTEXPR23 const TType* getUnfurled(const TType* type) {
		return type;
	}

	template <typename TType>
	_CONSTEXPR23 typename TUnfurled<TType>::Type* getUnfurled(TType& type) {
		if constexpr (sstl::is_managed_v<TType>) {
			return type.get();
		} else {
			return &type;
		}
	}

	template <typename TType>
	_CONSTEXPR23 const typename TUnfurled<TType>::Type* getUnfurled(const TType& type) {
		if constexpr (sstl::is_managed_v<TType>) {
			return type.get();
		} else {
			return &type;
		}
	}
}