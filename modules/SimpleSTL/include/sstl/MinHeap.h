#pragma once

#include <algorithm>
#include "Container.h"
#include "sutil/InitializerList.h"

/*
 * Min Heap does not have limited access like a queue or stack but rather always orders itself in a particular way
 * In this case, the heap guarantees the top element to always be the smallest element but does NOT guarantee ascending order
 */
template <typename TType>
struct TMinHeap : TSequenceContainer<TMinHeap<TType>> {

	using Super = TSequenceContainer<TMinHeap>;

	_CONSTEXPR20 TMinHeap() = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	_CONSTEXPR20 TMinHeap(TInitializerList<TType> init): m_Container(init) {
		std::make_heap(m_Container.begin(), m_Container.end(), MinCmp{});
	}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	_CONSTEXPR20 explicit TMinHeap(TArgs&&... args) {
		m_Container.reserve(sizeof...(TArgs));
		(m_Container.emplace_back(std::forward<TArgs>(args)), ...);
		std::make_heap(m_Container.begin(), m_Container.end(), MinCmp{});
	}

	_CONSTEXPR20 TMinHeap(const std::vector<TType>& otr): m_Container(otr) {
		std::make_heap(m_Container.begin(), m_Container.end(), MinCmp{});
	}

	[[nodiscard]] size_t getSize() const {
		return m_Container.size();
	}

	[[nodiscard]] bool isEmpty() const {
		return m_Container.empty();
	}

	[[nodiscard]] TType* data() { return m_Container.data(); }

	[[nodiscard]] const TType* data() const { return m_Container.data(); }

	[[nodiscard]] TType& top() {
		return m_Container.front();
	}

	[[nodiscard]] const TType& top() const {
		return m_Container.front();
	}

	[[nodiscard]] TType& bottom() {
		return m_Container.back();
	}

	[[nodiscard]] const TType& bottom() const {
		return m_Container.back();
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
		return m_Container[index];
	}

	const TType& get(size_t index) const {
		return m_Container[index];
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	void resize(size_t amt) {
		m_Container.resize(amt);
		std::make_heap(m_Container.begin(), m_Container.end(), MinCmp{});
	}

	void resize(size_t amt, std::function<TType(size_t)> func) {
		const size_t previousSize = getSize();
		m_Container.reserve(amt);
		for (size_t i = previousSize; i < amt; ++i) {
			m_Container.emplace_back(std::forward<TType>(func(i)));
		}
		std::make_heap(m_Container.begin(), m_Container.end(), MinCmp{});
	}

	void reserve(size_t amt) {
		m_Container.reserve(amt);
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	TType& push() {
		m_Container.emplace_back();
		std::push_heap(m_Container.begin(), m_Container.end(), MinCmp{});
		return get(getSize() - 1);
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	size_t push(const TType& obj) {
		m_Container.emplace_back(obj);
		std::push_heap(m_Container.begin(), m_Container.end(), MinCmp{});
		return getSize() - 1;
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	size_t push(TType&& obj) {
		m_Container.emplace_back(std::move(obj));
		std::push_heap(m_Container.begin(), m_Container.end(), MinCmp{});
		return getSize() - 1;
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void push(const size_t index, const TType& obj) {
		m_Container.insert(m_Container.begin() + index, obj);
		std::make_heap(m_Container.begin(), m_Container.end(), MinCmp{});
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void push(const size_t index, TType&& obj) {
		m_Container.insert(m_Container.begin() + index, std::move(obj));
		std::make_heap(m_Container.begin(), m_Container.end(), MinCmp{});
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
		std::pop_heap(m_Container.begin(), m_Container.end(), MinCmp{});
		popAt(0);
	}

	void popAt(const size_t index) {
		m_Container.erase(m_Container.begin() + index);
		std::make_heap(m_Container.begin(), m_Container.end(), MinCmp{});
	}

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TType>)
	void pop(const TType& obj) {
		ERASE(m_Container, obj);
		std::make_heap(m_Container.begin(), m_Container.end(), MinCmp{});
	}

#ifdef USING_SIMPLEPTR
	void pop(typename TUnfurled<TType>::Type* obj) {
		if constexpr (sstl::is_managed_v<TType>) {
			ERASE(m_Container, obj, TUnfurled<TType>::get);
			std::make_heap(m_Container.begin(), m_Container.end(), MinCmp{});
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

protected:

	struct MinCmp {
		bool operator()(const TType& a, const TType& b) const {
			return b < a;
		}
	};

	std::vector<TType> m_Container;
};

template <typename TType>
struct TContainerTraits<TMinHeap<TType>> {
	using Type = TType;
	using ContainerType = std::vector<TType>;
	using Iterator = typename ContainerType::iterator;
	using ConstIterator = typename ContainerType::const_iterator;
	constexpr static bool bIsContiguousMemory = true;
	constexpr static bool bIsLimitedAccess = false;
};

template <typename TType, typename... TArgs>
TMinHeap(TType, TArgs...) -> TMinHeap<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
