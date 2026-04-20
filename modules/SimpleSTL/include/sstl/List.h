#pragma once

#include <list>
#include "Container.h"
#include "sutil/InitializerList.h"

template <typename TType>
struct TList : TSequenceContainer<TList<TType>> {

	using Super = TSequenceContainer<TList>;

#ifdef USING_SIMPLEPTR
	using typename Super::TUnfurledType;
#endif

	TList() = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	TList(TInitializerList<TType> init): m_Container(init) {}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	explicit TList(TArgs&&... args) {
		(m_Container.emplace_back(std::forward<TArgs>(args)), ...);
	}

	TList(const std::list<TType>& otr): m_Container(otr) {}

	[[nodiscard]] size_t getSize() const {
		return m_Container.size();
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

	[[nodiscard]] typename Super::ReverseIterator rbegin() noexcept {
		return m_Container.rbegin();
	}

	[[nodiscard]] typename Super::ConstReverseIterator rbegin() const noexcept {
		return m_Container.rbegin();
	}

	[[nodiscard]] typename Super::Iterator end() noexcept {
		return m_Container.end();
	}

	[[nodiscard]] typename Super::ConstIterator end() const noexcept {
		return m_Container.end();
	}

	[[nodiscard]] typename Super::ReverseIterator rend() noexcept {
		return m_Container.rend();
	}

	[[nodiscard]] typename Super::ConstReverseIterator rend() const noexcept {
		return m_Container.rend();
	}

	[[nodiscard]] bool isValid(size_t index) const {
		return index > 0 && index < getSize();
	}

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TType>)
	bool contains(const TType& obj) const {
		return CONTAINS(m_Container, obj);
	}

#ifdef USING_SIMPLEPTR
	bool contains(const TFrail<TUnfurledType>& obj) const {
		// Will compare pointers, is always comparable
		return CONTAINS(m_Container, obj);
	}
#endif

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TType>)
	size_t find(const TType& obj) const {
		return DISTANCE(m_Container, obj);
	}

#ifdef USING_SIMPLEPTR
	size_t find(const TFrail<TUnfurledType>& obj) const {
		// Will compare pointers, is always comparable
		return DISTANCE(m_Container, obj);
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
			m_Container.emplace_back(std::forward<TType>(func(i)));
		}
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	TType& push() {
		m_Container.emplace_back();
		return get(getSize() - 1);
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	size_t push(const TType& obj) {
		m_Container.emplace_back(obj);
		return getSize() - 1;
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	size_t push(TType&& obj) {
		m_Container.emplace_back(std::move(obj));
		return getSize() - 1;
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void push(const size_t index, const TType& obj) {
		auto itr = m_Container.begin();
		std::advance(itr, index);
		m_Container.insert(itr, obj);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void push(const size_t index, TType&& obj) {
		auto itr = m_Container.begin();
		std::advance(itr, index);
		m_Container.insert(itr, std::move(obj));
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
		m_Container.erase(m_Container.begin());
	}

	void popAt(const size_t index) {
		auto itr = m_Container.begin();
		std::advance(itr, index);
		m_Container.erase(itr);
	}

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TType>)
	void pop(const TType& obj) {
		ERASE(m_Container, obj);
	}

#ifdef USING_SIMPLEPTR
	void pop(const TFrail<TUnfurledType>& obj) {
		// Will compare pointers, is always comparable
		ERASE(m_Container, obj);
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

	// List transfer can use splicing
	void transfer(TSequenceContainer<TList>& list, const size_t index) {
		auto itr = m_Container.begin();
		std::advance(itr, index);
		SContainer::derived(list).m_Container.splice(SContainer::derived(list).m_Container.begin(), m_Container, itr);
	}

	template <typename TOtherContainerType>
	void append(const TSequenceContainer<TOtherContainerType>& otr) {
#if CXX_VERSION >= 23
		m_Container.append_range(SContainer::getSubcontainer(otr));
#else
		m_Container.insert(m_Container.end(), SContainer::getSubcontainer(otr).begin(), SContainer::getSubcontainer(otr).end());
#endif
	}

protected:

	friend struct SContainer;

	auto& getSubcontainer() { return m_Container; }
	const auto& getSubcontainer() const { return m_Container; }

	std::list<TType> m_Container;
};

template <typename TType>
struct TContainerTraits<TList<TType>> {
	using Type = TType;
	using ContainerType = std::list<TType>;
	using Iterator = typename ContainerType::iterator;
	using ReverseIterator = typename ContainerType::reverse_iterator;
	using ConstIterator = typename ContainerType::const_iterator;
	using ConstReverseIterator = typename ContainerType::const_reverse_iterator;
	constexpr static bool bIsContiguousMemory = false;
	constexpr static bool bIsLimitedAccess = false;
	constexpr static bool bIsForwardOnly = true;
	constexpr static bool bIsLimitedSize = false;
};

template <typename TType, typename... TArgs>
TList(TType, TArgs...) -> TList<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
