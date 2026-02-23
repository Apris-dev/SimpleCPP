add_simplecpp_test(SimpleUtils ThreadedTest
        ThreadedTest.cpp
)

link_simplecpp_test(SimpleUtils ThreadedTest SimpleSTL)
link_simplecpp_test(SimpleUtils ThreadedTest SimplePtr)