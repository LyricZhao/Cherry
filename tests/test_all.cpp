#include <cmath>
#include <iostream>

#include "cherry.hpp"
#include "gtest/gtest.h"


/// Test whether GoogleTest works
TEST(Cherry, gtest) {
    int a = 1, b = 2;
    ASSERT_EQ(a + b, 3);
}


/// Test `Timer`
TEST(Cherry, Timer) {
    // Use two timer to check whether the intervals are similar
    Timer timer, wall_timer;
    uint64_t total_time = 0;
    for (int i = 0; i < 1000; ++ i) {
        total_time += timer.tik();
    }
    ASSERT_GT(total_time, 0);
    ASSERT_NEAR(total_time, wall_timer.tik(), total_time * 0.2);
}


/// Test `Random`
TEST(Cherry, Random) {
    // Random integer
    auto random_int = Random(10, 100);
    for (int i = 0; i < 100; ++ i) {
        ASSERT_GE(random_int(), 10);
        ASSERT_LE(random_int(), 100);
    }

    // Random real number
    auto random_double = Random<double>(0.0, 10.0);
    for (int i = 0; i < 100; ++ i) {
        ASSERT_GE(random_double(), 0.0);
        ASSERT_LE(random_double(), 10.0);
    }
}


/// Test `Random`
TEST(Cherry, ShiftRange) {
    // Shift a vector and change value
    int count = 0;
    std::vector<int> vec(10, 0);
    for (auto &value: shift(vec, 5)) {
        value = 1;
        ++ count;
    }
    ASSERT_EQ(count, 5);

    // Check value
    for (auto &value: shift(vec, 0, 5)) {
        ASSERT_EQ(value, 0);
    }
    for (auto &value: shift(vec, 5)) {
        ASSERT_EQ(value, 1);
    }
}


/// Test `ReversedRange`
TEST(Cherry, ReversedRange) {
    // Assign and check values
    int index = 0;
    std::vector<int> vec(10);
    for (auto &value: vec) {
        value = index ++;
    }
    for (auto &value: reversed(vec)) {
        ASSERT_EQ(value, -- index);
    }
}


/// Test `IndexingRange`
TEST(Cherry, IndexingRange) {
    // Assign and check values
    int index = 5;
    std::vector<int> indexes(5);
    for (auto &value: indexes) {
        value = -- index;
    }
    std::vector<int> values(10);
    for (auto &value: values) {
        value = index ++;
    }
    index = 5;
    for (auto &value: indexing(values, indexes)) {
        ASSERT_EQ(value, -- index);
    }
}


/// Test `JoinedIterator`
TEST(Cherry, JoinedIterator) {
    std::vector<int> vec1(10, 0), vec2(10, 1);
    int index = 0;
    for (auto &value: join(vec1, vec2)) {
        if (index < 10) {
            ASSERT_EQ(value, 0);
        } else {
            ASSERT_EQ(value, 1);
        }
        value = 2;
        index ++;
    }
    for (auto &value: join(vec1, vec2)) {
        ASSERT_EQ(value, 2);
    }
}