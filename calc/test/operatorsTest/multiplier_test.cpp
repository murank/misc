

#include <gtest/gtest.h>

#include "operators/multiplier.h"

TEST(Multiplier, Normal) {

	Multiplier m;

	// 6*9=42
	EXPECT_EQ(42, m.calc(6, 9));
}

