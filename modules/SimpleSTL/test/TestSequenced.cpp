#include <iostream>
#include <random>
#include <cassert>

#include "TestShared.h"
#include "sptr/Memory.h"

#include "sstl/Array.h"
#include "sstl/Deque.h"
#include "sstl/ForwardList.h"
#include "sstl/Vector.h"
#include "sstl/MaxHeap.h"
#include "sstl/MinHeap.h"
#include "sstl/List.h"
#include "sstl/Queue.h"
#include "sstl/Stack.h"

template <typename TContainerType>
void containerTest(const std::string& containerName, TSequenceContainer<TContainerType>& container) {

	using TType = typename TSequenceContainer<TContainerType>::TType;

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


template <typename TContainerType>
void transferTest(const std::string& containerName, TSequenceContainer<TContainerType>& container) {

	using TType = typename TSequenceContainer<TContainerType>::TType;

	{
		std::cout << "Vector Transfer Test" << std::endl;

		TVector<TType> from;
		from.push(TUnfurled<TType>::template create<SObject>((size_t)100, containerName));

		std::cout << "Pre Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const TType& obb : from) { sstl::getUnfurled(obb)->print(); }
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }

		//assert(from.getSize() == 1);

		from.transfer(container, 0);

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const TType& obb : from) { sstl::getUnfurled(obb)->print(); }
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }
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
		for (const TType& obb : from) { sstl::getUnfurled(obb)->print(); }
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }

		//assert(from.getSize() == 1);

		from.transfer(container, 0);

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const TType& obb : from) { sstl::getUnfurled(obb)->print(); }
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }
		std::cout << std::endl;

		//assert(container.getSize() == 1);

		container.clear();
	}
}

#define SINGLE_TEST(...) \
	{ std::cout << std::endl << "--------------------" << std::endl << #__VA_ARGS__ " Test" << std::endl; } \
	{ __VA_ARGS__ container; containerTest(#__VA_ARGS__, container); transferTest(#__VA_ARGS__, container); }

#define DO_TEST(x) \
    std::cout << std::endl << std::endl << "******************** " #x " ********************" << std::endl << std::endl; \
	SINGLE_TEST(x<Parent>) \
	SINGLE_TEST(x<TShared<Parent>>) \
	SINGLE_TEST(x<TUnique<Parent>>) \
	SINGLE_TEST(x<TUnique<Abstract>>) \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Constructor Test" << std::endl; } \
	{ x container{0, 5, 10}; for (auto& i : container) { std::cout << i << std::endl; } } \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Unique Constructor Test" << std::endl; } \
	{ x container{TUnique{0}, TUnique{5}, TUnique{10}}; for (auto& i : container) { std::cout << *i << std::endl; } }


#define DO_ARRAY_TEST(x) \
    std::cout << std::endl << std::endl << "******************** " #x " ********************" << std::endl << std::endl; \
	SINGLE_TEST(x<Parent, 10>) \
	SINGLE_TEST(x<TShared<Parent>, 10>) \
	SINGLE_TEST(x<TUnique<Parent>, 10>) \
	SINGLE_TEST(x<TUnique<Abstract>, 10>) \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Constructor Test" << std::endl; } \
	{ x<int, 3> container{0, 5, 10}; for (auto& i : container) { std::cout << i << std::endl; } } \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Unique Constructor Test" << std::endl; } \
	{ x<TUnique<int>, 3> container{TUnique{0}, TUnique{5}, TUnique{10}}; for (auto& i : container) { std::cout << *i << std::endl; } }

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

	return 0;
}