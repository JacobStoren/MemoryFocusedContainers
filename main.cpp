
#include <iostream>

#include "gtest/gtest.h"
#include <ctime>

int main(int argc, char **argv) {

  while (clock() < 1000) {}

  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
