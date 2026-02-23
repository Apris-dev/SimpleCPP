#pragma once

#include <unordered_set>
#include "Container.h"
#include "sutil/InitializerList.h"
#include "../../../SimpleArchive/include/sarch/HashArchive.h"

template <typename TType,
          std::enable_if_t<sutil::is_hashable_v<TType>, int> = 0
>
struct TSet : TSingleAssociativeContainer<TType> {

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

	[[nodiscard]] virtual size_t getSize() const override {
		return m_Container.size();
	}

	virtual const TType& top() const override {
		return *m_Container.begin();
	}

	virtual const TType& bottom() const override {
		auto itr = m_Container.end();
		--itr;
		return *itr;
	}

	virtual bool contains(const TType& obj) const override {
		return ASSOCIATIVE_CONTAINS(m_Container, obj);
	}

	virtual bool contains(typename TUnfurled<TType>::Type* obj) const override {
		if constexpr (sstl::is_managed_v<TType>) {
			return CONTAINS(m_Container, obj, TUnfurled<TType>::get);
		} else {
			return contains(*obj);
		}
	}

	virtual void resize(const size_t amt) override {
		if constexpr (std::is_default_constructible_v<TType>) {
			for (size_t i = getSize(); i < amt; ++i) {
				m_Container.emplace();
			}
		} else {
			throw std::runtime_error("Type is not default constructible!");
		}
	}

	virtual void resize(const size_t amt, std::function<TType()> func) override {
		for (size_t i = getSize(); i < amt; ++i) {
			m_Container.emplace(std::forward<TType>(func()));
		}
	}

	virtual void reserve(const size_t amt) override {
		m_Container.reserve(amt);
	}

	virtual const TType& push() override {
		if constexpr (std::is_default_constructible_v<TType>) {
			m_Container.emplace();
			return top();
		} else {
			throw std::runtime_error("Type is not default constructible!");
		}
	}

	virtual void push(const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			m_Container.emplace(obj);
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual void push(TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {
			m_Container.emplace(std::move(obj));
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	virtual void replace(const TType& tgt, const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			// Since this container is unordered, replacing doesn't need to set at the same index
			pop(tgt);
			m_Container.insert(obj);
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual void replace(const TType& tgt, TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {// Since this container is unordered, replacing doesn't need to set at the same index
			pop(tgt);
			m_Container.insert(std::move(obj));
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	virtual void clear() override {
		m_Container.clear();
	}

	virtual void pop() override {
		m_Container.erase(m_Container.begin());
	}

	virtual void pop(const TType& obj) override {
		m_Container.erase(obj);
	}

	virtual void pop(typename TUnfurled<TType>::Type* obj) override {
		if constexpr (sstl::is_managed_v<TType>) {
			ERASE(m_Container, obj, TUnfurled<TType>::get);
		} else {
			pop(*obj);
		}
	}

	virtual void transfer(TSingleAssociativeContainer<TType>& otr, TType& obj) override {
		if (!this->contains(obj)) return;
		auto itr = m_Container.extract(m_Container.find(obj));
		// Prefer move, but copy if not available
		if constexpr (std::is_move_constructible_v<TType>) {
			otr.push(std::move(itr.value()));
		} else {
			otr.push(itr.value());
		}
	}

	virtual void transfer(TSingleAssociativeContainer<TType>& otr, typename TUnfurled<TType>::Type* obj) override {
		if constexpr (sstl::is_managed_v<TType>) {
			if (!this->contains(obj)) return;
			auto itr = m_Container.extract(FIND(m_Container, obj, TUnfurled<TType>::get));
			// Prefer move, but copy if not available
			if constexpr (std::is_move_constructible_v<TType>) {
				otr.push(std::move(itr.value()));
			} else {
				otr.push(itr.value());
			}
		} else {
			transfer(otr, *obj);
		}
	}

	virtual void forEach(const std::function<void(const TType&)>& func) const override {
		for (auto itr = m_Container.begin(); itr != m_Container.end(); ++itr) {
			func(*itr);
		}
	}

protected:

	struct Hasher {
		size_t operator()(const TType& p) const noexcept {
			return getHash(p);
		}
	};

	std::unordered_set<TType, Hasher> m_Container;
};

template <typename TType, typename... TArgs>
TSet(TType, TArgs...) -> TSet<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
