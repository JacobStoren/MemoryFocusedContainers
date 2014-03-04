
#include "SmallVector.h"
#include "IndexableMemoryPool.h"
#include "MemoryReusingVector.h"

#include <iostream>

using namespace std;

int destructorCountA = 0;
int constructorCountA = 0;

//#pragma pack(1)
class A
{
public:
   A() :d(12.34567d), a(2222) { ++constructorCountA; }
   ~A() { ++destructorCountA; }
   double d;
   int a;
};
//#pragma pack()

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
      //cout << "--Block freed -----------" << endl;
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
      constructorCountA = 0;
      destructorCountA = 0;

      MemoryReusingVector<A> vec;
      vec.reserve(2);
      size_t idx1 = vec.create();
      size_t idx2 = vec.create();
      size_t idx3 = vec.create();

      EXPECT_EQ((size_t)0, idx1);
      EXPECT_EQ((size_t)1, idx2);
      EXPECT_EQ((size_t)2, idx3);

      vec.erase(idx1);
      vec.erase(idx2);

      EXPECT_FALSE(vec.isValid(0));
      EXPECT_FALSE(vec.isValid(1));
      EXPECT_TRUE (vec.isValid(2));

      idx1 = vec.create();
      idx2 = vec.create();
      idx3 = vec.create();

      EXPECT_EQ((size_t)1, idx1);
      EXPECT_EQ((size_t)0, idx2);
      EXPECT_EQ((size_t)3, idx3);

      for (size_t i = 0; i < vec.size(); ++i)
      {
         if (vec[i])
         {
            EXPECT_EQ(2222, vec[i]->a);
            EXPECT_DOUBLE_EQ(12.34567d, vec[i]->d);
         }
      }

   }

   EXPECT_EQ(constructorCountA, destructorCountA);
   EXPECT_EQ(6, constructorCountA);
}


TEST(MemoryTools, SmallVector1)
{
   {
      constructorCountA = 0;
      destructorCountA = 0;

      SmallVector<A, 1> ve;

      A a;
      ve.push_back(a);

      EXPECT_EQ(1, ve.size());
      for (size_t i = 0; i < ve.size(); ++i)
      {
         EXPECT_EQ(2222, ve[i].a);
         EXPECT_DOUBLE_EQ(12.34567d, ve[i].d);
      }

      ve.push_back(a);

      EXPECT_EQ(2, ve.size());
      for (size_t i = 0; i < ve.size(); ++i)
      {
         EXPECT_EQ(2222, ve[i].a);
         EXPECT_DOUBLE_EQ(12.34567d, ve[i].d);
      }

      ve.resize(5);

      EXPECT_EQ(5, ve.size());
      for (size_t i = 0; i < ve.size(); ++i)
      {
         EXPECT_EQ(2222, ve[i].a);
         EXPECT_DOUBLE_EQ(12.34567d, ve[i].d);
      }

      ve[2].d = 100000;

      EXPECT_EQ(100000, ve[2].d);
      EXPECT_EQ(6, constructorCountA);

      ve.resize(5);
      EXPECT_EQ(6, constructorCountA);

      ve.resize(3);
      EXPECT_EQ(6, constructorCountA);
      EXPECT_EQ(2, destructorCountA);

      ve.resize(8);
      EXPECT_EQ(11, constructorCountA);
      EXPECT_EQ(2, destructorCountA);

      for (size_t i = 0; i < ve.size(); ++i)
      {
         EXPECT_EQ(2222, ve[i].a);
         if (i == 2)
            EXPECT_DOUBLE_EQ(100000, ve[i].d);
         else
            EXPECT_DOUBLE_EQ(12.34567d, ve[i].d);
      }


   }

   EXPECT_EQ(constructorCountA, destructorCountA);
   EXPECT_EQ(11, constructorCountA);
}



TEST(MemoryTools, SmallVector3)
{
   {
      constructorCountA = 0;
      destructorCountA = 0;

      SmallVector<A, 3> ve;

      A a;
      ve.push_back(a);

      EXPECT_EQ(1, ve.size());
      for (size_t i = 0; i < ve.size(); ++i)
      {
         EXPECT_EQ(2222, ve[i].a);
         EXPECT_DOUBLE_EQ(12.34567d, ve[i].d);
      }

      ve.push_back(a);

      EXPECT_EQ(2, ve.size());
      for (size_t i = 0; i < ve.size(); ++i)
      {
         EXPECT_EQ(2222, ve[i].a);
         EXPECT_DOUBLE_EQ(12.34567d, ve[i].d);
      }
      ve.resize(1);
      EXPECT_EQ(1, destructorCountA);

      EXPECT_EQ(1, ve.size());
      for (size_t i = 0; i < ve.size(); ++i)
      {
         EXPECT_EQ(2222, ve[i].a);
         EXPECT_DOUBLE_EQ(12.34567d, ve[i].d);
      }

      ve.resize(5);

      EXPECT_EQ(5, ve.size());
      for (size_t i = 0; i < ve.size(); ++i)
      {
         EXPECT_EQ(2222, ve[i].a);
         EXPECT_DOUBLE_EQ(12.34567d, ve[i].d);
      }

      ve[2].d = 100000;

      EXPECT_EQ(100000, ve[2].d);
      EXPECT_EQ(7, constructorCountA);

      ve.resize(5);
      EXPECT_EQ(7, constructorCountA);

      ve.resize(3);
      EXPECT_EQ(7, constructorCountA);
      EXPECT_EQ(3, destructorCountA);

      ve.resize(8);
      EXPECT_EQ(12, constructorCountA);
      EXPECT_EQ(3, destructorCountA);

      for (size_t i = 0; i < ve.size(); ++i)
      {
         EXPECT_EQ(2222, ve[i].a);
         if (i == 2)
            EXPECT_DOUBLE_EQ(100000, ve[i].d);
         else
            EXPECT_DOUBLE_EQ(12.34567d, ve[i].d);
      }
   }

   EXPECT_EQ(constructorCountA, destructorCountA);
   EXPECT_EQ(12, constructorCountA);
}

struct B
{
   double d;
   int a;
};

class C
{
public:

   union{
      void * ptr;
      B hepp;
   };
   char byte;
};

//#pragma pack(1)
class D
{
   void*  ptr;
   char dir  : 1;
   char lIdx : 2;
};
//#pragma pack()

TEST(MemoryTools, StructSizes)
{
   cout << "SizeOf SmallVector<A, 1>: " << sizeof(SmallVector<A, 1>) << endl;
   cout << "SizeOf SmallVector<A, 3>: " << sizeof(SmallVector<A, 3>) << endl;
   cout << "SizeOf double: " << sizeof(double) << endl;
   cout << "SizeOf int:    " << sizeof(int) << endl;
   cout << "SizeOf A: " << sizeof(A) << endl;
   cout << "SizeOf B: " << sizeof(B) << endl;
   cout << "SizeOf C: " << sizeof(C) << endl;
   cout << "SizeOf D: " << sizeof(D) << endl;
   cout << "SizeOf SmallVector<D, 1>: " << sizeof(SmallVector<D, 1>) << endl;
}
