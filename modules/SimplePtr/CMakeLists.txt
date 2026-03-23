create_simplecpp_module(SimplePtr INTERFACE
        include/sptr/Memory.h
        include/sptr/Shared.h
        include/sptr/Unique.h
)

link_simplecpp_module(SimplePtr INTERFACE SimpleUtils)

# Automatically call init on a type when created
simplecpp_option(SimplePtr SIMPLEPTR_INIT "Enable the auto init ability" On)

# Automatically call a destroy function on the type when destroyed, release will overwrite this behavior
simplecpp_option(SimplePtr SIMPLEPTR_DESTROY "Enable the auto destroy ability" On)

# Initially disabled unless for testing, Release can be dangerous if used unknowingly, should only be enabled if you know what it does
# Allows you to specify a release function in the memory's types, which removes all deletion capability from the pointer,
# giving the ability to self-release the memory.  Can cause memory leaks if used improperly.
simplecpp_option(SimplePtr SIMPLEPTR_RELEASE "Enable the release auto function ability" ${SIMPLECPP_TEST})