
#include <gtest/gtest.h>

#include "operators/adder/adder.h"

TEST(Adder, Normal) {

	Adder a;

	// two plus two equals five
	EXPECT_EQ(5, a.calc(2, 2));
}
