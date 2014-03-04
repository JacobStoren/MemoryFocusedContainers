// A continous memory container that reuses slots that are deleted
// Copyright (C) 2014 Jens Jacob Storen
// Can be used under the GNU General Public License v3.0
// If that license does not fit, please contact the author.

#ifndef MEMORYREUSINGVECTORGI_H
#define MEMORYREUSINGVECTORGI_H

#include <vector>
#include <cstddef>

//const size_t UNDEFINED_SIZE_T = (size_t)-1;

template < typename Object>
class MemoryReusingVectorGI
{
public:
   MemoryReusingVectorGI() : m_idxToNextFreeElement(UNDEFINED_SIZE_T) {}
   ~MemoryReusingVectorGI();

   void     reserve(size_t size)       { m_elements.reserve(size); }

   class GuardedIndex
   {
      unsigned int index;
      unsigned int reAllocCount : 31;
      friend class MemoryReusingVectorGI<Object>;
   };

   GuardedIndex   create();
   void     erase(GuardedIndex idx);

   bool     isValid(size_t idx)  { return m_elements[idx].isValid; }
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

      unsigned int reAllocCount : 31;
      unsigned int isValid : 1;
   };

   size_t m_idxToNextFreeElement;
   std::vector<Element> m_elements;
};

template <typename Object>
MemoryReusingVectorGI<Object>::~MemoryReusingVectorGI()
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
typename MemoryReusingVectorGI<Object>::GuardedIndex MemoryReusingVectorGI<Object>::create()
{
   GuardedIndex newIdx;

   if (m_idxToNextFreeElement == UNDEFINED_SIZE_T)
   {
      m_elements.push_back(Element());
      m_idxToNextFreeElement = m_elements.size()-1;
      m_elements[m_idxToNextFreeElement].isValid = false;
      m_elements[m_idxToNextFreeElement].idxToNextFreeElement = UNDEFINED_SIZE_T;
      m_elements[m_idxToNextFreeElement].reAllocCount = 0;
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
void MemoryReusingVectorGI<Object>::erase(MemoryReusingVectorGI<Object>::GuardedIndex idx)
{
   if (!m_elements[idx.index].isValid || m_elements[idx.index].reAllocCount != idx.reAllocCount)
   {
      return;
   }

   Object* objToDelete = reinterpret_cast<Object*>(&(m_elements[idx.index].objectData));
   objToDelete->~Object();

   m_elements[idx.index].isValid = false;
   m_elements[idx.index].idxToNextFreeElement = m_idxToNextFreeElement;
   ++(m_elements[idx.index].reAllocCount);

   m_idxToNextFreeElement = idx.index;
}

template <typename Object>
Object*  MemoryReusingVectorGI<Object>::get(GuardedIndex idx)
{
   if (!m_elements[idx.index].isValid || m_elements[idx.index].reAllocCount != idx.reAllocCount)
   {
      return NULL;
   }

   return reinterpret_cast<Object*>(&(m_elements[idx.index].objectData));
}

#endif // MEMORYREUSINGVECTORGI_H
