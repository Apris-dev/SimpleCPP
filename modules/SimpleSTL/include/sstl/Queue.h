#pragma once

#include "Deque.h"

// std::queue is already a wrapper around a deque, so we do that here too, and skip std::queue's own abstraction
template <typename TType>
struct TQueue : TDeque<TType> {

	using TDeque<TType>::m_Container;
	
	TQueue() = default;

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_copy_constructible_v<TOtherType>, int> = 0
	>
	TQueue(TInitializerList<TType> init): TDeque<TType>(init) {}

	template <typename... TArgs,
		std::enable_if_t<std::conjunction_v<std::is_constructible<TType, TArgs>...>, int> = 0
	>
	explicit TQueue(TArgs&&... args): TDeque<TType>(std::forward<TArgs>(args)...) {}

	TQueue(const std::deque<TType>& otr): TDeque<TType>::m_Container(otr) {}

	virtual TType& top() override {
		return m_Container.back();
	}

	virtual const TType& top() const override {
		return m_Container.back();
	}

	virtual size_t push(const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			m_Container.emplace_back(obj);
			return TDeque<TType>::getSize() - 1;
		} else {
			throw std::runtime_error("Type is not copyable");
		}
	}

	virtual size_t push(TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {
			m_Container.emplace_back(std::move(obj));
			return TDeque<TType>::getSize() - 1;
		} else {
			throw std::runtime_error("Type is not moveable");
		}
	}

	virtual void replace(const size_t index, const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			TDeque<TType>::popAt(index);
			TDeque<TType>::push(index, obj);
		} else {
			throw std::runtime_error("Type is not copyable");
		}
	}

	virtual void replace(const size_t index, TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {
			TDeque<TType>::popAt(index);
			TDeque<TType>::push(index, std::move(obj));
		} else {
			throw std::runtime_error("Type is not moveable");
		}
	}

	virtual void pop() override {
		TDeque<TType>::popAt(TDeque<TType>::getSize() - 1);
	}

protected:

	virtual TType& bottom() override {
		return TDeque<TType>::bottom();
	}

	virtual const TType& bottom() const override {
		return TDeque<TType>::bottom();
	}

	virtual TType& get(size_t index) override {
		return TDeque<TType>::get(index);
	}

	virtual const TType& get(size_t index) const override {
		return TDeque<TType>::get(index);
	}

	virtual void push(const size_t index, const TType& obj) override {
		TDeque<TType>::push(index, obj);
	}

	virtual void push(const size_t index, TType&& obj) override {
		TDeque<TType>::push(index, std::move(obj));
	}

	virtual void popAt(const size_t index) override {
		TDeque<TType>::popAt(index);
	}
};

template <typename TType, typename... TArgs>
TQueue(TType, TArgs...) -> TQueue<typename sstl::EnforceConvertible<TType, TArgs...>::Type>;
