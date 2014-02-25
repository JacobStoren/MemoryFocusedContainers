#include "IndexableMemoryPool.h"

#include <iostream>

using namespace std;

const size_t UNDEFINED_SIZE_T = (size_t)-1;

#if 0
template<Object>
class IdxHandle
{

   // Needs static access to MemVector ...

   Object* operator ->() { }
   bool isNull();

private:
   size_t idx;
   size_t reAllocCount;
   template<Object> MemoryReusingVector* m_dataArray;
}
#endif
#if 0
template < typename Object>
class MemoryReusingVector
{
   MemoryReusingVector() : nextFreeElement(UNDEFINED_SIZE_T) {}
   void reserve(size_t size)
   {

   }

   size_t create()
   {
      size_t newIdx = UNDEFINED_SIZE_T;

      if (nextFreeElement == UNDEFINED_SIZE_T)
      {
         m_elements.push_back(Element());
         nextFreeElement = m_elements.size()-1;
      }

      // Use placement new to construct an object in the previously allocated memory

      void * objMem = reinterpret_cast<void*>(&(m_elements[m_nextFreeElement].object));
      new (objMem) Object;

      m_elements[m_nextFreeElement].isValid = true;
      if ()
      m_nextFreeElement = m_nextFreeElement->objectMemory.nextFreeElement;



   }
   bool isValid(size_t idx);
   Object& get(size_t idx);
   void erase(size_t idx);

private:
   class Element
   {
      union
      {
          char object[sizeof(Object)];
          size_t nextFreeElement;
      };

      bool isValid;
   };

   size_t nextFreeElement;

   std::vector<Element> m_elements;
};
  #endif

class A
{
public:
   A()  { cout << "A::A()" << endl; }
   ~A() { cout << "A::~A()" << endl; }
   double d;
   int a;
};


int main()
{
   cout << "Hello World!" << endl;
   IndexableMemoryPool<A, 100> APool;
   A* a = APool.create();
   A* a2 = APool.create();
   A* a3 = APool.create();
   APool.erase(a);

   for (size_t i = 0; i < APool.size(); ++i)
   {
      cout << "[" << i << "] " << APool[i] << endl;
   }
   APool.erase(a2);
   APool.erase(a3);

   cout << "PoolBlocks : " << APool.memoryBlockCount() << endl;

   for (size_t i = 0; i < 203; ++i)
   {
      APool.create();
   }

   for (size_t i = 0; i < APool.size(); ++i)
   {
      cout << "[" << i << "] " << APool[i] << endl;
   }

   cout << "PoolBlocks : " << APool.memoryBlockCount() << endl;
   APool.erase(APool[199]);
   APool.erase(APool[200]);
   APool.erase(APool[201]);
   APool.erase(APool[202]);
   cout << "PoolBlocks : " << APool.memoryBlockCount() << endl;

   for (size_t i = 0; i < APool.size(); ++i)
   {
      cout << "[" << i << "] " << APool[i] << endl;
   }


   return 0;
}

