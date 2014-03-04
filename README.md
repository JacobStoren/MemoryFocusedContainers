MemoryFocusedContainers
=======================

This library consists of some specialized containers addressing different 
aspects and needs with respect to memory utilization.

## IndexableMemoryPool

A memory pool implementation with deallocation and index based access to the managed objects.
It might also be called a block based dynamic array. 

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

## MemoryReusingVector

This is a dynamic vector that reuses the space of elements that are erased. 

It is based on std::vector and shares a lot of its properties.


## SmallVector

The SmallVector is a tool to create dynamic arrays with as small as possible memory footprint.
It is sensible to use this if you want a dynamic array as a member of a class, but the array max size needed is quite small.

It is a template allowing you to specify how many elements that will be embedded in the object itself, making it possible to avoid heap allocations for the array data as long as its size is below that embedded size.

This makes sense when you expect the array to  be of a certain size "normally", but might be bigger in certain seldom  situations.  

This is the case in some geometric relations. A triangle normally has one triangle as neighbour for an edge, but might have several.


## Library Status

It is quite sound, with fairly good unit tests. 
I have, however, not used other compilers than the mingw compiler yet.
