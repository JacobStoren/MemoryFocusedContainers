// A dynamic vector optimized for small elements and size
// Copyright (C) 2014 Jens Jacob Støren
// Can be used under the GNU General Public License v3.0
// If that license does not fit, please contact the author.

#ifndef SMALLVECTOR_H
#define SMALLVECTOR_H

#include <cstring>

// #pragma pack(1) // Use to minimize memory fotprint at the cost of some speed

template <typename Element, unsigned char embeddedElementCount>
class SmallVector
{
public:
   SmallVector() : m_elmArray(0), m_size(0) {}
   ~SmallVector()
   {
      resize(0);
   }

   void resize(unsigned char newSize)
   {
      if (newSize == m_size) return;

      runDestructorForUnnecessaryObjects(newSize);

      void * newData = allocateNewData(newSize);

      copyOldDataIntoNewData(newData, newSize);

      runConstructorsForNewElements(newData, newSize);

      freeOldDataAndUseNew(newData, newSize);
   }


   Element& operator[] (unsigned char idx)     { if ( m_size <= embeddedElementCount ) { return (reinterpret_cast<Element*>(m_elmData))[static_cast<unsigned short> (idx)]; } else {return m_elmArray[static_cast<unsigned short> (idx)]; } }
   void push_back(const Element& elm) { unsigned char newIdx = m_size; this->resize(m_size + 1); (*this)[newIdx] = elm; }
   unsigned char size()                        { return m_size; }
   void clear() { resize(0); }

private:

   void runDestructorForUnnecessaryObjects(unsigned char newSize)
   {
      if (newSize >= m_size) return;

      Element* objectsToDelete = NULL;

      if (embeddedElementCount < m_size )
      {
         objectsToDelete = m_elmArray;
      }
      else
      {
         objectsToDelete = reinterpret_cast<Element*> (m_elmData);
      }

      for (int i = newSize; i < m_size; ++i)
      {
         (&(objectsToDelete[i]))->~Element();
      }
   }

   void * allocateNewData(unsigned char newSize)
   {
      if (newSize == 0) return NULL;

      void * newData = NULL;

      if (newSize > embeddedElementCount)
      {
         newData = ::operator new (newSize * sizeof(Element));
      }
      else
      {
         newData = reinterpret_cast<void*>(m_elmData);
      }

      return newData;
   }

   void copyOldDataIntoNewData(void * newData, unsigned char newSize)
   {
      if (m_size == 0 || newSize == 0) return;

      size_t minElmCount = newSize > m_size ? m_size: newSize;

      if (m_size > embeddedElementCount)
      {
         memcpy( newData, m_elmArray, minElmCount*sizeof(Element));
      }
      else if (newSize > embeddedElementCount)
      {
         memcpy( newData,  m_elmData, minElmCount*sizeof(Element));
      }
   }

   void runConstructorsForNewElements(void * newData, unsigned char newSize)
   {
      Element* newElements = reinterpret_cast<Element*> (newData);

      for (int i = m_size; i < newSize; ++i)
      {
         new (&(newElements[i])) Element;
      }
   }

   void freeOldDataAndUseNew(void * newData, unsigned char newSize)
   {
      if (m_size > embeddedElementCount)
      {
         ::operator delete (reinterpret_cast<void*>(m_elmArray));
      }

      if (newSize > embeddedElementCount)
      {
         m_elmArray = reinterpret_cast<Element*> (newData);
      }

      m_size = newSize;
   }

   union {
      Element* m_elmArray;
      char     m_elmData[embeddedElementCount*sizeof(Element)];
   };
   unsigned char m_size;

};
#pragma pack()
#endif // SMALLVECTOR_H
