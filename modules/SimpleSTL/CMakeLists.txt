create_simplecpp_module(SimpleSTL INTERFACE
        # Base Classes
        include/sstl/Container.h

        # Sequence Containers
        include/sstl/Vector.h
        include/sstl/Deque.h
        include/sstl/List.h
        include/sstl/ForwardList.h
        include/sstl/Array.h

        # Sorted Sequence Containers
        include/sstl/MinHeap.h
        include/sstl/MaxHeap.h

        # Container Adaptors
        include/sstl/Queue.h
        include/sstl/Stack.h

        # Associative Containers
        include/sstl/Set.h
        include/sstl/MultiSet.h
        include/sstl/Map.h
        include/sstl/MultiMap.h

        # Sorted Associative Containers
        include/sstl/PrioritySet.h
        include/sstl/PriorityMultiSet.h
        include/sstl/PriorityMap.h
        include/sstl/PriorityMultiMap.h
)

link_simplecpp_module(SimpleSTL INTERFACE SimpleUtils)
