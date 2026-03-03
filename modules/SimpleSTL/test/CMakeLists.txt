add_simplecpp_test(SimpleSTL TestSequenced
        TestShared.h
        TestSequenced.cpp
)

link_simplecpp_test(SimpleSTL TestSequenced SimpleArchive)

add_simplecpp_test(SimpleSTL TestAssociative
        TestShared.h
        TestAssociative.cpp
)

link_simplecpp_test(SimpleSTL TestAssociative SimpleArchive)

add_simplecpp_test(SimpleSTL TestSelfAssociative
        TestShared.h
        TestSelfAssociative.cpp
)

link_simplecpp_test(SimpleSTL TestSelfAssociative SimpleArchive)

add_simplecpp_test(SimpleSTL ReleaseTest
        TestShared.h
        ReleaseTest.cpp
)