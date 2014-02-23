#include "IndexableMemoryPool.h"

#include <iostream>

using namespace std;

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

