
#include <gtest/gtest.h>

#include "add.h"

TEST(Add, Normal) {
  EXPECT_EQ(3, add(1, 2));
}

