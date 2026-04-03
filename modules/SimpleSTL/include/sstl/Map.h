#pragma once

#include <unordered_map>
#include "Container.h"
#include "sutil/InitializerList.h"

#ifdef USING_SIMPLEARCHIVE
#include "sarch/HashArchive.h"
#endif

template <typename TKeyType, typename TValueType>
struct TMap : TAssociativeContainer<std::unordered_map<TKeyType, TValueType, TContainerHasher<TKeyType>>> {

	using Super = TAssociativeContainer<std::unordered_map<TKeyType, TValueType, TContainerHasher<TKeyType>>>;

	TMap() = default;

	template <typename TOtherValueType = TValueType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherValueType>, int> = 0
	>
	TMap(TInitializerList<TPair<TKeyType, TValueType>> init) {
		m_Container.reserve(init.size());
		for (auto& pair : init) {
			m_Container.emplace(pair.first, pair.second);
		}
	}

	template <typename... TPairs>
	explicit TMap(TPairs&&... args) {
		m_Container.reserve(sizeof...(TPairs));
		(m_Container.emplace(std::forward<typename TPairs::KeyType>(args.first), std::forward<typename TPairs::ValueType>(args.second)), ...);
	}

	TMap(const std::unordered_map<TKeyType, TValueType>& otr): m_Container(otr) {}

	[[nodiscard]] virtual size_t getSize() const override {
		return m_Container.size();
	}

	[[nodiscard]] virtual TPair<TKeyType, const TValueType&> top() const override {
		return TPair<TKeyType, const TValueType&>{*m_Container.begin()};
	}

	[[nodiscard]] virtual TPair<TKeyType, const TValueType&> bottom() const override {
		auto itr = m_Container.end();
		std::advance(itr, -1);
		return TPair<TKeyType, const TValueType&>{*itr};
	}

	[[nodiscard]] virtual typename Super::Iterator begin() noexcept override {
		return m_Container.begin();
	}

	[[nodiscard]] virtual typename Super::ConstIterator begin() const noexcept override {
		return m_Container.begin();
	}

	[[nodiscard]] virtual typename Super::Iterator end() noexcept override {
		return m_Container.end();
	}

	[[nodiscard]] virtual typename Super::ConstIterator end() const noexcept override {
		return m_Container.end();
	}

	virtual bool contains(const TKeyType& key) const override {
		return ASSOCIATIVE_CONTAINS(m_Container, key);
	}

	virtual TValueType& get(const TKeyType& key) override {
		return m_Container.at(key);
	}

	virtual const TValueType& get(const TKeyType& key) const override {
		return m_Container.at(key);
	}

	virtual void resize(const size_t amt, std::function<TPair<TKeyType, TValueType>()> func) override {
		m_Container.reserve(amt);
		for (size_t i = getSize(); i < amt; ++i) {
			TPair<TKeyType, TValueType> pair = func();
			m_Container.emplace(std::forward<TKeyType>(pair.first), std::forward<TValueType>(pair.second));
		}
	}

	virtual void reserve(size_t amt) override {
		m_Container.reserve(amt);
	}

	virtual TPair<TKeyType, const TValueType&> push() override {
		if constexpr (std::is_default_constructible_v<TKeyType> && std::is_default_constructible_v<TValueType>) {
			m_Container.emplace();
			return top();
		} else {
			throw std::runtime_error("Type is not default constructible!");
		}
	}

	virtual TValueType& push(const TKeyType& key) override {
		if constexpr (std::is_default_constructible_v<TValueType>) {
			push(TPair<TKeyType, TValueType>{key, {}});
			return get(key);
		} else {
			throw std::runtime_error("Type is not default constructible!");
		}
	}

	virtual TValueType& push(const TKeyType& key, const TValueType& value) override {
		if constexpr (std::is_copy_constructible_v<TValueType>) {
			push(TPair<TKeyType, TValueType>{key, value});
			return get(key);
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual TValueType& push(const TKeyType& key, TValueType&& value) override {
		if constexpr (std::is_move_constructible_v<TValueType>) {
			push(TPair<TKeyType, TValueType>{key, std::move(value)});
			return get(key);
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	virtual void push(const TPair<TKeyType, TValueType>& pair) override {
		if constexpr (std::is_copy_constructible_v<TValueType>) {
			m_Container.emplace(pair.first, pair.second);
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual void push(TPair<TKeyType, TValueType>&& pair) override {
		if constexpr (std::is_move_constructible_v<TValueType>) {
			m_Container.emplace(std::move(pair.first), std::move(pair.second));
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	virtual void replace(const TKeyType& key, const TValueType& obj) override {
		if constexpr (std::is_copy_constructible_v<TValueType>) {
			pop(key);
			push(TPair<TKeyType, TValueType>{key, obj});
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual void replace(const TKeyType& key, TValueType&& obj) override {
		if constexpr (std::is_move_constructible_v<TValueType>) {
			pop(key);
			push(TPair<TKeyType, TValueType>{key, std::move(obj)});
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	virtual void clear() override {
		m_Container.clear();
	}

	virtual void pop() override {
		pop(m_Container.begin()->first);
	}

	virtual void pop(const TKeyType& key) override {
		m_Container.erase(key);
	}

	virtual typename std::unordered_map<TKeyType, TValueType, TContainerHasher<TKeyType>>::node_type extract(const TKeyType& key) override {
		return m_Container.extract(m_Container.find(key));
	}

	virtual void forEach(const std::function<void(TPair<TKeyType, const TValueType&>)>& func) const override {
		for (auto itr = m_Container.begin(); itr != m_Container.end(); ++itr) {
			func(TPair<TKeyType, const TValueType&>{itr->first, itr->second});
		}
	}

protected:

	std::unordered_map<TKeyType, TValueType, TContainerHasher<TKeyType>> m_Container;
};

template <typename TKeyType, typename TValueType>
TMap(TInitializerList<TPair<TKeyType, TValueType>>) -> TMap<TKeyType, TValueType>;

template <typename TPair, typename... TPairs>
TMap(TPair, TPairs...) -> TMap<typename TPair::KeyType, typename TPair::ValueType>;