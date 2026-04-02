#include <iostream>
#include <random>
#include <cassert>
#include <cstdint>

#include "TestShared.h"
#include "sptr/Memory.h"

#include "sstl/Map.h"
#include "sstl/PriorityMap.h"
#include "sstl/MultiMap.h"
#include "sstl/PriorityMultiMap.h"

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

#ifdef USING_SIMPLEARCHIVE
inline COutputArchive& operator<<(COutputArchive& inArchive, const MapEnum& obj) {
	inArchive << (size_t)obj;
	return inArchive;
}
#endif

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

template <typename TContainerType>
void containerTest(const std::string& containerName, TAssociativeContainer<TContainerType>& container) {

	using TType = typename TAssociativeContainer<TContainerType>::TValueType;

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

template <typename TContainerType>
void transferTest(const std::string& containerName, TAssociativeContainer<TContainerType>& container) {

	using TType = typename TAssociativeContainer<TContainerType>::TValueType;

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
	DO_MAP_TEST(TMap)
	DO_MAP_TEST(TMultiMap)
	DO_MAP_TEST(TPriorityMap)
	DO_MAP_TEST(TPriorityMultiMap)

	return 0;
}