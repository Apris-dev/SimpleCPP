add_test(TestSequenced
        TestShared.h
        TestSequenced.cpp
)

link_test(SimpleArchive)
link_test(SimplePtr)

add_test(TestAssociative
        TestShared.h
        TestAssociative.cpp
)

link_test(SimpleArchive)
link_test(SimplePtr)

add_test(TestSelfAssociative
        TestShared.h
        TestSelfAssociative.cpp
)

link_test(SimpleArchive)
link_test(SimplePtr)

add_test(ReleaseTest
        TestShared.h
        ReleaseTest.cpp
)

link_test(SimplePtr)

add_test(TestItr
        TestItr.cpp
)