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

    // Combine with `ShiftRange`
    index = 10;
    for (auto &value: reversed(shift(vec, 5))) {
        ASSERT_EQ(value, -- index);
        value = 0;
    }
    for (auto &value: shift(reversed(vec), 0, 5)) {
        ASSERT_EQ(value, 0);
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

    // Combine with `ReversedRange`
    for (auto &value: reversed(indexing(values, indexes))) {
        ASSERT_EQ(value, index ++);
        value = 0;
    }
    for (auto &value: reversed(indexing(values, indexes))) {
        ASSERT_EQ(value, 0);
    }
}


/// Test `JoinedIterator`
TEST(Cherry, JoinedIterator) {
    // Assign and check values
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

    // Join and join
    std::vector<int> vec3(10, 0);
    for (auto &value: join(join(vec1, vec2), vec3)) {
        value = 3;
    }
    auto check = [](const std::vector<int> &vec) {
        for (auto value: vec) {
            ASSERT_EQ(value, 3);
        }
    };
    check(vec1);
    check(vec2);
    check(vec3);

    // Combine with `ReversedRange`
    index = 0;
    for (auto &value: join(join(vec1, reversed(vec2)), vec3)) {
        value = index ++;
    }
    index = 20;
    for (auto &value: vec2) {
        ASSERT_EQ(value, -- index);
    }
}