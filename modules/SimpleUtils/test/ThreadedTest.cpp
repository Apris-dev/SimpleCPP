#include <iostream>
#include <thread>

#include "sstl/Vector.h"
#include "sptr/Memory.h"
#include "sutil/Threading.h"

#include "TestShared.h"

int main() {

	CThreadPool pool{1};

	for (int i = 0; i < 10; ++i) {
		pool.run([] {
			std::cout << "Hello World!" << std::endl;
		});
	}

	std::cout << "Sent Hello Dispatch" << std::endl;

	pool.wait();

	return 0;

	TThreadSafe<TVector<TUnique<SObject>>> vec;

	char c = 'y';
	while (c != 'n') {
		std::thread threadOne([&] {
			vec->push(TUnique<SObject>{100, "Thread One"});
			vec->top()->print();
			//vec->doFor(0, [](const TUnique<SObject>& obj) { obj->print(); });
			vec->popAt(static_cast<size_t>(0));
		});
		vec->push(TUnique<SObject>{101, "Thread Two"});
		vec->top()->print();
		//vec->doFor(0, [](TUnique<SObject>& obj) { obj->print(); });
		vec->popAt(static_cast<size_t>(0));

		std::cout << "Continue?" << std::endl;
		std::cin >> c;

		threadOne.join();
	}

	vec->clear();

	return 0;
}