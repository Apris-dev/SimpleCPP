#pragma once

#include <unordered_set>
#include "Container.h"
#include "sutil/InitializerList.h"

template <typename TType>
struct TSet : TSingleAssociativeContainer<TSet<TType>> {

	using Super = TSingleAssociativeContainer<TSet>;

#ifdef USING_SIMPLEPTR
	using typename Super::TUnfurledType;
#endif

	TSet() = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	TSet(TInitializerList<TType> init): m_Container(init) {}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	explicit TSet(TArgs&&... args) {
		m_Container.reserve(sizeof...(TArgs));
		(m_Container.emplace(std::forward<TArgs>(args)), ...);
	}

	TSet(const std::unordered_set<TType>& otr): m_Container(otr) {}

	[[nodiscard]] size_t getSize() const {
		return m_Container.size();
	}

	[[nodiscard]] bool isEmpty() const {
		return m_Container.empty();
	}

	[[nodiscard]] const TType& top() const {
		return *m_Container.begin();
	}

	[[nodiscard]] const TType& bottom() const {
		auto itr = m_Container.end();
		std::advance(itr, -1);
		return *itr;
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
		return ASSOCIATIVE_CONTAINS(m_Container, obj);
	}

#ifdef USING_SIMPLEPTR
	bool contains(const TFrail<TUnfurledType>& obj) const {
		return CONTAINS(m_Container, obj);
	}
#endif

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	void resize(const size_t amt) {
		for (size_t i = getSize(); i < amt; ++i) {
			m_Container.emplace();
		}
	}

	void resize(const size_t amt, std::function<TType()> func) {
		for (size_t i = getSize(); i < amt; ++i) {
			m_Container.emplace(std::forward<TType>(func()));
		}
	}

	void reserve(const size_t amt) {
		m_Container.reserve(amt);
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TType>)
	const TType& push() {
		m_Container.emplace();
		return top();
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void push(const TType& obj) {
		m_Container.emplace(obj);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void push(TType&& obj) {
		m_Container.emplace(std::move(obj));
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TType>)
	void replace(const TType& tgt, const TType& obj) {
		// Since this container is unordered, replacing doesn't need to set at the same index
		pop(tgt);
		m_Container.insert(obj);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TType>)
	void replace(const TType& tgt, TType&& obj) {
		pop(tgt);
		m_Container.insert(std::move(obj));
	}

	void clear() {
		m_Container.clear();
	}

	void pop() {
		m_Container.erase(m_Container.begin());
	}

	void pop(const TType& obj) {
		m_Container.erase(obj);
	}

#ifdef USING_SIMPLEPTR
	void pop(const TFrail<TUnfurledType>& obj) {
		ERASE(m_Container, obj);
	}
#endif

	// Moves an object from this to container otr
	template <typename TOtherContainerType>
	void transfer(TSingleAssociativeContainer<TOtherContainerType>& otr, TType& obj) {
		if (!this->contains(obj)) return;
		auto itr = m_Container.extract(m_Container.find(obj));
		// Prefer move, but copy if not available
		if constexpr (std::is_move_constructible_v<TType>) {
			otr.push(std::move(itr.value()));
		} else {
			otr.push(itr.value());
		}
	}

#ifdef USING_SIMPLEPTR
	// Version of transfer that guarantees raw pointer input
	template <typename TOtherContainerType>
	void transfer(TSingleAssociativeContainer<TOtherContainerType>& otr, const TFrail<TUnfurledType>& obj) {
		if (!this->contains(obj)) return;
		auto itr = m_Container.extract(FIND(m_Container, obj));
		// Prefer move, but copy if not available
		if constexpr (std::is_move_constructible_v<TType>) {
			otr.push(std::move(itr.value()));
		} else {
			otr.push(itr.value());
		}
	}
#endif

	template <typename TOtherContainerType>
	void append(const TSingleAssociativeContainer<TOtherContainerType>& otr) {
#if CXX_VERSION >= 23
		m_Container.insert_range(SContainer::getSubcontainer(otr));
#else
		m_Container.insert(SContainer::getSubcontainer(otr).begin(), SContainer::getSubcontainer(otr).end());
#endif
	}

protected:

	friend struct SContainer;

	auto& getSubcontainer() { return m_Container; }
	const auto& getSubcontainer() const { return m_Container; }

	std::unordered_set<TType, TContainerHasher<TType>> m_Container;
};

template <typename TType>
struct TContainerTraits<TSet<TType>> {
	using Type = TType;
	using ContainerType = std::unordered_set<TType, TContainerHasher<TType>>;
	using Iterator = typename ContainerType::iterator;
	using ConstIterator = typename ContainerType::const_iterator;
	constexpr static bool bHasHashing = true;
};

template <typename TType, typename... TArgs>
TSet(TType, TArgs...) -> TSet<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
