
#include "SmallVector.h"
#include "IndexableMemoryPool.h"
#include "MemoryReusingVector.h"

#include <iostream>

using namespace std;

int destructorCountA = 0;
int constructorCountA = 0;

class A
{
public:
   A() :d(12.34567d), a(2222) { ++constructorCountA; }
   ~A() { ++destructorCountA; }
   double d;
   int a;
};

struct C
{
   double d;
   int a;
};
int destructorCountB = 0;
int constructorCountB = 0;

class B
{
public:
   B() { ++constructorCountB; }
   ~B() { ++destructorCountB; }
  // double d;
  // int a;

   char byte;
   union{
      void * ptr;
      C hepp;
   };

};

#include "gtest/gtest.h"

TEST(MemoryTools, IndexableMemoryPool)
{
   {
      IndexableMemoryPool<A, 100> APool;
      A* a = APool.create();
      A* a2 = APool.create();
      A* a3 = APool.create();

      EXPECT_EQ(2222, a3->a);
      EXPECT_DOUBLE_EQ(12.34567d, a3->d);

      APool.erase(a);

      EXPECT_EQ(NULL, APool[0]);
      EXPECT_EQ(a2, APool[1]);
      EXPECT_EQ(a3, APool[2]);

      EXPECT_EQ((size_t)3, APool.size());
      EXPECT_EQ((size_t)1, APool.memoryBlockCount());
      EXPECT_EQ((size_t)2, APool.validObjectCount());

      APool.erase(a3);
      EXPECT_EQ((size_t)2, APool.size());
      EXPECT_EQ((size_t)1, APool.memoryBlockCount());
      EXPECT_EQ((size_t)1, APool.validObjectCount());

      APool.erase(a2);

      EXPECT_EQ((size_t)0, APool.size());
      EXPECT_EQ((size_t)0, APool.memoryBlockCount());
      EXPECT_EQ((size_t)0, APool.validObjectCount());

      for (size_t i = 0; i < 200; ++i)
      {
         APool.create();
      }

      EXPECT_EQ(2222, APool[199]->a);
      EXPECT_DOUBLE_EQ(12.34567d, APool[199]->d);

      a = APool.create();
      a2 = APool.create();
      a3 = APool.create();
      APool.erase(a);

      EXPECT_EQ(NULL, APool[200]);
      EXPECT_EQ(a2, APool[201]);
      EXPECT_EQ(a3, APool[202]);

      EXPECT_EQ((size_t)203, APool.size());
      EXPECT_EQ((size_t)3, APool.memoryBlockCount());
      EXPECT_EQ((size_t)202, APool.validObjectCount());

      APool.erase(a3);
      EXPECT_EQ((size_t)202, APool.size());
      EXPECT_EQ((size_t)3, APool.memoryBlockCount());
      EXPECT_EQ((size_t)201, APool.validObjectCount());

      APool.erase(APool[0]);
      APool.erase(APool[99]);
      APool.erase(APool[50]);
      APool.erase(APool[120]);

      EXPECT_EQ((size_t)202, APool.size());
      EXPECT_EQ((size_t)3, APool.memoryBlockCount());
      EXPECT_EQ((size_t)197, APool.validObjectCount());
      APool.erase(a2);

      EXPECT_EQ((size_t)200, APool.size());
      EXPECT_EQ((size_t)2, APool.memoryBlockCount());
      EXPECT_EQ((size_t)196, APool.validObjectCount());
      cout << "--Block freed -----------" << endl;
      EXPECT_EQ(2222, APool[199]->a);
      EXPECT_DOUBLE_EQ(12.34567d, APool[199]->d);

      a = APool.create();
      a2 = APool.create();
      a3 = APool.create();
      A* a4 = APool.create();

      EXPECT_EQ(a, APool[0]);
      EXPECT_EQ(a2, APool[50]);
      EXPECT_EQ(a3, APool[99]);
      EXPECT_EQ(a4, APool[120]);

      for (size_t i = 0; i < APool.size(); ++i)
      {
         //cout << "[" << i << "]" << endl;
         EXPECT_EQ(2222, APool[i]->a);
         EXPECT_DOUBLE_EQ(12.34567d, APool[i]->d);
      }
   }

   EXPECT_EQ(constructorCountA, destructorCountA);
   EXPECT_EQ(210, constructorCountA);
}

TEST(MemoryTools, MemoryReusingVector)
{
   {
      cout << endl << "MemoryReusingVector" << endl << endl;

      MemoryReusingVector<A> vec;
      size_t idx1 = vec.create();
      size_t idx2 = vec.create();
      size_t idx3 = vec.create();

      cout << idx1 << " " << idx2 << " " << idx3 << " " <<endl;

      vec.erase(idx2);
      vec.erase(idx1);

      for (size_t i = 0; i < vec.size(); ++i)
      {
         cout << "[" << i << "] " << vec.isValid(i) << endl;
      }

      idx1 = vec.create();
      idx2 = vec.create();

      cout << idx1 << " " << idx2 << " " << idx3 << " " <<endl;
   }

   cout << endl << "SmallVector" << endl<< endl;

   SmallVector<A, 1, 5> ve;
   ve.resize(5);
   cout << "Size: " << (unsigned short)ve.size() << endl;
   ve[2].d = 100000;
   cout << ve[2].d << endl;
   cout << ve[3].d << endl;

   cout << "SizeOf SmallVec: " << sizeof(SmallVector<A, 1, 5>) << endl;
   cout << "SizeOf double: " << sizeof(double) << endl;
   cout << "SizeOf int: " << sizeof(int) << endl;
   cout << "SizeOf A: " << sizeof(A) << endl;
   cout << "SizeOf B: " << sizeof(B) << endl;
   cout << "SizeOf C: " << sizeof(C) << endl;

}



