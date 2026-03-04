#pragma once

#include <deque>
#include "Container.h"
#include "sutil/InitializerList.h"

template <typename TType>
struct TDeque : TSequenceContainer<TType> {

	TDeque() = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	TDeque(TInitializerList<TType> init): m_Container(init) {}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	explicit TDeque(TArgs&&... args) {
		(m_Container.emplace_back(std::forward<TArgs>(args)), ...);
	}

	TDeque(const std::deque<TType>& otr): m_Container(otr) {}

	[[nodiscard]] virtual size_t getSize() const override {
		return m_Container.size();
	}

	virtual TType& top() override {
		return m_Container.front();
	}

	virtual const TType& top() const override {
		return m_Container.front();
	}

	virtual TType& bottom() override {
		return m_Container.back();
	}

	virtual const TType& bottom() const override {
		return m_Container.back();
	}

	virtual bool contains(const TType& obj) const override {
		if constexpr (sutil::is_equality_comparable_v<TType>) {
			return CONTAINS(m_Container, obj);
		} else {
			throw std::runtime_error("Type is not comparable!");
		}
	}

#ifdef USING_SIMPLEPTR
	virtual bool contains(typename TUnfurled<TType>::Type* obj) const override {
		if constexpr (sstl::is_managed_v<TType>) {
			// Will compare pointers, is always comparable
			return CONTAINS(m_Container, obj, TUnfurled<TType>::get);
		} else {
			return contains(*obj);
		}
	}
#endif

	virtual size_t find(const TType& obj) const override {
		if constexpr (sutil::is_equality_comparable_v<TType>) {
			return DISTANCE(m_Container, obj);
		} else {
			throw std::runtime_error("Type is not comparable!");
		}
	}

#ifdef USING_SIMPLEPTR
	virtual size_t find(typename TUnfurled<TType>::Type* obj) const override {
		if constexpr (sstl::is_managed_v<TType>) {
			// Will compare pointers, is always comparable
			return DISTANCE(m_Container, obj, TUnfurled<TType>::get);
		} else {
			return find(*obj);
		}
	}
#endif

	virtual TType& get(size_t index) override {
		return m_Container[index];
	}

	virtual const TType& get(size_t index) const override {
		return m_Container[index];
	}

	virtual void resize(size_t amt) override {
		if constexpr (std::is_default_constructible_v<TType>) {
			m_Container.resize(amt);
		} else {
			throw std::runtime_error("Type is not default constructible!");
		}
	}

	virtual void resize(const size_t amt, std::function<TType(size_t)> func) override {
		const size_t previousSize = getSize();
		for (size_t i = previousSize; i < amt; ++i) {
			m_Container.emplace_back(std::forward<TType>(func(i)));
		}
	}

	virtual TType& push() override {
		if constexpr (std::is_default_constructible_v<TType>) {
			m_Container.emplace_back();
			return get(getSize() - 1);
		} else {
			throw std::runtime_error("Type is not default constructible!");
		}
	}

	virtual size_t push(const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			m_Container.emplace_back(obj);
			return getSize() - 1;
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual size_t push(TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {
			m_Container.emplace_back(std::move(obj));
			return getSize() - 1;
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	virtual void push(const size_t index, const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			m_Container.insert(m_Container.begin() + index, obj);
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual void push(const size_t index, TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {
			m_Container.insert(m_Container.begin() + index, std::move(obj));
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	virtual void replace(const size_t index, const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			popAt(index);
			push(index, obj);
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual void replace(const size_t index, TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {
			popAt(index);
			push(index, std::move(obj));
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	virtual void clear() override {
		m_Container.clear();
	}

	virtual void pop() override {
		popAt(static_cast<size_t>(0));
	}

	virtual void popAt(const size_t index) override {
		m_Container.erase(m_Container.begin() + index);
	}

	virtual void pop(const TType& obj) override {
		if constexpr (sutil::is_equality_comparable_v<TType>) {
			ERASE(m_Container, obj);
		} else {
			throw std::runtime_error("Type is not comparable!");
		}
	}

#ifdef USING_SIMPLEPTR
	virtual void pop(typename TUnfurled<TType>::Type* obj) override {
		if constexpr (sstl::is_managed_v<TType>) {
			// Will compare pointers, is always comparable
			ERASE(m_Container, obj, TUnfurled<TType>::get);
		} else {
			pop(*obj);
		}
	}
#endif

	virtual void forEach(const std::function<void(size_t, TType&)>& func) override {
		size_t i = 0;
		for (auto itr = m_Container.begin(); itr != m_Container.end(); ++itr, ++i) {
			func(i, *itr);
		}
	}

	virtual void forEach(const std::function<void(size_t, const TType&)>& func) const override {
		size_t i = 0;
		for (auto itr = m_Container.begin(); itr != m_Container.end(); ++itr, ++i) {
			func(i, *itr);
		}
	}

	virtual void forEachReverse(const std::function<void(size_t, TType&)>& func) override {
		size_t i = getSize() - 1;
		for (auto itr = m_Container.rbegin(); itr != m_Container.rend(); ++itr, --i) {
			func(i, *itr);
		}
	}

	virtual void forEachReverse(const std::function<void(size_t, const TType&)>& func) const override {
		size_t i = getSize() - 1;
		for (auto itr = m_Container.rbegin(); itr != m_Container.rend(); ++itr, --i) {
			func(i, *itr);
		}
	}

protected:

	std::deque<TType> m_Container;
};

template <typename TType, typename... TArgs>
TDeque(TType, TArgs...) -> TDeque<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
