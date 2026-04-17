#pragma once

#include <map>
#include "Container.h"
#include "sutil/InitializerList.h"

template <typename TKeyType, typename TValueType,
          std::enable_if_t<sutil::is_less_than_comparable_v<TKeyType>, int> = 0
>
struct TPriorityMap : TAssociativeContainer<TPriorityMap<TKeyType, TValueType>> {

	using Super = TAssociativeContainer<TPriorityMap>;

#ifdef USING_SIMPLEPTR
	using typename Super::TUnfurledValueType;
#endif

	TPriorityMap() = default;

	template <typename TOtherValueType = TValueType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherValueType>, int> = 0
	>
	TPriorityMap(TInitializerList<TPair<TKeyType, TValueType>> init) {
		for (auto& pair : init) {
			m_Container.emplace(pair.first, pair.second);
		}
	}

	template <typename... TPairs>
	explicit TPriorityMap(TPairs&&... args) {
		(m_Container.emplace(std::forward<typename TPairs::KeyType>(args.first), std::forward<typename TPairs::ValueType>(args.second)), ...);
	}

	TPriorityMap(const std::map<TKeyType, TValueType>& otr): m_Container(otr) {}

	[[nodiscard]] size_t getSize() const {
		return m_Container.size();
	}

	[[nodiscard]] bool isEmpty() const {
		return m_Container.empty();
	}

	[[nodiscard]] TPair<TKeyType, const TValueType&> top() const {
		return TPair<TKeyType, const TValueType&>{*m_Container.begin()};
	}

	[[nodiscard]] TPair<TKeyType, const TValueType&> bottom() const {
		auto itr = m_Container.end();
		--itr;
		return TPair<TKeyType, const TValueType&>{*itr};
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

	bool isValid(const TKeyType& key) const {
		return ASSOCIATIVE_CONTAINS(m_Container, key);
	}

	ENABLE_FUNC_IF(sutil::is_equality_comparable_v<TValueType>)
	bool contains(const TValueType& obj) const {
		return std::find_if(m_Container.cbegin(), m_Container.cend(), [&obj](const std::pair<TKeyType, const TValueType&>& pair) {
			return pair.second == obj;
		}) != m_Container.cend();
	}

#ifdef USING_SIMPLEPTR
	bool contains(const TFrail<TUnfurledValueType>& obj) const {
		// Will compare pointers, is always comparable
		return std::find_if(m_Container.cbegin(), m_Container.cend(), [&obj](const std::pair<TKeyType, const TValueType&>& pair) {
			return pair.second == obj;
		}) != m_Container.cend();
	}
#endif

	[[nodiscard]] TKeyType find(const TValueType& obj) const {
		return std::find_if(m_Container.cbegin(), m_Container.cend(), [&obj](const std::pair<TKeyType, const TValueType&>& pair) {
			return pair.second == obj;
		})->first;
	}

#ifdef USING_SIMPLEPTR
	[[nodiscard]] TKeyType find(const TFrail<TUnfurledValueType>& obj) const {
		// Will compare pointers, is always comparable
		return std::find_if(m_Container.cbegin(), m_Container.cend(), [&obj](const std::pair<TKeyType, const TValueType&>& pair) {
			return pair.second == obj;
		})->first;
	}
#endif

	TValueType& get(const TKeyType& key) {
		return m_Container.at(key);
	}

	const TValueType& get(const TKeyType& key) const {
		return m_Container.at(key);
	}

	void resize(const size_t amt, std::function<TPair<TKeyType, TValueType>()> func) {
		for (size_t i = getSize(); i < amt; ++i) {
			TPair<TKeyType, TValueType> pair = func();
			m_Container.emplace(std::forward<TKeyType>(pair.first), std::forward<TValueType>(pair.second));
		}
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TKeyType> && std::is_default_constructible_v<TValueType>)
	TPair<TKeyType, const TValueType&> push() {
		m_Container.emplace();
		return top();
	}

	ENABLE_FUNC_IF(std::is_default_constructible_v<TValueType>)
	TValueType& push(const TKeyType& key) {
		push(TPair<TKeyType, TValueType>{key, {}});
		return get(key);
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TValueType>)
	TValueType& push(const TKeyType& key, const TValueType& value) {
		push(TPair<TKeyType, TValueType>{key, value});
		return get(key);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TValueType>)
	TValueType& push(const TKeyType& key, TValueType&& value) {
		push(TPair<TKeyType, TValueType>{key, std::move(value)});
		return get(key);
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TValueType>)
	void push(const TPair<TKeyType, TValueType>& pair) {
		m_Container.emplace(pair.first, pair.second);
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TValueType>)
	void push(TPair<TKeyType, TValueType>&& pair) {
		m_Container.emplace(std::move(pair.first), std::move(pair.second));
	}

	ENABLE_FUNC_IF(std::is_copy_constructible_v<TValueType>)
	void replace(const TKeyType& key, const TValueType& obj) {
		pop(key);
		push(TPair<TKeyType, TValueType>{key, obj});
	}

	ENABLE_FUNC_IF(std::is_move_constructible_v<TValueType>)
	void replace(const TKeyType& key, TValueType&& obj) {
		pop(key);
		push(TPair<TKeyType, TValueType>{key, std::move(obj)});
	}

	void clear() {
		m_Container.clear();
	}

	void pop() {
		pop(m_Container.begin()->first);
	}

	void pop(const TKeyType& key) {
		m_Container.erase(key);
	}

	template <typename TOtherContainerType>
	void transfer(TAssociativeContainer<TOtherContainerType>& otr, const TKeyType& key) {
		auto itr = m_Container.extract(m_Container.find(key));
		// Prefer move, but copy if not available
		if constexpr (std::is_move_constructible_v<TValueType>) {
			otr.push(itr.key(), std::move(itr.mapped()));
		} else {
			otr.push(itr.key(), itr.mapped());
		}
	}

protected:

	std::map<TKeyType, TValueType> m_Container;
};

template <typename TKeyType, typename TValueType>
struct TContainerTraits<TPriorityMap<TKeyType, TValueType>> {
	using KeyType = TKeyType;
	using ValueType = TValueType;
	using ContainerType = std::map<TKeyType, TValueType>;
	using Iterator = typename ContainerType::iterator;
	using ConstIterator = typename ContainerType::const_iterator;
	constexpr static bool bHasHashing = false;
};

template <typename TKeyType, typename TValueType>
TPriorityMap(TInitializerList<TPair<TKeyType, TValueType>>) -> TPriorityMap<TKeyType, TValueType>;

template <typename TPair, typename... TPairs>
TPriorityMap(TPair, TPairs...) -> TPriorityMap<typename TPair::KeyType, typename TPair::ValueType>;