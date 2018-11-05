#include "gtest/gtest.h"
#include "Random.h"

TEST(Foo, Bar) {
	int random1 = Random::RandInt(0, 1 << 30);
	int random2 = Random::RandInt(0, 1 << 30);
	ASSERT_NE(random1, random2);
	int random3 = Random::RandInt(random1, random2);
	ASSERT_GE(random3, random1);
	ASSERT_LT(random3, random2);
}
