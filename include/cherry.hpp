#pragma once

#include <cctype>
#include <chrono>
#include <iostream>
#include <ostream>
#include <random>
#include <string>

/// A nanosecond-level timer
class Timer {
    typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> NanoTimePoint;

private:
    NanoTimePoint last_time_point;

public:
    Timer() {
        last_time_point = std::chrono::system_clock::now();
    }

    /// Return the duration from last time point (constructor `Timer()` or `tik()`)
    uint64_t tik() {
        NanoTimePoint time_point = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(time_point - last_time_point);
        last_time_point = time_point;
        return duration.count();
    }
};


/// A random number generator
template <typename T>
class Random {
private:
    std::default_random_engine engine;
    std::uniform_int_distribution<T> dist;

public:
    Random(T min, T max, int seed=0) {
        assert(min < max);
        engine = std::default_random_engine(seed);
        dist = std::uniform_int_distribution<T>(min, max - 1);
    }

    /// Generate a random number
    T operator () () {
        return dist(engine);
    }
};


/// An iterator which can begin from a certain position for `std::vector`
template <typename T>
struct VectorIterator {
    int pos;
    std::vector<T> &items;

    explicit VectorIterator(std::vector<T> &items, int pos=0): items(items), pos(pos) { }

    [[nodiscard]] auto begin() const {
        return items.begin() + pos;
    }

    [[nodiscard]] auto end() const {
        return items.end();
    }
};


/// A reversed iterator for `std::vector`
template <typename T>
struct ReversedVectorIterator {
    std::vector<T> &items;

    explicit ReversedVectorIterator(std::vector<T> &items): items(items) { }

    [[nodiscard]] auto begin() const {
        return items.rbegin();
    }

    [[nodiscard]] auto end() const {
        return items.rend();
    }
};


/// A function wrapper for `ReversedVectorIterator`
template <typename T>
ReversedVectorIterator<T> iterate_reversed(std::vector<T> &items) {
    return ReversedVectorIterator<T>(items);
}


/// An iterator for an indexing array and corresponding items
template <typename T>
struct IndexIterator {
    int pos;
    std::vector<int> &indexes;
    std::vector<T> &items;

    IndexIterator(std::vector<int> &indexes, std::vector<T> &items):
            indexes(indexes), items(items) {
        pos = 0;
    }

    IndexIterator(std::vector<int> &indexes, std::vector<T> &items, int pos):
            indexes(indexes), items(items), pos(pos) { }

    T& operator * () const {
        return items[indexes[pos]];
    }

    IndexIterator operator ++ () {
        pos ++;
        return *this;
    }

    bool operator != (const IndexIterator &other) const {
        return pos != other.pos;
    }

    [[nodiscard]] IndexIterator begin() const {
        return IndexIterator(indexes, items);
    }

    [[nodiscard]] IndexIterator end() const {
        return IndexIterator(indexes, items, indexes.size());
    }

    /// Extract the items by indexes to `std::vector`
    std::vector<T> to_vector() const {
        std::vector<T> vector;
        vector.reserve(indexes.size());
        for (auto &item: IndexIterator<T>(indexes, items)) {
            vector.push_back(item);
        }
        return vector;
    }
};


/// A reversed iterator for an indexing array and corresponding items
template <typename T>
struct ReversedIndexIterator {
    int pos;
    std::vector<int> &indexes;
    std::vector<T> &items;

    ReversedIndexIterator(std::vector<int> &indexes, std::vector<T> &items):
            indexes(indexes), items(items) {
        pos = items.size() - 1;
    }

    ReversedIndexIterator(std::vector<int> &indexes, std::vector<T> &items, int pos):
            indexes(indexes), items(items), pos(pos) { }

    T& operator * () const {
        return items[indexes[pos]];
    }

    ReversedIndexIterator operator ++ () {
        pos --;
        return *this;
    }

    bool operator != (const ReversedIndexIterator &other) const {
        return pos != other.pos;
    }

    [[nodiscard]] ReversedIndexIterator begin() const {
        return ReversedIndexIterator(indexes, items);
    }

    [[nodiscard]] ReversedIndexIterator end() const {
        return ReversedIndexIterator(indexes, items, -1);
    }

    /// Extract the items by reversed indexes to `std::vector`
    std::vector<T> to_vector() const {
        std::vector<T> vector;
        vector.reserve(indexes.size());
        for (auto &item: ReversedVectorIterator<T>(indexes, items)) {
            vector.push_back(item);
        }
        return vector;
    }
};


/// Convert a number to `std::string` with units
template <typename T>
std::string pretty(T value, T scale, const char* *units, int max_level) {
    int count = 0;
    auto d = static_cast<double>(value);
    while (d > scale && count < max_level - 1) {
        d /= scale;
        count += 1;
    }
    static char buffer[64];
    sprintf(buffer, "%.6f %s", d, units[count]);
    return buffer;
}


/// Convert a size to `std::string` with units
std::string prettyBytes(size_t size) {
    static const char* units[5] = {"B", "KiB", "MiB", "GiB"};
    return pretty<size_t>(size, 1024, units, 4);
}

/// Convert a nanosecond to `std::string` with units (always millisecond if `fixed` is true)
std::string prettyNanoseconds(uint64_t duration, bool fixed=true) {
    // To millisecond
    if (fixed) {
        static char buffer[64];
        sprintf(buffer, "%.6f ms", duration / 1e6);
        return buffer;
    }
    // Non-fixed
    static const char* units[5] = {"ns", "us", "ms", "s"};
    return pretty<uint64_t>(duration, 1000, units, 4);
}


/// Console colors
class ConsoleColor {
public:
    static constexpr const char *reset  = "\033[0m";
    static constexpr const char *black  = "\033[30m";
    static constexpr const char *red    = "\033[31m";
    static constexpr const char *green  = "\033[32m";
    static constexpr const char *yellow = "\033[33m";
    static constexpr const char *blue   = "\033[34m";
    static constexpr const char *white  = "\033[37m";
};


/// Size and time units' helper
class Unit {
public:
    template<typename T>
    static constexpr size_t B(T size) {
        return size;
    }

    template<typename T>
    static constexpr size_t KiB(T size) {
        return size * 1024ull;
    }

    template<typename T>
    static constexpr size_t MiB(T size) {
        return size * 1024ull * 1024ull;
    }

    template<typename T>
    static constexpr size_t GiB(T size) {
        return size * 1024ull * 1024ull * 1024ull;
    }

    template<typename T>
    static constexpr uint64_t ns(T duration) {
        return duration;
    }

    template<typename T>
    static constexpr uint64_t us(T duration) {
        return duration * 1000ull;
    }

    template<typename T>
    static constexpr uint64_t ms(T duration) {
        return duration * 1000000ull;
    }

    template<typename T>
    static constexpr uint64_t s(T duration) {
        return duration * 1000000000ull;
    }

    /// Convert `std::string` to `size_t`
    static size_t bytes_from(const std::string &text) {
        const char *ptr = text.c_str();
        char *unit_ptr;
        double size = strtod(ptr, &unit_ptr);
        if (unit_ptr - ptr == text.size()) {
            std::cerr << "No unit specified" << std::endl;
        } else if (*unit_ptr == 'B') {
            return B(size);
        } else if (*unit_ptr == 'K') {
            return KiB(size);
        } else if (*unit_ptr == 'M') {
            return MiB(size);
        } else if (*unit_ptr == 'G') {
            return GiB(size);
        }
        std::cerr << "Failed to parse size (format: {num}{B/KiB/MiB/GiB}, e.g. 8GiB)" << std::endl;
        return 0;
    }
};
