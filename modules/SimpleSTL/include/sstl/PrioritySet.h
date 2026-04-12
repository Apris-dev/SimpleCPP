#pragma once

#include <set>
#include "Container.h"
#include "sutil/InitializerList.h"

template <typename TType,
          std::enable_if_t<sutil::is_less_than_comparable_v<TType>, int> = 0
>
struct TPrioritySet : TSingleAssociativeContainer<std::set<TType>> {

	using Super = TSingleAssociativeContainer<std::set<TType>>;

	TPrioritySet() = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	TPrioritySet(TInitializerList<TType> init): m_Container(init) {}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	explicit TPrioritySet(TArgs&&... args) {
		(m_Container.emplace(std::forward<TArgs>(args)), ...);
	}

	TPrioritySet(const std::set<TType>& otr): m_Container(otr) {}

	[[nodiscard]] virtual size_t getSize() const override {
		return m_Container.size();
	}

	[[nodiscard]] virtual bool isEmpty() const override {
		return m_Container.empty();
	}

	[[nodiscard]] virtual const TType& top() const override {
		return *m_Container.begin();
	}

	[[nodiscard]] virtual const TType& bottom() const override {
		auto itr = m_Container.end();
		--itr;
		return *itr;
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

	virtual bool contains(const TType& obj) const override {
		return ASSOCIATIVE_CONTAINS(m_Container, obj);
	}

#ifdef USING_SIMPLEPTR
	virtual bool contains(typename TUnfurled<TType>::Type* obj) const override {
		if constexpr (sstl::is_managed_v<TType>) {
			return CONTAINS(m_Container, obj, TUnfurled<TType>::get);
		} else {
			return contains(*obj);
		}
	}
#endif

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

#ifdef USING_SIMPLEPTR
	virtual void pop(typename TUnfurled<TType>::Type* obj) override {
		if constexpr (sstl::is_managed_v<TType>) {
			ERASE(m_Container, obj, TUnfurled<TType>::get);
		} else {
			pop(*obj);
		}
	}
#endif

	virtual typename std::set<TType>::node_type extract(TType& obj) override {
		return m_Container.extract(m_Container.find(obj));
	}

#ifdef USING_SIMPLEPTR
	virtual typename std::set<TType>::node_type extract(typename TUnfurled<TType>::Type* obj) override {
		if constexpr (sstl::is_managed_v<TType>) {
			return m_Container.extract(FIND(m_Container, obj, TUnfurled<TType>::get));
		} else {
			return extract(*obj);
		}
	}
#endif

protected:

	std::set<TType> m_Container;
};

template <typename TType, typename... TArgs>
TPrioritySet(TType, TArgs...) -> TPrioritySet<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
