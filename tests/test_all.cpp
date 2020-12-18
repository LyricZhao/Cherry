#include <bitset>
#include <cmath>

#include "cherry.hpp"
#include "gtest/gtest.h"


// Ignore a Clang-Tidy warning from GoogleTest's `TEST` macro
#pragma ide diagnostic ignored "cert-err58-cpp"


/// Test whether GoogleTest works
TEST(Cherry, gtest) {
    int a = 1, b = 2;
    ASSERT_EQ(a + b, 3);
}


/// Test `Timer`
TEST(Cherry, NanoTimer) {
    // Use two timer to check whether the intervals are similar
    NanoTimer timer, wall_timer;
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

    // Global random
    int a = global_random_int(), b = global_random_int();
    ASSERT_NE(a, b);
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

    // `const_iterator`
    for (const auto &value: shift(vec, 0, 5)) {
        ASSERT_EQ(value, 0);
    }

    // `const Range &`
    const std::vector<int> const_vec(10, 0);
    for (const auto &value: shift(const_vec, 0, 5)) {
        ASSERT_EQ(value, 0);
    }

    // R-value
    for (const auto &value: shift(shift(vec, 0, 4), 0, 2)) {
        ASSERT_EQ(value, 0);
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
    for (const auto &value: reverse(vec)) {
        ASSERT_EQ(value, -- index);
    }

    // Combine with `ShiftRange`
    index = 10;
    for (auto &value: reverse(shift(vec, 5))) {
        ASSERT_EQ(value, -- index);
        value = 0;
    }
    for (auto &value: shift(reverse(vec), 0, 5)) {
        ASSERT_EQ(value, 0);
    }

    // `const Range &`
    const std::vector<int> const_vec(10, 0);
    for (const auto &value: reverse(const_vec)) {
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
    for (auto &value: reverse(indexing(values, indexes))) {
        ASSERT_EQ(value, index ++);
        value = 0;
    }
    for (auto &value: reverse(indexing(values, indexes))) {
        ASSERT_EQ(value, 0);
    }

    // `const Range &`
    std::vector<int> const_vec(10, 0);
    for (const auto &value: reverse(indexing(const_vec, indexes))) {
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
    for (auto &value: join(join(vec1, reverse(vec2)), vec3)) {
        value = index ++;
    }
    index = 20;
    for (auto &value: vec2) {
        ASSERT_EQ(value, -- index);
    }

    // `const Range &`
    std::vector<int> non_const_vec(10, 0);
    const std::vector<int> const_vec(10, 0);
    for (const auto &value: join(non_const_vec, join(non_const_vec, const_vec))) {
        ASSERT_EQ(value, 0);
    }
}


/// Test `map`
TEST(Cherry, map) {
    struct Item {
        int a = 0, b = 0;
    };
    std::vector<Item> items(10);
    auto mapped = map(items, [](const Item &item) -> int {
        return item.a;
    });
    for (const auto &value: mapped) {
        ASSERT_EQ(value, 0);
    }
}


/// Test `for_each`
TEST(Cherry, for_each) {
    // Test non-const
    int index = 0;
    std::vector<int> vec = {0, 1, 2, 3, 4};
    for_each(reverse(vec), [&index](int &item) {
        item = index ++;
    });
    for (const auto &item: vec) {
        ASSERT_EQ(item, -- index);
    }

    // Test const
    const std::vector<int> const_vec = vec;
    std::vector<int> vec2;
    for_each(const_vec, [&vec2](const int &item) {
        vec2.push_back(item);
    });
    ASSERT_EQ(vec2.size(), 5);
}


/// Test `all_of`
TEST(Cherry, all_of) {
    std::vector<int> vec = {0, 1, 2, 3, 4};
    auto check1 = [](const int &item) -> bool {
        return item < 5;
    };
    auto check2 = [](const int &item) -> bool {
        return item < 4;
    };
    ASSERT_EQ(all_of(vec, check1), true);
    ASSERT_EQ(all_of(vec, check2), false);
}


/// Test `none_of`
TEST(Cherry, none_of) {
    std::vector<int> vec = {0, 1, 2, 3, 4};
    auto check1 = [](const int &item) -> bool {
        return item >= 5;
    };
    auto check2 = [](const int &item) -> bool {
        return item >= 4;
    };
    ASSERT_EQ(none_of(vec, check1), true);
    ASSERT_EQ(none_of(vec, check2), false);
}


/// Test `any_of`
TEST(Cherry, any_of) {
    std::vector<int> vec = {0, 1, 2, 3, 4};
    auto check1 = [](const int &item) -> bool {
        return item == 2;
    };
    auto check2 = [](const int &item) -> bool {
        return item == 5;
    };
    ASSERT_EQ(any_of(vec, check1), true);
    ASSERT_EQ(any_of(vec, check2), false);
}


/// Test `find`
TEST(Cherry, find) {
    std::vector<int> vec = {0, 1, 2, 3, 4};
    ASSERT_EQ(find(vec, 0), true);
    ASSERT_EQ(find(shift(vec, 1), 0), false);
    ASSERT_EQ(find(shift(vec, 1), 4), true);
    ASSERT_EQ(find(shift(vec, 1, 2), 4), false);
}


/// Check `sum`
TEST(Cherry, sum) {
    std::vector<int> vec = {0, 1, 2, 3, 4};
    ASSERT_EQ(sum(vec), 10);
}


/// Check `check_duplicate`
TEST(Cherry, check_duplicate) {
    std::vector<int> vec = {1, 1, 2, 3, 4};
    ASSERT_EQ(check_duplicate(vec), true);
    ASSERT_EQ(check_duplicate(shift(vec, 1)), false);

    // TODO: bug
    // std::vector<int> vec2 = {1, 2, 3};
    // ASSERT_EQ(check_duplicate(join(vec, vec2)), true);
}


/// Check `Bitset`
TEST(Cherry, Bitset) {
    auto check = [](int length, auto &std_bitset, auto &my_bitset) -> bool {
        Random<int> random(0, length - 1);
        Random<bool> random_bool(false, true);
        for (int i = 0; i < 10; ++ i) {
            int k = random();
            bool value = random_bool();
            std_bitset[k] = value;
            my_bitset.set_bit(k, value);
        }
        for (int i = 0; i < length; ++ i) {
            if (std_bitset[i] != my_bitset.get_bit(i)) {
                return false;
            }
        }
        return true;
    };

    std::bitset<3> std_bitset_3;
    Bitset my_bitset_3(3);
    ASSERT_EQ(check(3, std_bitset_3, my_bitset_3), true);

    std::bitset<1024> std_bitset_1024;
    Bitset my_bitset_1024(1024);
    ASSERT_EQ(check(1024, std_bitset_1024, my_bitset_1024), true);
}