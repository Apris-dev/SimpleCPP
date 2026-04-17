#pragma once

#include <forward_list>
#include "Container.h"
#include "sutil/InitializerList.h"

template <typename TType>
struct TForwardList : TSequenceContainer<TForwardList<TType>> {

	using Super = TSequenceContainer<TForwardList>;

	TForwardList() = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	TForwardList(TInitializerList<TType> init): m_Container(init) {}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	explicit TForwardList(TArgs&&... args) {
		(m_Container.emplace_front(std::forward<TArgs>(args)), ...);
	}

	TForwardList(const std::forward_list<TType>& otr): m_Container(otr) {}

	[[nodiscard]] size_t getSize() const {
		return SIZE(m_Container);
	}

	[[nodiscard]] bool isEmpty() const {
		return m_Container.empty();
	}

	[[nodiscard]] TType& top() {
		return m_Container.front();
	}

	[[nodiscard]] const TType& top() const {
		return m_Container.front();
	}

	[[nodiscard]] typename Super::Iterator begin() noexcept {
		return m_Container.begin();
	}

	[[nodiscard]] typename Super::ConstIterator begin() const noexcept {
		return m_Container.begin();
	}

	[[nodiscard]] typename Super::Iterator end() noexcept {
		return m_Container.end();
	}

	[[nodiscard]] typename Super::ConstIterator end() const noexcept {
		return m_Container.end();
	}

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TType>)
	bool contains(const TType& obj) const {
		return CONTAINS(m_Container, obj);
	}

#ifdef USING_SIMPLEPTR
	bool contains(typename TUnfurled<TType>::Type* obj) const {
		if constexpr (sstl::is_managed_v<TType>) {
			// Will compare pointers, is always comparable
			return CONTAINS(m_Container, obj, TUnfurled<TType>::get);
		} else {
			return contains(*obj);
		}
	}
#endif

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TType>)
	size_t find(const TType& obj) const {
		return DISTANCE(m_Container, obj);
	}

#ifdef USING_SIMPLEPTR
	size_t find(typename TUnfurled<TType>::Type* obj) const {
		if constexpr (sstl::is_managed_v<TType>) {
			// Will compare pointers, is always comparable
			return DISTANCE(m_Container, obj, TUnfurled<TType>::get);
		} else {
			return find(*obj);
		}
	}
#endif

	TType& get(size_t index) {
		auto itr = m_Container.begin();
		std::advance(itr, index);
		return *itr;
	}

	const TType& get(size_t index) const {
		auto itr = m_Container.begin();
		std::advance(itr, index);
		return *itr;
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	void resize(size_t amt) {
		m_Container.resize(amt);
	}

	void resize(const size_t amt, std::function<TType(size_t)> func) {
		const size_t previousSize = getSize();
		for (size_t i = previousSize; i < amt; ++i) {
			m_Container.emplace_front(std::forward<TType>(func(i)));
		}
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	TType& push() {
		m_Container.emplace_front();
		return get(getSize() - 1);
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	size_t push(const TType& obj) {
		m_Container.emplace_front(obj);
		return getSize() - 1;
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	size_t push(TType&& obj) {
		m_Container.emplace_front(std::move(obj));
		return getSize() - 1;
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void push(const size_t index, const TType& obj) {
		auto itr = m_Container.before_begin();
		std::advance(itr, index);
		m_Container.insert_after(itr, obj);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void push(const size_t index, TType&& obj) {
		auto itr = m_Container.before_begin();
		std::advance(itr, index);
		m_Container.insert_after(itr, std::move(obj));
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void replace(const size_t index, const TType& obj) {
		popAt(index);
		push(index, obj);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void replace(const size_t index, TType&& obj) {
		popAt(index);
		push(index, std::move(obj));
	}

	void clear() {
		m_Container.clear();
	}

	void pop() {
		m_Container.erase_after(m_Container.before_begin());
	}

	void popAt(const size_t index) {
		auto itr = m_Container.before_begin();
		std::advance(itr, index);
		m_Container.erase_after(itr);
	}

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TType>)
	void pop(const TType& obj) {
		m_Container.erase_after(std::remove(m_Container.before_begin(), m_Container.end(), obj), m_Container.end());
	}

#ifdef USING_SIMPLEPTR
	void pop(typename TUnfurled<TType>::Type* obj) {
		if constexpr (sstl::is_managed_v<TType>) {
			// Will compare pointers, is always comparable
			m_Container.erase_after(std::remove(m_Container.before_begin(), m_Container.end(), obj), m_Container.end());
		} else {
			pop(*obj);
		}
	}
#endif

	template <typename TOtherContainerType>
	void transfer(TSequenceContainer<TOtherContainerType>& otr, const size_t index) {
		// Prefer move, but copy if not available
		auto& obj = get(index);
		if constexpr (std::is_move_constructible_v<TType>) {
			otr.push(std::move(obj));
		} else {
			otr.push(obj);
		}
		popAt(index);
	}

	// Forward List transfer can use splicing
	void transfer(TSequenceContainer<TForwardList>& list, const size_t index) {
		auto itr = m_Container.before_begin();
		std::advance(itr, index);
		list._derived().m_Container.splice_after(list._derived().m_Container.before_begin(), m_Container, itr);
	}

protected:

	template <typename>
	friend struct TList;

	std::forward_list<TType> m_Container;
};

template <typename TType>
struct TContainerTraits<TForwardList<TType>> {
	using Type = TType;
	using ContainerType = std::forward_list<TType>;
	using Iterator = typename ContainerType::iterator;
	using ConstIterator = typename ContainerType::const_iterator;
	constexpr static bool bIsContiguousMemory = false;
	constexpr static bool bIsLimitedAccess = false;
};

template <typename TType, typename... TArgs>
TForwardList(TType, TArgs...) -> TForwardList<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
