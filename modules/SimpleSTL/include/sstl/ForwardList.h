#pragma once

#include <forward_list>
#include "Container.h"
#include "sutil/InitializerList.h"

template <typename TType>
struct TForwardList : TSequenceContainer<std::forward_list<TType>> {

	using Super = TSequenceContainer<std::forward_list<TType>>;

	TForwardList() = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	TForwardList(TInitializerList<TType> init): m_Container(init) {}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	explicit TForwardList(TArgs&&... args) {
		(m_Container.emplace_front(std::forward<TArgs>(args)), ...);
	}

	TForwardList(const std::forward_list<TType>& otr): m_Container(otr) {}

	[[nodiscard]] virtual size_t getSize() const override {
		return SIZE(m_Container);
	}

	[[nodiscard]] virtual TType& top() override {
		return m_Container.front();
	}

	[[nodiscard]] virtual const TType& top() const override {
		return m_Container.front();
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
		auto itr = m_Container.begin();
		std::advance(itr, index);
		return *itr;
	}

	virtual const TType& get(size_t index) const override {
		auto itr = m_Container.begin();
		std::advance(itr, index);
		return *itr;
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
			m_Container.emplace_front(std::forward<TType>(func(i)));
		}
	}

	virtual TType& push() override {
		if constexpr (std::is_default_constructible_v<TType>) {
			m_Container.emplace_front();
			return get(getSize() - 1);
		} else {
			throw std::runtime_error("Type is not default constructible!");
		}
	}

	virtual size_t push(const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			m_Container.emplace_front(obj);
			return getSize() - 1;
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual size_t push(TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {
			m_Container.emplace_front(std::move(obj));
			return getSize() - 1;
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	virtual void push(const size_t index, const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			auto itr = m_Container.before_begin();
			std::advance(itr, index);
			m_Container.insert_after(itr, obj);
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual void push(const size_t index, TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {
			auto itr = m_Container.before_begin();
			std::advance(itr, index);
			m_Container.insert_after(itr, std::move(obj));
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
		m_Container.erase_after(m_Container.before_begin());
	}

	virtual void popAt(const size_t index) override {
		auto itr = m_Container.before_begin();
		std::advance(itr, index);
		m_Container.erase_after(itr);
	}

	virtual void pop(const TType& obj) override {
		if constexpr (sutil::is_equality_comparable_v<TType>) {
			m_Container.erase_after(std::remove(m_Container.before_begin(), m_Container.end(), obj), m_Container.end());
		} else {
			throw std::runtime_error("Type is not comparable!");
		}
	}

#ifdef USING_SIMPLEPTR
	virtual void pop(typename TUnfurled<TType>::Type* obj) override {
		if constexpr (sstl::is_managed_v<TType>) {
			// Will compare pointers, is always comparable
			m_Container.erase_after(std::remove(m_Container.before_begin(), m_Container.end(), obj), m_Container.end());
		} else {
			pop(*obj);
		}
	}
#endif

	using Super::transfer;

	virtual void transfer(Super& otr, const size_t index) override {
		// Forward List transfer can use splicing
		auto list = dynamic_cast<TForwardList*>(&otr);
		auto itr = m_Container.before_begin();
		std::advance(itr, index);
		list->m_Container.splice_after(list->m_Container.before_begin(), m_Container, itr);
	}

protected:

	std::forward_list<TType> m_Container;
};

template <typename TType, typename... TArgs>
TForwardList(TType, TArgs...) -> TForwardList<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
