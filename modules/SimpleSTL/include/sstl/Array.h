#pragma once

#include <array>
#include "Container.h"
#include "sutil/InitializerList.h"

template <typename TType, size_t TSize>
struct TArray : TSequenceContainer<TArray<TType, TSize>> {

	using Super = TSequenceContainer<TArray>;
	using TPointerType = typename Super::TPointerType;

#ifdef USING_SIMPLEPTR
	using typename Super::TUnfurledType;
#endif
	
	_CONSTEXPR20 TArray() {
		m_IsPopulated.fill(false);
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	_CONSTEXPR20 TArray(TInitializerList<TType> init) {
		if (init.size() > TSize) {
			throw std::runtime_error("Initializer contains too many elements for TArray!");
		}

		m_IsPopulated.fill(false);

		size_t index = 0;
		for (const auto& obj : init) {
			m_Container[index] = obj;
			m_IsPopulated[index] = true;
			++index;
		}

	}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	_CONSTEXPR20 explicit TArray(TArgs&&... args) {
		m_IsPopulated.fill(false);
		size_t index = 0;
		(arrayArgsInit(std::forward<TArgs>(args), index), ...);
	}

	_CONSTEXPR20 TArray(const std::array<TType, TSize>& otr): m_Container(otr) {
		m_IsPopulated.fill(true);
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
		return m_IsPopulated[index];
	}

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TType>)
	bool contains(const TType& obj) const {
		return CONTAINS(m_Container, obj);
	}

#ifdef USING_SIMPLEPTR
	bool contains(TPointerType obj) const {
		// Will compare pointers, is always comparable
		return CONTAINS(m_Container, obj);
	}
#endif

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TType>)
	size_t find(const TType& obj) const {
		return DISTANCE(m_Container, obj);
	}

#ifdef USING_SIMPLEPTR
	size_t find(TPointerType obj) const {
		// Will compare pointers, is always comparable
		return DISTANCE(m_Container, obj);
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
		for (size_t i = 0; i < amt; ++i) {
			if (!m_IsPopulated[i]) {
				m_Container[i] = {};
				m_IsPopulated[i] = true;
			}
		}
	}

	void resize(const size_t amt, std::function<TType(size_t)> func) {
		for (size_t i = 0; i < amt; ++i) {
			if (!m_IsPopulated[i]) {
				get(i) = std::forward<TType>(func(i));
				m_IsPopulated[i] = true;
			}
		}
	}

	void fill() {
		resize(TSize);
	}

	void resize(std::function<TType(size_t)> func) {
		resize(TSize, func);
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	TType& push() {
		return get(push(TType{}));
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	size_t push(const TType& obj) {
		for (size_t i = 0; i < getSize(); ++i) {
			if (!m_IsPopulated[i]) { //is not populated
				m_IsPopulated[i] = true;
				m_Container[i] = obj;
				return i;
			}
		}
		throw std::runtime_error("Array is full, cannot add any more elements.");
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	size_t push(TType&& obj) {
		for (size_t i = 0; i < getSize(); ++i) {
			if (!m_IsPopulated[i]) { //is not populated
				m_IsPopulated[i] = true;
				m_Container[i] = std::move(obj);
				return i;
			}
		}
		throw std::runtime_error("Array is full, cannot add any more elements.");
	}

	void push(const size_t index, const TType& obj) {
		replace(index, obj);
	}

	void push(const size_t index, TType&& obj) {
		replace(index, std::move(obj));
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void replace(const size_t index, const TType& obj) {
		m_Container[index] = obj;
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void replace(const size_t index, TType&& obj) {
		m_Container[index] = std::move(obj);
	}

	// Array is on stack, it is destroyed when out of scope
	void clear() {
		m_IsPopulated.fill(false);
	}

	void pop() {
		for (size_t i = getSize(); i > 0; --i) {
			if (m_IsPopulated[i - 1]) { //is populated
				m_IsPopulated[i - 1] = false;
				return;
			}
		}
		throw std::runtime_error("No element to be popped!");
	}

	void popAt(const size_t index) {
		if (m_IsPopulated[index]) { //is populated
			m_IsPopulated[index] = false;
			return;
		}
		throw std::runtime_error("No element at index to be popped!");
	}

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TType>)
	void pop(const TType& obj) {
		for (size_t index = 0; index < getSize(); ++index) {
			if (m_Container[index] == obj) {
				m_IsPopulated[index] = false;
			}
		}
	}

#ifdef USING_SIMPLEPTR
	void pop(TPointerType obj) {
		if constexpr (sstl::is_managed_v<TType>) {
			for (size_t index = 0; index < getSize(); ++index) {
				// Will compare pointers, is always comparable
				if (m_Container[index].get() == obj) {
					m_IsPopulated[index] = false;
				}
			}
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

	friend struct SContainer;

	auto& getSubcontainer() { return m_Container; }
	const auto& getSubcontainer() const { return m_Container; }

	void arrayArgsInit(const TType& obj, size_t& index) noexcept {
		m_Container[index] = obj;
		m_IsPopulated[index] = true;
		index++;
	}

	void arrayArgsInit(TType&& obj, size_t& index) noexcept {
		m_Container[index] = std::move(obj);
		m_IsPopulated[index] = true;
		index++;
	}

	std::array<bool, TSize> m_IsPopulated;
	std::array<TType, TSize> m_Container;
};

template <typename TType, size_t TSize>
struct TContainerTraits<TArray<TType, TSize>> {
	using Type = TType;
	using ContainerType = std::array<TType, TSize>;
	using Iterator = typename ContainerType::iterator;
	using ReverseIterator = typename ContainerType::reverse_iterator;
	using ConstIterator = typename ContainerType::const_iterator;
	using ConstReverseIterator = typename ContainerType::const_reverse_iterator;
	constexpr static bool bIsContiguousMemory = true;
	constexpr static bool bIsLimitedAccess = false;
	constexpr static bool bIsForwardOnly = true;
	constexpr static bool bIsLimitedSize = true;
};