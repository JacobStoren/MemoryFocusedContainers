// Iterator preserving, parallell access, contingous memory dynamic storage container
// Copyright (C) 2014 Jens Jacob Støren
// Can be used under the GNU General Public License v3.0
// If that license does not fit, please contact the author.

#ifndef INDEXABLEMEMORYPOOL_H
#define INDEXABLEMEMORYPOOL_H

#include <vector>
#include <cstddef>
#include <iostream>

template <typename Object, size_t blockSize>
class IndexableMemoryPool
{
public:

   IndexableMemoryPool() : m_size(0), m_validObjectCount(0), m_nextFreeElement(NULL) {}
   ~IndexableMemoryPool();

   Object*  create();
   void     erase(Object * objToDelete);

   Object*  get(size_t idx);
   Object*  operator[] (size_t idx)    { return this->get(idx); }
   size_t   size() const               { return m_size;         }

   size_t memoryBlockCount() const     { return m_elementMemoryBlocks.size(); }
   size_t validObjectCount() const     { return m_validObjectCount; }

private:
   class Element;

   size_t                  m_size;
   size_t                  m_validObjectCount;
   Element*                m_nextFreeElement;
   std::vector<Element*>   m_elementMemoryBlocks;

   union ObjectUnion
   {
      char        objectData[sizeof(Object)];
      Element*    nextFreeElement;
   };

   struct Element
   {
      ObjectUnion objectMemory;
      bool        isValid;
   };

   size_t findIndexOfFirstEmptyElementMemoryBlock()
   {
      size_t idxOfFirstEmptyBlock = 0;

      if (m_size > 0)
      {
         size_t lastUsedBlockIdx = (m_size-1)/blockSize;
         idxOfFirstEmptyBlock = lastUsedBlockIdx + 1;
      }

      return idxOfFirstEmptyBlock;
   }

   void freeUnusedElementMemoryBlocksAtEnd(size_t idxOfFirstBlockToFree)
   {
      if (idxOfFirstBlockToFree < m_elementMemoryBlocks.size())
      {
         if (idxOfFirstBlockToFree > 0)
         {
            Element* freeElement = NULL;

            for (size_t bIdx = idxOfFirstBlockToFree; bIdx > 0 ; --bIdx)
            {
               for (size_t eIdx =  blockSize; eIdx > 0 ; --eIdx)
               {
                  Element* currentElement = &(m_elementMemoryBlocks[bIdx-1][eIdx-1]);
                  if (!currentElement->isValid)
                  {
                     currentElement->objectMemory.nextFreeElement = freeElement;
                     freeElement = currentElement;
                  }
               }
            }

            m_nextFreeElement = freeElement;
         }
         else
         {
            m_nextFreeElement = NULL;
         }

         for (size_t blockIdx = idxOfFirstBlockToFree; blockIdx < m_elementMemoryBlocks.size(); ++blockIdx)
         {
            this->freeElementMemoryBlock(m_elementMemoryBlocks[blockIdx]);
            m_elementMemoryBlocks[blockIdx] = NULL;
         }

         m_elementMemoryBlocks.resize(idxOfFirstBlockToFree);
      }
   }

   static void freeElementMemoryBlock(Element* elementMemoryBlock)
   {
      void * pureMemoryBlock = reinterpret_cast<void*>(elementMemoryBlock);
      ::operator delete(pureMemoryBlock);
   }

   void makeSurePoolMemoryIsAvailable()
   {
      if (!m_nextFreeElement)
      {
         Element* elementMemoryBlock = this->allocateElementMemoryBlock();
         initializeElementMemoryBlock(elementMemoryBlock);
         registerElementMemoryBlock(elementMemoryBlock);
      }
   }

   static Element* allocateElementMemoryBlock()
   {
      void * pureMemoryBlock = ::operator new (sizeof(Element) * blockSize);
      Element* elementMemoryBlock = reinterpret_cast<Element*>(pureMemoryBlock);

      return elementMemoryBlock;
   }

   static void initializeElementMemoryBlock(Element* elementMemoryBlock)
   {
      for (size_t i = 0; i < blockSize-1 ; ++i)
      {
         elementMemoryBlock[i].objectMemory.nextFreeElement = &elementMemoryBlock[i + 1];
         elementMemoryBlock[i].isValid = false;
      }

      elementMemoryBlock[blockSize - 1].objectMemory.nextFreeElement = NULL;
      elementMemoryBlock[blockSize - 1].isValid = false;
   }

   void registerElementMemoryBlock(Element* elementMemoryBlock)
   {
      m_elementMemoryBlocks.push_back(elementMemoryBlock);
      m_nextFreeElement = elementMemoryBlock;
   }

};


template <typename Object, size_t blockSize>
IndexableMemoryPool<Object, blockSize>::~IndexableMemoryPool()
{
   for (size_t blockIdx = 0; blockIdx < m_elementMemoryBlocks.size(); ++blockIdx)
   {
      for (size_t idxInBlock = 0; idxInBlock < blockSize; ++idxInBlock)
      {
         if (m_elementMemoryBlocks[blockIdx][idxInBlock].isValid)
         {
            Object * objToDelete = reinterpret_cast<Object*>(&(m_elementMemoryBlocks[blockIdx][idxInBlock].objectMemory.objectData));
            objToDelete->~Object();
         }
      }

      freeElementMemoryBlock(m_elementMemoryBlocks[blockIdx]);
      m_elementMemoryBlocks[blockIdx] = NULL;
   }
}

template <typename Object, size_t blockSize>
Object* IndexableMemoryPool<Object, blockSize>::create()
{
   Object* newObject = NULL;

   this->makeSurePoolMemoryIsAvailable();

   void * objMem = reinterpret_cast<void*>(&(m_nextFreeElement->objectMemory.objectData));
   m_nextFreeElement->isValid = true;
   m_nextFreeElement = m_nextFreeElement->objectMemory.nextFreeElement;

   // Use placement new to construct an object in the previously allocated memory

   newObject = new (objMem) Object;


   // Increment size

   if (this->get(m_size) == newObject)
   {
      ++m_size;
   }

   ++m_validObjectCount;

   return newObject;
}


template <typename Object, size_t blockSize>
void IndexableMemoryPool<Object, blockSize>::erase(Object * objToDelete)
{
   if (!objToDelete)
   {
      return;
   }

   objToDelete->~Object(); // Run destructor code for objToDelete

   Element* objToDeleteElement =  reinterpret_cast<Element*> (objToDelete);

   objToDeleteElement->isValid = false;
   objToDeleteElement->objectMemory.nextFreeElement = m_nextFreeElement;
   m_nextFreeElement = objToDeleteElement;

   // Decrement m_size

   while (m_size && this->get(m_size-1) == NULL)
   {
      --m_size;
   }

   --m_validObjectCount;

   // Deallocate unused blocks

   size_t idxOfFirstBlockToFree = this->findIndexOfFirstEmptyElementMemoryBlock();

   this->freeUnusedElementMemoryBlocksAtEnd (idxOfFirstBlockToFree);
}

template <typename Object, size_t blockSize>
Object* IndexableMemoryPool<Object, blockSize>::get(size_t idx)
{
   size_t blockIdx = idx/blockSize;
   size_t idxInBlock = idx%blockSize;

   if (blockIdx < m_elementMemoryBlocks.size() && m_elementMemoryBlocks[blockIdx][idxInBlock].isValid)
   {
      return reinterpret_cast<Object*>(&(m_elementMemoryBlocks[blockIdx][idxInBlock].objectMemory.objectData));
   }
   else
   {
      return NULL;
   }
}

#endif // INDEXABLEMEMORYPOOL_H
