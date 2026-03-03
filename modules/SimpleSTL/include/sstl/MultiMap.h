#pragma once

#include <unordered_map>
#include "Container.h"
#include "sutil/InitializerList.h"

#ifdef USING_SIMPLEARCHIVE
#include "sarch/HashArchive.h"
#endif

template <typename TKeyType, typename TValueType>
struct TMultiMap : TAssociativeContainer<TKeyType, TValueType> {

	TMultiMap() = default;

	template <typename TOtherValueType = TValueType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherValueType>, int> = 0
	>
	TMultiMap(TInitializerList<TPair<TKeyType, TValueType>> init) {
		m_Container.reserve(init.size());
		for (auto& pair : init) {
			m_Container.emplace(pair.first, pair.second);
		}
	}

	template <typename... TPairs>
	explicit TMultiMap(TPairs&&... args) {
		m_Container.reserve(sizeof...(TPairs));
		(m_Container.emplace(std::forward<typename TPairs::KeyType>(args.first), std::forward<typename TPairs::ValueType>(args.second)), ...);
	}

	TMultiMap(const std::unordered_multimap<TKeyType, TValueType>& otr): m_Container(otr) {}

	[[nodiscard]] virtual size_t getSize() const override {
		return m_Container.size();
	}

	virtual TPair<TKeyType, const TValueType&> top() const override {
		return TPair<TKeyType, const TValueType&>{*m_Container.begin()};
	}

	virtual TPair<TKeyType, const TValueType&> bottom() const override {
		auto itr = m_Container.end();
		std::advance(itr, -1);
		return TPair<TKeyType, const TValueType&>{*itr};
	}

	virtual bool contains(const TKeyType& key) const override {
		return ASSOCIATIVE_CONTAINS(m_Container, key);
	}

	virtual TValueType& get(const TKeyType& key) override {
		return m_Container.find(key)->second;
	}

	virtual const TValueType& get(const TKeyType& key) const override {
		return m_Container.find(key)->second;
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

	virtual void transfer(TAssociativeContainer<TKeyType, TValueType>& otr, const TKeyType& key) override {
		auto itr = m_Container.extract(m_Container.find(key));
		// Prefer move, but copy if not available
		if constexpr (std::is_move_constructible_v<TValueType>) {
			otr.push(itr.key(), std::move(itr.mapped()));
		} else {
			otr.push(itr.key(), itr.mapped());
		}
	}

	virtual void forEach(const std::function<void(TPair<TKeyType, const TValueType&>)>& func) const override {
		for (auto itr = m_Container.begin(); itr != m_Container.end(); ++itr) {
			func(TPair<TKeyType, const TValueType&>{itr->first, itr->second});
		}
	}

protected:

	struct Hasher {
		size_t operator()(const TKeyType& p) const noexcept {
#ifdef USING_SIMPLEARCHIVE
			CHashArchive archive;
			archive << p;
			return archive.get();
#else
			return std::hash<TKeyType>()(p);
#endif
		}
	};

	std::unordered_multimap<TKeyType, TValueType, Hasher> m_Container;
};

template <typename TKeyType, typename TValueType>
TMultiMap(TInitializerList<TPair<TKeyType, TValueType>>) -> TMultiMap<TKeyType, TValueType>;

template <typename TPair, typename... TPairs>
TMultiMap(TPair, TPairs...) -> TMultiMap<typename TPair::KeyType, typename TPair::ValueType>;