#pragma once

/*
 * Container Base class designed to use CRTP to avoid runtime type deduction.
 * This also has the added benefit of avoiding compiling functions with limited access. (Like stack/queue push at index)
 */

#include <functional>
#include <stdexcept>
#include <algorithm>

#ifdef USING_SIMPLEARCHIVE
#include "sarch/Archive.h"
#include "sarch/HashArchive.h"
#endif

#include "sutil/Comparison.h"
#include "sutil/PlatformDefinition.h"

#if CXX_VERSION >= 20
#include <algorithm>
#endif

#ifdef USING_SIMPLEPTR
#include "sptr/Memory.h"
#endif

#include "sutil/Pair.h"

#if CXX_VERSION >= 20
#ifdef USING_MSVC
#define FIND(c, x, ...) std::ranges::find(c, x, ##__VA_ARGS__)
#else
#define FIND(c, x, ...) std::ranges::find(c, x __VA_OPT__(,) __VA_ARGS__)
#endif
#define ERASE(c, x, ...) c.erase(FIND(c, x, __VA_ARGS__))
#define ASSOCIATIVE_CONTAINS(c, x) c.contains(x)
#define DISTANCE(c, x, ...) std::ranges::distance(c.begin(), FIND(c, x, __VA_ARGS__))
#define SIZE(c) std::ranges::distance(c)
#define SHUFFLE(c, r) std::ranges::shuffle(c, r);
#else
#define FIND(c, x, ...) std::find(c.begin(), c.end(), x)
#define ERASE(c, x, ...) c.erase(FIND(c, x))
#define ASSOCIATIVE_CONTAINS(c, x) c.find(x) != c.end()
#define DISTANCE(c, x, ...) std::distance(c.begin(), FIND(c, x))
#define SIZE(c) std::distance(c.begin(), c.end())
#define SHUFFLE(c, r) std::shuffle(c.begin(), c.end(), r);
#endif

#define CONTAINS(c, x, ...) FIND(c, x, __VA_ARGS__) != c.end()

// Makes it easy to see if a function is guaranteed or not
#define GUARANTEED = 0;
#define NOT_GUARANTEED { throw std::runtime_error("Attempted Usage of unimplemented function in TContainer."); }

// A universal iterator that guarantees std::advance
template <typename TItrType>
struct TVirtualIterator {

    TVirtualIterator(const TItrType& inItr): itr(inItr) {}

	//TODO: verify offset or keep unsafe...

	decltype(auto) operator*() noexcept {
		return *itr;
	}

	decltype(auto) operator[](const int offset) noexcept {
		return itr[offset];
    }

	decltype(auto) operator[](const int offset) const noexcept {
		return itr[offset];
	}

	bool operator==(TVirtualIterator otr) {
		return itr == otr.itr;
	}

	bool operator!=(TVirtualIterator otr) {
		return itr != otr.itr;
	}

	bool operator<(const TVirtualIterator& otr) const noexcept {
		return itr < otr.itr;
	}

	bool operator>(const TVirtualIterator& otr) const noexcept {
		return otr < *this;
	}

	bool operator<=(const TVirtualIterator& otr) const noexcept {
		return !(otr < *this);
	}

	bool operator>=(const TVirtualIterator& otr) const noexcept {
		return !(*this < otr);
	}

	TVirtualIterator& operator++() noexcept {
		std::advance(itr, 1);
		return *this;
	}

	TVirtualIterator operator++(int) noexcept {
        TVirtualIterator otr = *this;
    	++*this;
    	return otr;
    }

	TVirtualIterator& operator+=(const int offset) noexcept {
		std::advance(itr, offset);
		return *this;
	}

	TVirtualIterator operator+(const int offset) noexcept {
		TVirtualIterator otr = *this;
		otr += offset;
		return otr;
	}

	friend TVirtualIterator operator+(const int offset, TVirtualIterator otr) noexcept {
		otr += offset;
		return otr;
	}

	TVirtualIterator& operator--() noexcept {
		std::advance(itr, -1);
		return *this;
	}

	TVirtualIterator operator--(int) noexcept {
    	TVirtualIterator otr = *this;
    	--*this;
    	return otr;
    }

	TVirtualIterator& operator-=(const int offset) noexcept {
		std::advance(itr, -offset);
		return *this;
	}

	TVirtualIterator operator-(const int offset) noexcept {
		TVirtualIterator otr = *this;
		otr -= offset;
		return otr;
	}

	friend TVirtualIterator operator-(const int offset, TVirtualIterator otr) noexcept {
		otr -= offset;
		return otr;
	}

    TItrType itr;
};

template <typename TType>
struct TContainerHasher {
	size_t operator()(const TType& p) const noexcept {
#ifdef USING_SIMPLEARCHIVE
		CHashArchive archive;
		archive << p;
		return archive.get();
#else
		return std::hash<TType>()(p);
#endif
	}
};

template <typename>
struct TContainerTraits;

// A basic container of any amount of objects
// A size of 0 implies a dynamic array
template <typename TContainerType>
struct TSequenceContainer {

	using Traits = TContainerTraits<TContainerType>;

	using TType = typename Traits::Type;
	using Iterator = TVirtualIterator<typename Traits::Iterator>;
	using ConstIterator = TVirtualIterator<typename Traits::ConstIterator>;
	constexpr static bool bIsContiguousMemory = Traits::bIsContiguousMemory;
	constexpr static bool bIsLimitedAccess = Traits::bIsLimitedAccess;

	//virtual ~TSequenceContainer() = default;

	// Returns the size of the container
	[[nodiscard]] size_t getSize() const { return _derived().getSize(); }

	// Returns if the container is empty
	[[nodiscard]] bool isEmpty() const { return _derived().isEmpty(); }

	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] TType& top() { return _derived().top(); }

	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] const TType& top() const { return _derived().top(); }

	// Gets the first element possible, or the 'top' of the container
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] TType& bottom() {
		return _derived().bottom();
	}
	// Gets the first element possible, or the 'top' of the container
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] const TType& bottom() const {
		return _derived().bottom();
	}

	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] Iterator begin() noexcept {
		return _derived().begin();
	}

	// Allow const access for limited access (like asking people in a queue a question)
	[[nodiscard]] ConstIterator begin() const noexcept { return _derived().begin(); }

	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] Iterator end() noexcept {
		return _derived().end();
	}

	// Allow const access for limited access (like asking people in a queue a question)
	[[nodiscard]] ConstIterator end() const noexcept { return _derived().end(); }

	// Checks if a certain index is contained within the container
	[[nodiscard]] bool containsAt(const size_t index) const  { return _derived().containsAt(index); }

	// Checks if a certain object is contained within the container
	[[nodiscard]] bool contains(const TType& obj) const { return _derived().contains(obj); }

#ifdef USING_SIMPLEPTR
	// Version of contains that guarantees raw pointer input
	[[nodiscard]] bool contains(typename TUnfurled<TType>::Type* obj) const { return _derived().contains(obj); }
#endif

	// Find a certain element in the container
	[[nodiscard]] size_t find(const TType& obj) const { return _derived().find(obj); }

#ifdef USING_SIMPLEPTR
	// Version of contains that guarantees raw pointer input
	[[nodiscard]] size_t find(typename TUnfurled<TType>::Type* obj) const { return _derived().find(obj); }
#endif

	// Get an element at a specified index
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] TType& get(size_t index) {
		return _derived().get(index);
	}
	// Get an element at a specified index
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] const TType& get(size_t index) const {
		return _derived().get(index);
	}

	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] TType& operator[](const size_t index) {
		return get(index);
	}
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	[[nodiscard]] const TType& operator[](const size_t index) const {
		return get(index);
	}

	// Fills container with n defaulted elements
	void resize(size_t amt) { _derived().resize(amt); }

	// Fills container with TType& elements with size amt
	void resize(size_t amt, std::function<TType(size_t)> func) { _derived().resize(amt, func); }

	// Reserves memory for n elements
	ENABLE_FUNC_IF(bIsContiguousMemory)
	void reserve(size_t amt) {
		_derived().reserve(amt);
	}

	// Adds a defaulted element to the container
	TType& push() { return _derived().push(); }
	// Adds an element to the container, returning the index where it was added
	size_t push(const TType& obj) { return _derived().push(obj); }
	// Adds an element to the container, returning the index where it was added
	size_t push(TType&& obj) { return _derived().push(std::move(obj)); }
	// Inserts an element at a specified index
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void push(size_t index, const TType& obj) {
		_derived().push(index, obj);
	}
	// Inserts an element at a specified index
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void push(size_t index, TType&& obj) {
		_derived().push(index, std::move(obj));
	}

	// Replaces an element at a specified index, and returns the original
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void replace(size_t index, const TType& obj) {
		_derived().push(index, obj);
	}
	// Replaces an element at a specified index, and returns the original
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void replace(size_t index, TType&& obj) {
		_derived().replace(index, std::move(obj));
	}

	// Removes all elements from the container
	void clear() { _derived().clear(); }

	// Removes the topmost element from the container
	void pop() { _derived().pop(); }
	// Removes an element at the specified index
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void popAt(size_t index) {
		_derived().popAt(index);
	}
	// Removes a certain object from the container
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void pop(const TType& obj) {
		_derived().pop(obj);
	}

#ifdef USING_SIMPLEPTR
	// Version of pop that guarantees raw pointer input
	ENABLE_FUNC_IF(!bIsLimitedAccess)
	void pop(typename TUnfurled<TType>::Type* obj) { _derived().pop(obj); }
#endif

	// Moves an object at index from this to container otr
	template <typename TOtherContainerType>
	void transfer(TSequenceContainer<TOtherContainerType>& otr, const size_t index) {
		static_assert(!bIsLimitedAccess, "Limited Access Type cannot be transferred from");
		_derived().transfer(otr, index);
	}

	void doFor(const size_t index, const std::function<void(TType&)>& func) {
		func(get(index));
	}

	void doFor(const size_t index, const std::function<void(const TType&)>& func) const {
		func(get(index));
	}

	void doFor(const size_t start, const size_t end, const std::function<void(size_t, TType&)>& func) {
		for (size_t i = start; i < end; ++i) {
			func(i, get(i));
		}
	}

	void doFor(const size_t start, const size_t end, const std::function<void(size_t, const TType&)>& func) const {
		for (size_t i = start; i < end; ++i) {
			func(i, get(i));
		}
	}

#ifdef USING_SIMPLEARCHIVE
	friend CInputArchive& operator>>(CInputArchive& inArchive, TSequenceContainer& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size, [&](size_t) {
			TType obj;
			inArchive >> obj;
			return obj;
		});
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const TSequenceContainer& inValue) {
		inArchive << inValue.getSize();
		for (const TType& obj : inValue) {
			inArchive << obj;
		}
		return inArchive;
	}
#endif

public:

	TContainerType& _derived() {
		return *static_cast<TContainerType*>(this);
	}

	const TContainerType& _derived() const {
		return *static_cast<const TContainerType*>(this);
	}
};

// Designed to be a container with a key for indexing
// Note: always requires a comparable key type
template <typename TContainerType,
	std::enable_if_t<sutil::is_equality_comparable_v<typename TContainerType::key_type, typename TContainerType::key_type>, int> = 0
>
struct TAssociativeContainer {

	using TKeyType = typename TContainerType::key_type;
	using TValueType = typename TContainerType::mapped_type;
	using Iterator = TVirtualIterator<typename TContainerType::iterator>;
	using ConstIterator = TVirtualIterator<typename TContainerType::const_iterator>;

	virtual ~TAssociativeContainer() = default;

	// Returns the size of the container
	[[nodiscard]] virtual size_t getSize() const
		GUARANTEED

	// Returns if the container is empty
	[[nodiscard]] virtual bool isEmpty() const
		GUARANTEED

	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] virtual TPair<TKeyType, const TValueType&> top() const
		GUARANTEED

	// Gets the last element possible, or the 'bottom' of the container
	[[nodiscard]] virtual TPair<TKeyType, const TValueType&> bottom() const
		GUARANTEED

	[[nodiscard]] virtual Iterator begin() noexcept
		GUARANTEED

	[[nodiscard]] virtual ConstIterator begin() const noexcept
		GUARANTEED

	[[nodiscard]] virtual Iterator end() noexcept
		GUARANTEED

	[[nodiscard]] virtual ConstIterator end() const noexcept
		GUARANTEED

	// Checks if a certain key is contained within the container
	[[nodiscard]] virtual bool contains(const TKeyType& key) const
		GUARANTEED

	// Get an element at a specified key
	[[nodiscard]] virtual TValueType& get(const TKeyType& key)
		GUARANTEED
	// Get an element at a specified key
	[[nodiscard]] virtual const TValueType& get(const TKeyType& key) const
		GUARANTEED

	// Fills container with TType& elements with size amt
	virtual void resize(size_t amt, std::function<TPair<TKeyType, TValueType>()> func)
		GUARANTEED

	// Reserves memory for n elements
	virtual void reserve(size_t amt)
		NOT_GUARANTEED

	// Adds a defaulted element to the container
	virtual TPair<TKeyType, const TValueType&> push()
		GUARANTEED
	// Adds a defaulted element at key to the container
	virtual TValueType& push(const TKeyType& key)
		GUARANTEED
	// Adds an element value at key to the container
	virtual TValueType& push(const TKeyType& key, const TValueType& value)
		GUARANTEED
	// Adds an element value at key to the container
	virtual TValueType& push(const TKeyType& key, TValueType&& value)
		GUARANTEED
	// Adds an element to the container
	virtual void push(const TPair<TKeyType, TValueType>& pair)
		GUARANTEED
	// Adds an element to the container
	virtual void push(TPair<TKeyType, TValueType>&& pair)
		GUARANTEED
	// Replaces a specified element at key with another element
	virtual void replace(const TKeyType& key, const TValueType& obj)
		GUARANTEED
	// Replaces a specified element at key with another element
	virtual void replace(const TKeyType& key, TValueType&& obj)
		GUARANTEED

	// Removes all elements from the container
	virtual void clear()
		GUARANTEED

	// Removes the topmost element from the container
	virtual void pop()
		GUARANTEED
	// Removes an element at key from the container
	virtual void pop(const TKeyType& key)
		GUARANTEED

private:

	virtual typename TContainerType::node_type extract(const TKeyType& key)
		GUARANTEED

public:

	// Moves an object at key from this to container otr
	template <typename TOtherContainerType,
		std::enable_if_t<std::conjunction_v<
			std::is_convertible<TKeyType, typename TAssociativeContainer<TOtherContainerType>::TKeyType>,
			std::is_convertible<TValueType, typename TAssociativeContainer<TOtherContainerType>::TValueType>
	>, int> = 0
	>
	void transfer(TAssociativeContainer<TOtherContainerType>& otr, const TKeyType& key) {
		auto itr = extract(key);
		// Prefer move, but copy if not available
		if constexpr (std::is_move_constructible_v<TValueType>) {
			otr.push(itr.key(), std::move(itr.mapped()));
		} else {
			otr.push(itr.key(), itr.mapped());
		}
	}

	// Iterates through each element (Maps do not support reverse iteration)
	virtual void forEach(const std::function<void(TPair<TKeyType, const TValueType&>)>& func) const
		GUARANTEED

#ifdef USING_SIMPLEARCHIVE
	friend CInputArchive& operator>>(CInputArchive& inArchive, TAssociativeContainer& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size, [&] {
			TPair<TKeyType, TValueType> pair;
			inArchive >> pair;
			return pair;
		});
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const TAssociativeContainer& inValue) {
		inArchive << inValue.getSize();
		inValue.forEach([&](TPair<TKeyType, const TValueType&> pair) {
			inArchive << pair;
		});
		return inArchive;
	}
#endif
};

// Designed to be a container without indexing
// Note: always requires a comparable type
template <typename TContainerType,
	std::enable_if_t<sutil::is_equality_comparable_v<typename TContainerType::value_type, typename TContainerType::value_type>, int> = 0
>
struct TSingleAssociativeContainer {

	using TType = typename TContainerType::value_type;
	using Iterator = TVirtualIterator<typename TContainerType::iterator>;
	using ConstIterator = TVirtualIterator<typename TContainerType::const_iterator>;

	virtual ~TSingleAssociativeContainer() = default;

	// Returns the size of the container
	[[nodiscard]] virtual size_t getSize() const
		GUARANTEED

	// Returns if the container is empty
	[[nodiscard]] virtual bool isEmpty() const
		GUARANTEED

	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] virtual const TType& top() const
		GUARANTEED

	// Gets the last element possible, or the 'bottom' of the container
	[[nodiscard]] virtual const TType& bottom() const
		GUARANTEED

	[[nodiscard]] virtual Iterator begin() noexcept
		GUARANTEED

	[[nodiscard]] virtual ConstIterator begin() const noexcept
		GUARANTEED

	[[nodiscard]] virtual Iterator end() noexcept
		GUARANTEED

	[[nodiscard]] virtual ConstIterator end() const noexcept
		GUARANTEED

	// Checks if a certain index is contained within the container
	[[nodiscard]] virtual bool contains(const size_t index) const {
		return index > 0 && index < getSize();
	}

	// Checks if a certain object is contained within the container
	[[nodiscard]] virtual bool contains(const TType& obj) const
		GUARANTEED

#ifdef USING_SIMPLEPTR
	// Version of contains that guarantees raw pointer input
	[[nodiscard]] virtual bool contains(typename TUnfurled<TType>::Type* obj) const
		GUARANTEED
#endif

	// Fills container with n defaulted elements
	virtual void resize(size_t amt)
		GUARANTEED

	// Fills container with TType& elements with size amt
	virtual void resize(size_t amt, std::function<TType()> func)
		GUARANTEED

	// Reserves memory for n elements
	virtual void reserve(size_t amt)
		NOT_GUARANTEED

	// Adds a defaulted element to the container
	virtual const TType& push()
		GUARANTEED
	// Adds an element to the container
	virtual void push(const TType& obj)
		GUARANTEED
	// Adds an element to the container
	virtual void push(TType&& obj)
		GUARANTEED

	// Replaces a specified element with another element
	virtual void replace(const TType&, const TType&)
		GUARANTEED
	// Replaces a specified element with another element
	virtual void replace(const TType&, TType&&)
		GUARANTEED

	// Removes all elements from the container
	virtual void clear()
		GUARANTEED

	// Removes the topmost element from the container
	virtual void pop()
		GUARANTEED
	// Removes an element from the container
	virtual void pop(const TType&)
		GUARANTEED

#ifdef USING_SIMPLEPTR
	// Version of pop that guarantees raw pointer input, is O(n), unlike normal pop, due to comparisons
	virtual void pop(typename TUnfurled<TType>::Type* obj)
		GUARANTEED
#endif

private:

	virtual typename TContainerType::node_type extract(TType& obj)
		GUARANTEED

#ifdef USING_SIMPLEPTR
	virtual typename TContainerType::node_type extract(typename TUnfurled<TType>::Type* obj)
		GUARANTEED
#endif

public:

	// Moves an object from this to container otr
	template <typename TOtherContainerType,
		std::enable_if_t<std::conjunction_v<std::is_convertible<TType, typename TSequenceContainer<TOtherContainerType>::TType>>, int> = 0
	>
	void transfer(TSingleAssociativeContainer<TOtherContainerType>& otr, TType& obj) {
		if (!this->contains(obj)) return;
		auto itr = extract(obj);
		// Prefer move, but copy if not available
		if constexpr (std::is_move_constructible_v<TType>) {
			otr.push(std::move(itr.value()));
		} else {
			otr.push(itr.value());
		}
	}

#ifdef USING_SIMPLEPTR
	// Version of transfer that guarantees raw pointer input
	template <typename TOtherContainerType,
		std::enable_if_t<std::conjunction_v<
			std::is_convertible<TType, typename TSingleAssociativeContainer<TOtherContainerType>::TType>
	>, int> = 0
	>
	void transfer(TSingleAssociativeContainer<TOtherContainerType>& otr, typename TUnfurled<TType>::Type* obj) {
		if constexpr (sstl::is_managed_v<TType>) {
			if (!this->contains(obj)) return;
			auto itr = extractPtr(obj);
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
#endif

#ifdef USING_SIMPLEARCHIVE
	friend CInputArchive& operator>>(CInputArchive& inArchive, TSingleAssociativeContainer& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size, [&] {
			TType obj;
			inArchive >> obj;
			return obj;
		});
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const TSingleAssociativeContainer& inValue) {
		inArchive << inValue.getSize();
		for (const TType& obj : inValue) {
			inArchive << obj;
		}
		return inArchive;
	}
#endif
};