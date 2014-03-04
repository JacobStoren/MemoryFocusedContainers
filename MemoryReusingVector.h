// A continous memory container that reuses slots that are deleted
// Copyright (C) 2014 Jens Jacob Støren
// Can be used under the GNU General Public License v3.0
// If that license does not fit, please contact the author.

#ifndef MEMORYREUSINGVECTOR_H
#define MEMORYREUSINGVECTOR_H

#include <vector>
#include <cstddef>

const size_t UNDEFINED_SIZE_T = (size_t)-1;

template < typename Object>
class MemoryReusingVector
{
public:
   MemoryReusingVector() : m_idxToNextFreeElement(UNDEFINED_SIZE_T) {}
   ~MemoryReusingVector();

   void     reserve(size_t size)       { m_elements.reserve(size); }

   size_t   create();
   void     erase(size_t idx);

   bool     isValid(size_t idx)        { return m_elements[idx].isValid; }
   Object*  operator[] (size_t idx)    { return this->get(idx); }
   Object*  get(size_t idx);

   size_t size()                       { return m_elements.size(); }

private:
   struct Element
   {
      union
      {
          char objectData[sizeof(Object)];
          size_t idxToNextFreeElement;
      };

      bool isValid;
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
size_t MemoryReusingVector<Object>::create()
{
   size_t newIdx = UNDEFINED_SIZE_T;

   if (m_idxToNextFreeElement == UNDEFINED_SIZE_T)
   {
      m_elements.push_back(Element());
      m_idxToNextFreeElement = m_elements.size()-1;
      m_elements[m_idxToNextFreeElement].isValid = false;
      m_elements[m_idxToNextFreeElement].idxToNextFreeElement = UNDEFINED_SIZE_T;
   }

   void * objMem = reinterpret_cast<void*>(&(m_elements[m_idxToNextFreeElement].objectData));
   m_elements[m_idxToNextFreeElement].isValid = true;
   newIdx = m_idxToNextFreeElement;
   m_idxToNextFreeElement = m_elements[m_idxToNextFreeElement].idxToNextFreeElement;

   // Use placement new to construct an object in the previously allocated memory

   new (objMem) Object;

   return newIdx;
}

template <typename Object>
void MemoryReusingVector<Object>::erase(size_t idx)
{
   if (!m_elements[idx].isValid)
   {
      return;
   }
   Object* objToDelete = reinterpret_cast<Object*>(&(m_elements[idx].objectData));
   objToDelete->~Object();

   m_elements[idx].isValid = false;
   m_elements[idx].idxToNextFreeElement = m_idxToNextFreeElement;
   m_idxToNextFreeElement = idx;
}

template <typename Object>
Object*  MemoryReusingVector<Object>::get(size_t idx)
{
   if (!m_elements[idx].isValid)
   {
      return NULL;
   }

   return reinterpret_cast<Object*>(&(m_elements[idx].objectData));
}

#endif // MEMORYREUSINGVECTOR_H
