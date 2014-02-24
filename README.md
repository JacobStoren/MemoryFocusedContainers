IndexableMemoryPool
===================

A memory pool implementation with deallocation and index based access to the managed objects.
It might also be called a kind of dynamic array. 

Compared to a normal Memory Pool, it adds:

1. Deallocation of the memory blocks at the end when they are empty. 
2. Index based random access for parallell processing.

The point in using this instad of a plain std::vector etc is that pointers to the objects stored here is *not* invalidated as the pool grows or shrinks.
### Usage example

    IndexableMemoryPool<SomeClass, 100> SomeClassPool;
    
    SomeClass* a = SomeClassPool.create();
 
    for (size_t i = 0; i < SomeClassPool.size(); ++i)
    {
       SomeClass* obj = SomeClassPool[i];
       if (obj)
       {
          obj->doStuff();
       }
    }
    
    SomeClassPool.erase(a);

The destructor of the IndexableMemoryPool will run the destructor on all the objects it owns, and free all memory it has allocated.

### Limitations

1. Currently only objects with a default constructor is supported.
2. Certainly something more ...

### Status

The implementation is complete, and it seems to work as intended, but it is not completely covered by tests yet.
It has not been run through a memory analyser an could thus contain memory leaks etc.

