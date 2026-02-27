add_simplecpp_test(SimpleSTL Test
        TestShared.h
        Test.cpp
)

if(SIMPLECPP_COMPILER MATCHES USING_MSVC)
    # Prevent MSVC from complaining about template usage
    target_compile_options(SimpleCPP-SimpleSTL-Test PRIVATE /bigobj)
endif()

link_simplecpp_test(SimpleSTL Test SimpleArchive)

add_simplecpp_test(SimpleSTL ReleaseTest
        TestShared.h
        ReleaseTest.cpp
)