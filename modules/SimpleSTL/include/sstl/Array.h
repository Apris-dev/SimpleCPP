#pragma once

#include <array>
#include "Container.h"
#include "sutil/InitializerList.h"

template <typename TType, size_t TSize>
struct TArray : TSequenceContainer<std::array<TType, TSize>> {

	using Super = TSequenceContainer<std::array<TType, TSize>>;

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

	[[nodiscard]] virtual size_t getSize() const override {
		return m_Container.size();
	}

	[[nodiscard]] TType* data() { return m_Container.data(); }

	[[nodiscard]] const TType* data() const { return m_Container.data(); }

	[[nodiscard]] virtual TType& top() override {
		return m_Container.front();
	}

	[[nodiscard]] virtual const TType& top() const override {
		return m_Container.front();
	}

	[[nodiscard]] virtual TType& bottom() override {
		return m_Container.back();
	}

	[[nodiscard]] virtual const TType& bottom() const override {
		return m_Container.back();
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

	[[nodiscard]] virtual bool containsAt(size_t index) const override {
		return m_IsPopulated[index];
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
			for (size_t i = 0; i < amt; ++i) {
				if (!m_IsPopulated[i]) {
					m_Container[i] = {};
					m_IsPopulated[i] = true;
				}
			}
		} else {
			throw std::runtime_error("Type is not default constructible!");
		}
	}

	virtual void resize(const size_t amt, std::function<TType(size_t)> func) override {
		for (size_t i = 0; i < amt; ++i) {
			if (!m_IsPopulated[i]) {
				get(i) = std::forward<TType>(func(i));
				m_IsPopulated[i] = true;
			}
		}
	}

	virtual void fill() {
		resize(TSize);
	}

	virtual void resize(std::function<TType(size_t)> func) {
		resize(TSize, func);
	}

	virtual TType& push() override {
		if constexpr (std::is_default_constructible_v<TType>) {
			return get(push(TType{}));
		} else {
			throw std::runtime_error("Type is not default constructible!");
		}
	}

	virtual size_t push(const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			for (size_t i = 0; i < getSize(); ++i) {
				if (!m_IsPopulated[i]) { //is not populated
					m_IsPopulated[i] = true;
					m_Container[i] = obj;
					return i;
				}
			}
			throw std::runtime_error("Array is full, cannot add any more elements.");
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual size_t push(TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {
			for (size_t i = 0; i < getSize(); ++i) {
				if (!m_IsPopulated[i]) { //is not populated
					m_IsPopulated[i] = true;
					m_Container[i] = std::move(obj);
					return i;
				}
			}
			throw std::runtime_error("Array is full, cannot add any more elements.");
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	virtual void push(const size_t index, const TType& obj) override {
		replace(index, obj);
	}

	virtual void push(const size_t index, TType&& obj) override {
		replace(index, std::move(obj));
	}

	virtual void replace(const size_t index, const TType& obj) override {
		if constexpr (std::is_copy_constructible_v<TType>) {
			m_Container[index] = obj;
		} else {
			throw std::runtime_error("Type is not copyable!");
		}
	}

	virtual void replace(const size_t index, TType&& obj) override {
		if constexpr (std::is_move_constructible_v<TType>) {
			m_Container[index] = std::move(obj);
		} else {
			throw std::runtime_error("Type is not moveable!");
		}
	}

	// Array is on stack, it is destroyed when out of scope
	virtual void clear() override {
		m_IsPopulated.fill(false);
	}

	virtual void pop() override {
		for (size_t i = getSize(); i > 0; --i) {
			if (m_IsPopulated[i - 1]) { //is populated
				m_IsPopulated[i - 1] = false;
				return;
			}
		}
		throw std::runtime_error("No element to be popped!");
	}

	virtual void popAt(const size_t index) override {
		if (m_IsPopulated[index]) { //is populated
			m_IsPopulated[index] = false;
			return;
		}
		throw std::runtime_error("No element at index to be popped!");
	}

	virtual void pop(const TType& obj) override {
		if constexpr (sutil::is_equality_comparable_v<TType>) {
			for (size_t index = 0; index < getSize(); ++index) {
				if (m_Container[index] == obj) {
					m_IsPopulated[index] = false;
				}
			}
		} else {
			throw std::runtime_error("Type is not comparable!");
		}
	}

#ifdef USING_SIMPLEPTR
	virtual void pop(typename TUnfurled<TType>::Type* obj) override {
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

protected:

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