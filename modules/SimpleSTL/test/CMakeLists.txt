add_simplecpp_test(SimpleSTL TestSequenced
        TestShared.h
        TestSequenced.cpp
)

link_simplecpp_test(SimpleSTL TestSequenced SimpleArchive)
link_simplecpp_test(SimpleSTL TestSequenced SimplePtr)

add_simplecpp_test(SimpleSTL TestAssociative
        TestShared.h
        TestAssociative.cpp
)

link_simplecpp_test(SimpleSTL TestAssociative SimpleArchive)
link_simplecpp_test(SimpleSTL TestAssociative SimplePtr)

add_simplecpp_test(SimpleSTL TestSelfAssociative
        TestShared.h
        TestSelfAssociative.cpp
)

link_simplecpp_test(SimpleSTL TestSelfAssociative SimpleArchive)
link_simplecpp_test(SimpleSTL TestSelfAssociative SimplePtr)

add_simplecpp_test(SimpleSTL ReleaseTest
        TestShared.h
        ReleaseTest.cpp
)

link_simplecpp_test(SimpleSTL ReleaseTest SimplePtr)

add_simplecpp_test(SimpleSTL TestItr
        TestItr.cpp
)