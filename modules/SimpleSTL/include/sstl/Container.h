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

#ifdef USING_SIMPLEPTR
	using TUnfurledType = typename TUnfurled<TType>::Type;
#endif

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
	[[nodiscard]] bool isValid(const size_t index) const  { return _derived().isValid(index); }

	// Checks if a certain object is contained within the container
	[[nodiscard]] bool contains(const TType& obj) const { return _derived().contains(obj); }

#ifdef USING_SIMPLEPTR
	// Version of contains that guarantees raw pointer input
	[[nodiscard]] bool contains(const TFrail<TUnfurledType>& obj) const { return _derived().contains(obj); }
#endif

	// Find a certain element in the container
	[[nodiscard]] size_t find(const TType& obj) const { return _derived().find(obj); }

#ifdef USING_SIMPLEPTR
	// Version of contains that guarantees raw pointer input
	[[nodiscard]] size_t find(const TFrail<TUnfurledType>& obj) const { return _derived().find(obj); }
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
	void pop(const TFrail<TUnfurledType>& obj) { _derived().pop(obj); }
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

protected:

	static TContainerType& _derived(TSequenceContainer& self) {
		return self._derived();
	}

	TContainerType& _derived() {
		return *static_cast<TContainerType*>(this);
	}

	const TContainerType& _derived() const {
		return *static_cast<const TContainerType*>(this);
	}
};

// Designed to be a container with a key for indexing
// Note: always requires a comparable key type
/*template <typename TContainerType,
	std::enable_if_t<sutil::is_equality_comparable_v<typename TContainerTraits<TContainerType>::KeyType, typename TContainerTraits<TContainerType>::KeyType>, int> = 0
>*/
template <typename TContainerType>
struct TAssociativeContainer {

	using Traits = TContainerTraits<TContainerType>;

	using TKeyType = typename Traits::KeyType;
	using TValueType = typename Traits::ValueType;
	using Iterator = TVirtualIterator<typename Traits::Iterator>;
	using ConstIterator = TVirtualIterator<typename Traits::ConstIterator>;
	constexpr static bool bHasHashing = Traits::bHasHashing;

#ifdef USING_SIMPLEPTR
	using TUnfurledValueType = typename TUnfurled<TValueType>::Type;
#endif

	// Returns the size of the container
	[[nodiscard]] size_t getSize() const { return _derived().getSize(); }

	// Returns if the container is empty
	[[nodiscard]] bool isEmpty() const { return _derived().isEmpty(); }

	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] TPair<TKeyType, const TValueType&> top() const { return _derived().top(); }

	// Gets the last element possible, or the 'bottom' of the container
	[[nodiscard]] TPair<TKeyType, const TValueType&> bottom() const { return _derived().bottom(); }

	[[nodiscard]] Iterator begin() noexcept { return _derived().begin(); }

	[[nodiscard]] ConstIterator begin() const noexcept { return _derived().begin(); }

	[[nodiscard]] Iterator end() noexcept { return _derived().end(); }

	[[nodiscard]] ConstIterator end() const noexcept { return _derived().end(); }

	// Checks if a certain key is contained within the container
	[[nodiscard]] bool isValid(const TKeyType& key) const { return _derived().isValid(key); }

	// Checks if a certain object is contained within the container
	[[nodiscard]] bool contains(const TValueType& obj) const { return _derived().contains(obj); }

#ifdef USING_SIMPLEPTR
	// Version of contains that guarantees raw pointer input
	[[nodiscard]] bool contains(const TFrail<TUnfurledValueType>& obj) const { return _derived().contains(obj); }
#endif

	// Find a certain element in the container
	[[nodiscard]] TKeyType find(const TValueType& obj) const { return _derived().find(obj); }

#ifdef USING_SIMPLEPTR
	// Version of contains that guarantees raw pointer input
	[[nodiscard]] TKeyType find(const TFrail<TUnfurledValueType>& obj) const { return _derived().find(obj); }
#endif

	// Get an element at a specified key
	[[nodiscard]] TValueType& get(const TKeyType& key) { return _derived().get(key); }
	// Get an element at a specified key
	[[nodiscard]] const TValueType& get(const TKeyType& key) const { return _derived().get(key); }

	// Fills container with TType& elements with size amt
	void resize(size_t amt, std::function<TPair<TKeyType, TValueType>()> func) { _derived().resize(amt, func); }

	// Reserves memory for n elements
	ENABLE_FUNC_IF(bHasHashing)
	void reserve(size_t amt) { _derived().reserve(amt); }

	// Adds a defaulted element to the container
	TPair<TKeyType, const TValueType&> push() { return _derived().push(); }
	// Adds a defaulted element at key to the container
	TValueType& push(const TKeyType& key) { return _derived().push(key); }
	// Adds an element value at key to the container
	TValueType& push(const TKeyType& key, const TValueType& value) { return _derived().push(key, value); }
	// Adds an element value at key to the container
	TValueType& push(const TKeyType& key, TValueType&& value) { return _derived().push(key, std::move(value)); }
	// Adds an element to the container
	void push(const TPair<TKeyType, TValueType>& pair) { _derived().push(pair); }
	// Adds an element to the container
	void push(TPair<TKeyType, TValueType>&& pair) { _derived().push(std::move(pair)); }
	// Replaces a specified element at key with another element
	void replace(const TKeyType& key, const TValueType& obj) { _derived().replace(key, obj); }
	// Replaces a specified element at key with another element
	void replace(const TKeyType& key, TValueType&& obj) { _derived().replace(key, std::move(obj)); }

	// Removes all elements from the container
	void clear() { _derived().clear(); }

	// Removes the topmost element from the container
	void pop() { _derived().pop(); }
	// Removes an element at key from the container
	void pop(const TKeyType& key) { _derived().pop(key); }

	// Moves an object at key from this to container otr
	template <typename TOtherContainerType>
	void transfer(TAssociativeContainer<TOtherContainerType>& otr, const TKeyType& key) {
		_derived().transfer(otr, key);
	}

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

protected:

	static TContainerType& _derived(TAssociativeContainer& self) {
		return self._derived();
	}

	TContainerType& _derived() {
		return *static_cast<TContainerType*>(this);
	}

	const TContainerType& _derived() const {
		return *static_cast<const TContainerType*>(this);
	}
};

// Designed to be a container without indexing
// Note: always requires a comparable type
/*template <typename TContainerType,
	std::enable_if_t<sutil::is_equality_comparable_v<typename TContainerTraits<TContainerType>::Type, typename TContainerTraits<TContainerType>::Type>, int> = 0
>*/
template <typename TContainerType>
struct TSingleAssociativeContainer {

	using Traits = TContainerTraits<TContainerType>;

	using TType = typename Traits::Type;
	using Iterator = TVirtualIterator<typename Traits::Iterator>;
	using ConstIterator = TVirtualIterator<typename Traits::ConstIterator>;
	constexpr static bool bHasHashing = Traits::bHasHashing;

#ifdef USING_SIMPLEPTR
	using TUnfurledType = typename TUnfurled<TType>::Type;
#endif

	// Returns the size of the container
	[[nodiscard]] size_t getSize() const { return _derived().getSize(); }

	// Returns if the container is empty
	[[nodiscard]] bool isEmpty() const { return _derived().isEmpty(); }

	// Gets the first element possible, or the 'top' of the container
	[[nodiscard]] const TType& top() const { return _derived().top(); }

	// Gets the last element possible, or the 'bottom' of the container
	[[nodiscard]] const TType& bottom() const { return _derived().bottom(); }

	[[nodiscard]] Iterator begin() noexcept { return _derived().begin(); }

	[[nodiscard]] ConstIterator begin() const noexcept { return _derived().begin(); }

	[[nodiscard]] Iterator end() noexcept { return _derived().end(); }

	[[nodiscard]] ConstIterator end() const noexcept { return _derived().end(); }

	// Checks if a certain index is contained within the container
	[[nodiscard]] bool contains(const size_t index) const {
		return index > 0 && index < getSize();
	}

	// Checks if a certain object is contained within the container
	[[nodiscard]] bool contains(const TType& obj) const { return _derived().contains(obj); }

#ifdef USING_SIMPLEPTR
	// Version of contains that guarantees raw pointer input
	[[nodiscard]] bool contains(const TFrail<TUnfurledType>& obj) const { return _derived().contains(obj); }
#endif

	// Fills container with n defaulted elements
	void resize(size_t amt) { _derived().resize(amt); }

	// Fills container with TType& elements with size amt
	void resize(size_t amt, std::function<TType()> func) { _derived().resize(amt, func); }

	// Reserves memory for n elements
	ENABLE_FUNC_IF(bHasHashing)
	void reserve(size_t amt) { _derived().reserve(amt); }

	// Adds a defaulted element to the container
	const TType& push() { return _derived().push(); }
	// Adds an element to the container
	void push(const TType& obj) { _derived().push(obj); }
	// Adds an element to the container
	void push(TType&& obj) { _derived().push(std::move(obj)); }

	// Replaces a specified element with another element
	void replace(const TType& tgt, const TType& obj) { _derived().replace(tgt, obj); }
	// Replaces a specified element with another element
	void replace(const TType& tgt, TType&& obj) { _derived().replace(tgt, std::move(obj)); }

	// Removes all elements from the container
	void clear() { _derived().clear(); }

	// Removes the topmost element from the container
	void pop() { _derived().pop(); }
	// Removes an element from the container
	void pop(const TType& obj) { _derived().pop(obj); }

#ifdef USING_SIMPLEPTR
	// Version of pop that guarantees raw pointer input, is O(n), unlike normal pop, due to comparisons
	void pop(const TFrail<TUnfurledType>& obj) { _derived().pop(obj); }
#endif

	// Moves an object from this to container otr
	template <typename TOtherContainerType>
	void transfer(TSingleAssociativeContainer<TOtherContainerType>& otr, TType& obj) {
		_derived().transfer(otr, obj);
	}

#ifdef USING_SIMPLEPTR
	// Version of transfer that guarantees raw pointer input
	template <typename TOtherContainerType>
	void transfer(TSingleAssociativeContainer<TOtherContainerType>& otr, const TFrail<TUnfurledType>& obj) {
		_derived().transfer(otr, obj);
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

protected:

	static TContainerType& _derived(TSingleAssociativeContainer& self) {
		return self._derived();
	}

	TContainerType& _derived() {
		return *static_cast<TContainerType*>(this);
	}

	const TContainerType& _derived() const {
		return *static_cast<const TContainerType*>(this);
	}
};