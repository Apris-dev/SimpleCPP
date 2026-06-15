#include <iostream>
#include <vector>

#include "sstl/Container.h"
#include "sstl/Vector.h"
#include "sstl/MaxHeap.h"
#include "sstl/MinHeap.h"
#include "sstl/Deque.h"
#include "sstl/List.h"
#include "sstl/ForwardList.h"
#include "sstl/Stack.h"
#include "sstl/Array.h"
#include "sstl/Queue.h"

template <typename TContainerType>
void func(const TSequenceContainer<TContainerType>& val) {
    for (const auto& v : val) {
        std::cout << "Value is: " << v << std::endl;
    }
}

#define DO_TEST(x) \
    std::cout << "-----" #x "-----" << std::endl << std::endl; \
    { x<int> val; val.push(1); val.push(5); val.push(2); val.push(3); func(val); } \
    std::cout << std::endl;

#define DO_ARRAY_TEST(x) \
    std::cout << "-----" #x "-----" << std::endl << std::endl; \
    { x<int, 4> val; val.push(1); val.push(5); val.push(2); val.push(3); func(val); } \
    std::cout << std::endl;

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

    TForwardList<int> test;

    auto itr = test.begin();

    ++itr;



    return 0;
}