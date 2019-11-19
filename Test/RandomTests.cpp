#include "gtest/gtest.h"
#include "Random.h"

TEST(RandomTests, RandomInRange) {
	int random1 = Random::RandInt(0, 1 << 30);
	int random2 = Random::RandInt(0, 1 << 30);
	ASSERT_NE(random1, random2);
	if (random1 > random2) std::swap(random1, random2);
	int random3 = Random::RandInt(random1, random2);
	ASSERT_GE(random3, random1);
	ASSERT_LE(random3, random2);
}

TEST(RandomTests, SeedWorks) {
	Random::SetSeed(0);
	ASSERT_EQ(2531011, Random::RandInt(0, 1 << 30));
	ASSERT_EQ(505908858, Random::RandInt(0, 1 << 30));
	ASSERT_EQ(318135124, Random::RandInt(0, 1 << 30));
	ASSERT_EQ(159719620, Random::RandInt(0, 1 << 30));
	Random::SetSeed(0);
	ASSERT_EQ(2531011, Random::RandInt(0, 1 << 30));
}

TEST(RandomTests, SeedChangesInitialValue) {
	Random::SetSeed(0);
	int random1 = Random::RandInt(0, 1 << 30);
	Random::SetSeed(1);
	int random2 = Random::RandInt(0, 1 << 30);
	ASSERT_NE(random1, random2);

	Random::SetSeed(2);
	int random3 = Random::RandInt(0, 1 << 30);
	ASSERT_NE(random3, random1);
	ASSERT_NE(random3, random2);
}

TEST(RandomTests, EvenOdd) {
    std::vector<int> outputs;
    for (int i=0; i<100; i++) {
        outputs.emplace_back(Random::RandInt(0, 1));
    }
    int k = 0;
}