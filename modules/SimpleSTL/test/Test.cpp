#include <iostream>
#include <random>
#include <cassert>

#include "sstl/Array.h"
#include "sstl/Deque.h"
#include "sstl/ForwardList.h"
#include "sstl/Vector.h"
#include "sstl/MaxHeap.h"
#include "sstl/MinHeap.h"
#include "sstl/List.h"
#include "sstl/MultiSet.h"
#include "sstl/PriorityMultiSet.h"
#include "sstl/Queue.h"
#include "sstl/Set.h"
#include "sstl/PrioritySet.h"
#include "sstl/Stack.h"
#include "sstl/Map.h"
#include "sptr/Memory.h"
#include "sstl/PriorityMap.h"
#include "sstl/MultiMap.h"
#include "sstl/PriorityMultiMap.h"

#include "TestShared.h"

template <typename TType>
#if CXX_VERSION >= 20
requires std::is_base_of_v<Abstract, typename TUnfurled<TType>::Type>
#endif
void containerTest(const std::string& containerName, TSequenceContainer<TType>& container) {

	std::vector<size_t> vec;
	for (size_t i = 0; i < 10; ++i) {
		vec.push_back(i+500);
	}

	std::random_device rd;
	std::mt19937 rng(rd());

	SHUFFLE(vec, rng);

	container.resize(10, [&](const size_t index) {
		TType obj = TUnfurled<TType>::template create<SObject>(vec[index], containerName);
		return obj;
	});
	assert(container.getSize() == 10);

	const size_t size = container.getSize();
	for (size_t i = 0; i < size; ++i) {
		sstl::getUnfurled(container.top())->print();
		container.pop();
	}
	std::cout << std::endl;

	container.clear();

}


template <typename TType>
#if CXX_VERSION >= 20
requires std::is_base_of_v<Abstract, typename TUnfurled<TType>::Type>
#endif
void transferTest(const std::string& containerName, TSequenceContainer<TType>& container) {

	{
		std::cout << "Vector Transfer Test" << std::endl;

		TVector<TType> from;
		from.push(TUnfurled<TType>::template create<SObject>((size_t)100, containerName));

		std::cout << "Pre Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		from.forEach([](size_t index, const TType& obb) {sstl::getUnfurled(obb)->print();});
		std::cout << "to:" << std::endl;
		container.forEach([](size_t index, const TType& obb) {sstl::getUnfurled(obb)->print();});

		//assert(from.getSize() == 1);

		from.transfer(container, 0);

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		from.forEach([](size_t index, const TType& obb) {sstl::getUnfurled(obb)->print();});
		std::cout << "to:" << std::endl;
		container.forEach([](size_t index, const TType& obb) {sstl::getUnfurled(obb)->print();});
		std::cout << std::endl;

		//assert(container.getSize() == 1);

		container.clear();
	}

	{
		std::cout << "List Transfer Test" << std::endl;

		TList<TType> from;
		from.push(TUnfurled<TType>::template create<SObject>((size_t)100, containerName));

		std::cout << "Pre Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		from.forEach([](size_t index, const TType& obb) {sstl::getUnfurled(obb)->print();});
		std::cout << "to:" << std::endl;
		container.forEach([](size_t index, const TType& obb) {sstl::getUnfurled(obb)->print();});

		//assert(from.getSize() == 1);

		from.transfer(container, 0);

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		from.forEach([](size_t index, const TType& obb) {sstl::getUnfurled(obb)->print();});
		std::cout << "to:" << std::endl;
		container.forEach([](size_t index, const TType& obb) {sstl::getUnfurled(obb)->print();});
		std::cout << std::endl;

		//assert(container.getSize() == 1);

		container.clear();
	}
}

template <typename TType>
#if CXX_VERSION >= 20
requires std::is_base_of_v<Abstract, typename TUnfurled<TType>::Type>
#endif
void containerTest(const std::string& containerName, TSingleAssociativeContainer<TType>& container) {

	std::vector<size_t> vec;
	for (size_t i = 0; i < 10; ++i) {
		vec.push_back(i);
	}

	std::random_device rd;
	std::mt19937 rng(rd());

	SHUFFLE(vec, rng);

	size_t i = 0;

	container.resize(10, [&] {
		auto object = TUnfurled<TType>::template create<SObject>(vec[i], containerName);
		++i;
		return object;
	});
	assert(container.getSize() == 10);

	const size_t size = container.getSize();
	for (size_t v = 0; v < size; ++v) {
		sstl::getUnfurled(container.top())->print();
		container.pop();
	}
	std::cout << std::endl;
}

template <typename TType>
#if CXX_VERSION >= 20
requires std::is_base_of_v<Abstract, typename TUnfurled<TType>::Type>
#endif
void transferTest(const std::string& containerName, TSingleAssociativeContainer<TType>& container) {

	{
		std::cout << "Set Transfer Test" << std::endl;

		TSet<TType> from;
		from.push(TUnfurled<TType>::template create<SObject>((size_t)100, containerName));

		std::cout << "Pre Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		from.forEach([](const TType& obb) {sstl::getUnfurled(obb)->print();});
		std::cout << "to:" << std::endl;
		container.forEach([](const TType& obb) {sstl::getUnfurled(obb)->print();});

		assert(from.getSize() == 1);

		from.transfer(container, const_cast<TType&>(from.top()));

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		from.forEach([](const TType& obb) {sstl::getUnfurled(obb)->print();});
		std::cout << "to:" << std::endl;
		container.forEach([](const TType& obb) {sstl::getUnfurled(obb)->print();});
		std::cout << std::endl;

		assert(container.getSize() == 1);

		container.clear();
	}
}

enum class MapEnum : uint8_t {
	NONE,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE
};

inline COutputArchive& operator<<(COutputArchive& inArchive, const MapEnum& obj) {
	inArchive << (size_t)obj;
	return inArchive;
}

std::string enumToString(const MapEnum val) {
	switch (val) {
		case MapEnum::NONE:
			return "NONE";
		case MapEnum::ONE:
			return "ONE";
		case MapEnum::TWO:
			return "TWO";
		case MapEnum::THREE:
			return "THREE";
		case MapEnum::FOUR:
			return "FOUR";
		case MapEnum::FIVE:
			return "FIVE";
		case MapEnum::SIX:
			return "SIX";
		case MapEnum::SEVEN:
			return "SEVEN";
		case MapEnum::EIGHT:
			return "EIGHT";
		case MapEnum::NINE:
			return "NINE";
	}
	return "";
}

template <typename TType>
#if CXX_VERSION >= 20
requires std::is_base_of_v<Abstract, typename TUnfurled<TType>::Type>
#endif
void containerTest(const std::string& containerName, TAssociativeContainer<MapEnum, TType>& container) {

	std::vector<size_t> vec;
	for (size_t i = 0; i < 10; ++i) {
		vec.push_back(i);
	}

	std::random_device rd;
	std::mt19937 rng(rd());

	SHUFFLE(vec, rng);

	size_t i = 0;

	container.resize(10, [&] {
		auto pair = TPair<MapEnum, TType>{(MapEnum)vec[i], TUnfurled<TType>::template create<SObject>(vec[i], containerName)};
		++i;
		return pair;
	});
	assert(container.getSize() == 10);

	const size_t size = container.getSize();
	for (size_t v = 0; v < size; ++v) {
		MapEnum enm = (MapEnum)v;
		if (auto object = sstl::getUnfurled(container.get(enm))) {
			std::cout << "Key: " << enumToString(enm) << " ";
			object->print();
			container.pop(enm);
		}
	}

	std::cout << std::endl;
}

template <typename TType>
#if CXX_VERSION >= 20
requires std::is_base_of_v<Abstract, typename TUnfurled<TType>::Type>
#endif
void transferTest(const std::string& containerName, TAssociativeContainer<MapEnum, TType>& container) {

	{
		std::cout << "Map Transfer Test" << std::endl;

		TMap<MapEnum, TType> from;
		from.push(MapEnum::NONE, TUnfurled<TType>::template create<SObject>((size_t)100, containerName));

		std::cout << "Pre Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		from.forEach([](const TPair<MapEnum, const TType&>& obb) {
			std::cout << "Key: " << enumToString(obb.first) << " ";
			sstl::getUnfurled(obb.second)->print();
		});
		std::cout << "to:" << std::endl;
		container.forEach([](const TPair<MapEnum, const TType&>& obb) {
			std::cout << "Key: " << enumToString(obb.first) << " ";
			sstl::getUnfurled(obb.second)->print();
		});

		assert(from.getSize() == 1);

		from.transfer(container, MapEnum::NONE);

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		from.forEach([](const TPair<MapEnum, const TType&>& obb) {
			std::cout << "Key: " << enumToString(obb.first) << " ";
			sstl::getUnfurled(obb.second)->print();
		});
		std::cout << "to:" << std::endl;
		container.forEach([](const TPair<MapEnum, const TType&>& obb) {
			std::cout << "Key: " << enumToString(obb.first) << " ";
			sstl::getUnfurled(obb.second)->print();
		});
		std::cout << std::endl;

		assert(container.getSize() == 1);

		container.clear();
	}
}

#define SINGLE_TEST(...) \
	{ std::cout << std::endl << "--------------------" << std::endl << #__VA_ARGS__ " Test" << std::endl; } \
	{ __VA_ARGS__ container; containerTest(#__VA_ARGS__, container); transferTest(#__VA_ARGS__, container); }

#define CONSTRUCTOR_TEST(...) \
	{ std::cout << std::endl << "--------------------" << std::endl << #__VA_ARGS__ " Constructor Test" << std::endl; } \
	{ __VA_ARGS__ container{0, 5, 10}; container.forEach([](size_t, const int& i) { std::cout << i << std::endl; }); } \
	{ std::cout << std::endl << "--------------------" << std::endl << #__VA_ARGS__ " Unique Constructor Test" << std::endl; } \
	{ __VA_ARGS__ container{TUnique{0}, TUnique{5}, TUnique{10}}; container.forEach([](size_t, const TUnique<int>& i) { std::cout << *i << std::endl; }); }


#define DO_TEST(x) \
    std::cout << std::endl << std::endl << "******************** " #x " ********************" << std::endl << std::endl; \
	SINGLE_TEST(x<Parent>) \
	SINGLE_TEST(x<TShared<Parent>>) \
	SINGLE_TEST(x<TUnique<Parent>>) \
	SINGLE_TEST(x<TUnique<Abstract>>) \
	CONSTRUCTOR_TEST(x)

#define DO_ARRAY_TEST(x) \
    std::cout << std::endl << std::endl << "******************** " #x " ********************" << std::endl << std::endl; \
	SINGLE_TEST(x<Parent, 10>) \
	SINGLE_TEST(x<TShared<Parent>, 10>) \
	SINGLE_TEST(x<TUnique<Parent>, 10>) \
	SINGLE_TEST(x<TUnique<Abstract>, 10>) \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Constructor Test" << std::endl; } \
	{ x<int, 3> container{0, 5, 10}; container.forEach([](size_t, const int& i) { std::cout << i << std::endl; }); } \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Unique Constructor Test" << std::endl; } \
	{ x<TUnique<int>, 3> container{TUnique{0}, TUnique{5}, TUnique{10}}; container.forEach([](size_t, const TUnique<int>& i) { std::cout << *i << std::endl; }); }


#define DO_ASSOCIATIVE_TEST(x) \
    std::cout << std::endl << std::endl << "******************** " #x " ********************" << std::endl << std::endl; \
	SINGLE_TEST(x<Parent>) \
	SINGLE_TEST(x<TShared<Parent>>) \
	SINGLE_TEST(x<TUnique<Parent>>) \
	SINGLE_TEST(x<TUnique<Abstract>>) \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Constructor Test" << std::endl; } \
	{ x container{0, 5, 10}; container.forEach([](const int& i) { std::cout << i << std::endl; }); } \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Unique Constructor Test" << std::endl; } \
	{ x container{TUnique{0}, TUnique{5}, TUnique{10}}; container.forEach([](const TUnique<int>& i) { std::cout << *i.get() << std::endl; }); }

#define DO_MAP_TEST(x) \
    std::cout << std::endl << std::endl << "******************** " #x " ********************" << std::endl << std::endl; \
	SINGLE_TEST(x<MapEnum, Parent>) \
	SINGLE_TEST(x<MapEnum, TShared<Parent>>) \
	SINGLE_TEST(x<MapEnum, TUnique<Parent>>) \
	SINGLE_TEST(x<MapEnum, TUnique<Abstract>>) \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Constructor Test" << std::endl; } \
	{ x container{TPair{MapEnum::NONE, 0}, TPair{MapEnum::ONE, 5}, TPair{MapEnum::TWO, 10}}; container.forEach([](TPair<MapEnum, const int&> pair) { std::cout << pair.second << std::endl; }); } \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Unique Constructor Test" << std::endl; } \
	{ x container{TPair{MapEnum::NONE, TUnique{0}}, TPair{MapEnum::ONE, TUnique{5}}, TPair{MapEnum::TWO, TUnique{10}}}; container.forEach([](TPair<MapEnum, const TUnique<int>&> pair) { std::cout << *pair.second.get() << std::endl; }); } \


int main() {
	DO_TEST(TVector)
	DO_TEST(TMaxHeap)
	DO_TEST(TMinHeap)
	DO_TEST(TDeque)
	DO_TEST(TList)
	DO_TEST(TForwardList)
	DO_ARRAY_TEST(TArray)
	DO_TEST(TStack)
	DO_TEST(TQueue)
	DO_ASSOCIATIVE_TEST(TSet)
	DO_ASSOCIATIVE_TEST(TMultiSet)
	DO_ASSOCIATIVE_TEST(TPrioritySet)
	DO_ASSOCIATIVE_TEST(TPriorityMultiSet)
	DO_MAP_TEST(TMap)
	DO_MAP_TEST(TMultiMap)
	DO_MAP_TEST(TPriorityMap)
	DO_MAP_TEST(TPriorityMultiMap)

	std::cout << std::endl;

	TVector<size_t> vec;

	for (size_t i = 0; i < 10; ++i) {
		vec.push(i);
		CHashArchive archive;
		archive << vec;
		std::cout << archive.get() << std::endl;
	}

	return 0;
}