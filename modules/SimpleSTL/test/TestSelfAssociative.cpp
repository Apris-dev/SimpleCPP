#include <iostream>
#include <random>
#include <cassert>

#include "TestShared.h"
#include "sptr/Memory.h"

#include "sstl/MultiSet.h"
#include "sstl/PriorityMultiSet.h"
#include "sstl/Set.h"
#include "sstl/PrioritySet.h"

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

#define SINGLE_TEST(...) \
	{ std::cout << std::endl << "--------------------" << std::endl << #__VA_ARGS__ " Test" << std::endl; } \
	{ __VA_ARGS__ container; containerTest(#__VA_ARGS__, container); transferTest(#__VA_ARGS__, container); }

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

int main() {
	DO_ASSOCIATIVE_TEST(TSet)
	DO_ASSOCIATIVE_TEST(TMultiSet)
	DO_ASSOCIATIVE_TEST(TPrioritySet)
	DO_ASSOCIATIVE_TEST(TPriorityMultiSet)

	return 0;
}