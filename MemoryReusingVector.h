#ifndef MEMORYREUSINGVECTOR_H
#define MEMORYREUSINGVECTOR_H

#include <vector>
#include <cstddef>

const size_t UNDEFINED_SIZE_T = (size_t)-1;

#if 1

#endif


template < typename Object>
class MemoryReusingVector
{
public:
   MemoryReusingVector() : m_idxToNextFreeElement(UNDEFINED_SIZE_T) {}
   ~MemoryReusingVector();

   void     reserve(size_t size)       { m_elements.reserve(size); }

   class GuardedIndex
   {
      size_t index : 41;
      size_t reAllocCount: 23;
      friend typename MemoryReusingVector<Object>;
   };

   GuardedIndex   create();
   void     erase(GuardedIndex idx);

   bool     isValid(GuardedIndex idx)  { return m_elements[idx].isValid; }
   Object*  operator[] (size_t idx)
   {
      if (!m_elements[idx].isValid)
      {
         return NULL;
      }

      return reinterpret_cast<Object*>(&(m_elements[idx].objectData));
   }

   Object*  get(GuardedIndex idx);

   size_t size()                       { return m_elements.size(); }

private:
   struct Element
   {
      union
      {
          char objectData[sizeof(Object)];
          size_t idxToNextFreeElement;
      };

      unsigned int reAllocCount : 23;
      bool isValid : 1;
   };

   size_t m_idxToNextFreeElement;
   std::vector<Element> m_elements;
};

template <typename Object>
MemoryReusingVector<Object>::~MemoryReusingVector()
{
   for (size_t i = 0; i < m_elements.size(); ++i)
   {
      if (m_elements[i].isValid)
      {
         Object* objToDelete = reinterpret_cast<Object*>(&(m_elements[i].objectData));
         objToDelete->~Object();
      }
   }
}

template <typename Object>
MemoryReusingVector<Object>::GuardedIndex MemoryReusingVector<Object>::create()
{
   GuardedIndex newIdx;

   if (m_idxToNextFreeElement == UNDEFINED_SIZE_T)
   {
      m_elements.push_back(Element());
      m_idxToNextFreeElement = m_elements.size()-1;
      m_elements[m_idxToNextFreeElement].isValid = false;
      m_elements[m_idxToNextFreeElement].idxToNextFreeElement = UNDEFINED_SIZE_T;
   }

   void * objMem = reinterpret_cast<void*>(&(m_elements[m_idxToNextFreeElement].objectData));
   m_elements[m_idxToNextFreeElement].isValid = true;
   newIdx.index = m_idxToNextFreeElement;
   newIdx.reAllocCount = m_elements[m_idxToNextFreeElement].reAllocCount;
   m_idxToNextFreeElement = m_elements[m_idxToNextFreeElement].idxToNextFreeElement;

   // Use placement new to construct an object in the previously allocated memory

   new (objMem) Object;

   return newIdx;
}

template <typename Object>
void MemoryReusingVector<Object>::erase(MemoryReusingVector<Object>::GuardedIndex idx)
{
   if (!m_elements[idx.index].isValid || m_elements[idx.index].reAllocCount != idx.reAllocCount)
   {
      return;
   }

   Object* objToDelete = reinterpret_cast<Object*>(&(m_elements[idx].objectData));
   objToDelete->~Object();

   m_elements[idx.index].isValid = false;
   m_elements[idx.index].idxToNextFreeElement = m_idxToNextFreeElement;
   ++(m_elements[idx.index].reAllocCount);

   m_idxToNextFreeElement = idx.index;
}

template <typename Object>
Object*  MemoryReusingVector<Object>::get(GuardedIndex idx)
{
   if (!m_elements[idx].isValid || m_elements[idx.index].reAllocCount != idx.reAllocCount)
   {
      return NULL;
   }

   return reinterpret_cast<Object*>(&(m_elements[idx.index].objectData));
}

#endif // MEMORYREUSINGVECTOR_H
