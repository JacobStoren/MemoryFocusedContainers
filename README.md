IndexableMemoryPool
===================

A memory pool implementation with deallocation and index based access to the managed objects.
It might also be called a kind of dynamic array. 

Compared to a normal Memory Pool, it adds:

1. Deallocation of the memory blocks at the end when they are empty. 
2. Index based random access for parallell processing.

The point in using this instad of a plain std::vector etc is that pointers to the objects stored here is *not* invalidated as the pool grows or shrinks.

### Limitations

1. Currently only objects with a default constructor is supported.
2. Certainly something more ...

