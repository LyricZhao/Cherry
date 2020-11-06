#pragma once

#include <cctype>
#include <chrono>
#include <iostream>
#include <ostream>
#include <random>
#include <set>
#include <string>
#include <type_traits>


/// A nanosecond-level timer
class Timer {
    typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> time_point_t;

private:
    time_point_t last_time_point;

public:
    Timer() {
        last_time_point = std::chrono::system_clock::now();
    }

    /// Return the duration from last time point (constructor `Timer()` or `tik()`)
    uint64_t tik() {
        time_point_t time_point = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(time_point - last_time_point);
        last_time_point = time_point;
        return duration.count();
    }
};


/// A random number generator
template <typename value_type>
class Random {
private:
    typedef typename std::conditional<std::is_integral<value_type>::value,
            std::uniform_int_distribution<value_type>, std::uniform_real_distribution<value_type>>::type dist_t;

    std::default_random_engine engine;
    dist_t dist;

public:
    /// The interval is closed ([`min`, `max`])
    Random(value_type min, value_type max, int seed=0, bool pure=true) {
        assert(min <= max);
        if (pure) {
            seed = std::random_device()();
        }
        engine = std::default_random_engine(seed);
        dist = dist_t(min, max);
    }

    /// Generate a random number
    value_type operator () () {
        return dist(engine);
    }
};

// TODO: Support enumerate with index
// TODO: The compiler error may not be friendly when forcedly use `iterator` as `const_iterator`
#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-const-return-type"

/// A range which can begin from a certain position for the type with `begin()` and `end()` methods
template <typename Range>
class ShiftRange {
private:
    int pos, length = -1;
    Range &range;

public:
    typedef typename std::conditional<std::is_const<Range>::value, typename Range::const_iterator, typename Range::iterator>::type iterator;
    typedef typename std::conditional<std::is_const<Range>::value, typename Range::const_reverse_iterator, typename Range::reverse_iterator>::type reverse_iterator;
    typedef typename Range::const_iterator const_iterator;
    typedef typename Range::const_reverse_iterator const_reverse_iterator;
    typedef typename Range::value_type value_type;

    explicit ShiftRange(Range &range, int pos=0, int length=-1): range(range), pos(pos) {
        this->length = length == -1 ? range.end() - range.begin() - pos : length;
        assert(pos + this->length <= range.end() - range.begin());
    }

    [[nodiscard]] iterator begin() {
        return range.begin() + pos;
    }

    [[nodiscard]] iterator end() {
        return range.begin() + pos + length;
    }

    [[nodiscard]] reverse_iterator rbegin() {
        auto cut = range.end() - range.begin() - (pos + length);
        return range.rbegin() + cut;
    }

    [[nodiscard]] reverse_iterator rend() {
        auto cut = range.end() - range.begin() - pos;
        return range.rbegin() + cut;
    }

    [[nodiscard]] const_iterator begin() const {
        return range.begin() + pos;
    }

    [[nodiscard]] const_iterator end() const {
        return range.begin() + pos + length;
    }

    [[nodiscard]] const_reverse_iterator rbegin() const {
        auto cut = range.end() - range.begin() - (pos + length);
        return range.rbegin() + cut;
    }

    [[nodiscard]] const_reverse_iterator rend() const {
        auto cut = range.end() - range.begin() - pos;
        return range.rbegin() + cut;
    }
};


/// Return a range which can iterate from a certain position
template <typename Range>
ShiftRange<Range> shift(Range &range, int pos=0, int length=-1) {
    return ShiftRange(range, pos, length);
}


/// Return a range which can iterate from a certain position
template <typename Range>
const ShiftRange<const Range> shift(const Range &range, int pos=0, int length=-1) {
    return ShiftRange<const Range>(range, pos, length);
}


/// A reverse wrapper for a range
template <typename Range>
class ReversedRange {
private:
    Range &range;

public:
    typedef typename std::conditional<std::is_const<Range>::value, typename Range::const_reverse_iterator, typename Range::reverse_iterator>::type iterator;
    typedef typename std::conditional<std::is_const<Range>::value, typename Range::const_iterator, typename Range::iterator>::type reverse_iterator;
    typedef typename Range::const_reverse_iterator const_iterator;
    typedef typename Range::const_iterator const_reverse_iterator;
    typedef typename Range::value_type value_type;

    explicit ReversedRange(Range &range): range(range) {}

    [[nodiscard]] iterator begin() {
        return range.rbegin();
    }

    [[nodiscard]] iterator end() {
        return range.rend();
    }

    [[nodiscard]] reverse_iterator rbegin() {
        return range.begin();
    }

    [[nodiscard]] reverse_iterator rend() {
        return range.end();
    }

    [[nodiscard]] const_iterator begin() const {
        return range.rbegin();
    }

    [[nodiscard]] const_iterator end() const {
        return range.rend();
    }

    [[nodiscard]] const_reverse_iterator rbegin() const {
        return range.begin();
    }

    [[nodiscard]] const_reverse_iterator rend() const {
        return range.end();
    }
};


/// A function wrapper for `ReversedRange`
template <typename Range>
ReversedRange<Range> reverse(Range &range) {
    return ReversedRange<Range>(range);
}


/// A function wrapper for `ReversedRange`
template <typename Range>
ReversedRange<Range> reverse(Range &&range) {
    return ReversedRange<Range>(range);
}


/// A function wrapper for `ReversedRange`
template <typename Range>
const ReversedRange<const Range> reverse(const Range &range) {
    return ReversedRange<const Range>(range);
}


/// A range for an indexing array and corresponding items
template <typename Array, typename Range>
class IndexingRange {
private:
    Array &items;
    const Range &indexes;

public:
    typedef typename Array::value_type value_type;
    typedef typename std::conditional<std::is_const<Array>::value, const typename Array::value_type&, typename Array::value_type&>::type reference;

    /// The iterator type for `IndexRange`
    template<typename index_const_iterator_t>
    struct Iterator {
        Array &items;
        index_const_iterator_t index_const_iterator;

        Iterator(Array &items, const index_const_iterator_t &index_const_iterator): items(items), index_const_iterator(index_const_iterator) {}

        reference operator * () const {
            return items[*index_const_iterator];
        }

        Iterator operator ++ () {
            index_const_iterator ++;
            return *this;
        }

        bool operator == (const Iterator &other) const {
            return index_const_iterator == other.index_const_iterator;
        }

        bool operator != (const Iterator &other) const {
            return index_const_iterator != other.index_const_iterator;
        }
    };

    typedef Iterator<typename Range::const_iterator> iterator;
    typedef Iterator<typename Range::const_reverse_iterator> reverse_iterator;
    typedef Iterator<typename Range::const_iterator> const_iterator;
    typedef Iterator<typename Range::const_reverse_iterator> const_reverse_iterator;

    IndexingRange(Array &items, const Range &indexes):
            items(items), indexes(indexes) {}

    [[nodiscard]] iterator begin() {
        return iterator(items, indexes.begin());
    }

    [[nodiscard]] iterator end() {
        return iterator(items, indexes.end());
    }

    [[nodiscard]] reverse_iterator rbegin() {
        return reverse_iterator(items, indexes.rbegin());
    }

    [[nodiscard]] reverse_iterator rend() {
        return reverse_iterator(items, indexes.rend());
    }

    [[nodiscard]] const_iterator begin() const {
        return const_iterator(items, indexes.begin());
    }

    [[nodiscard]] const_iterator end() const {
        return const_iterator(items, indexes.end());
    }

    [[nodiscard]] const_reverse_iterator rbegin() const {
        return const_reverse_iterator(items, indexes.rbegin());
    }

    [[nodiscard]] const_reverse_iterator rend() const {
        return const_reverse_iterator(items, indexes.rend());
    }
};


/// Return a range which can iterate over the corresponding items by the indexing array
template <typename Array, typename Range>
IndexingRange<Array, Range> indexing(Array &array, const Range &indexes) {
    return IndexingRange<Array, Range>(array, indexes);
}


/// Return a range which can iterate over the corresponding items by the indexing array
template <typename Array, typename Range>
const IndexingRange<const Array, Range> indexing(const Array &array, const Range &indexes) {
    return IndexingRange<const Array, Range>(array, indexes);
}


/// A joined range for two ranges
template <typename Range1, typename Range2>
class JoinedRange {
private:
    Range1 &range1;
    Range2 &range2;

public:
    // Check whether they're the same type
    static_assert(std::is_same<typename Range1::value_type, typename Range2::value_type>::value,
                  "The types of two ranges in JoinedRange must be same");

    static constexpr bool be_const = std::is_const<Range1>::value or std::is_const<Range2>::value;
    typedef typename Range1::value_type value_type;
    typedef typename std::conditional<be_const, const typename Range1::value_type&, typename Range1::value_type&>::type reference;

    /// The iterator type for `JoinedRange`
    template <typename iterator1_t, typename iterator2_t>
    struct Iterator {
        bool first;
        iterator1_t iterator1, iterator1_end;
        iterator2_t iterator2;

        Iterator(bool first, const iterator1_t &iterator1, const iterator1_t &iterator1_end, const iterator2_t &iterator2):
                first(first), iterator1(iterator1), iterator1_end(iterator1_end), iterator2(iterator2) {}

        Iterator(const iterator1_t &iterator1, const iterator1_t &iterator1_end, const iterator2_t &iterator2):
                iterator1(iterator1), iterator1_end(iterator1_end), iterator2(iterator2) {
            first = iterator1 != iterator1_end;
        }

        reference operator * () const {
            return first ? *iterator1 : *iterator2;
        }

        Iterator operator ++ () {
            if (first) {
                ++ iterator1;
                if (iterator1 == iterator1_end) {
                    first = false;
                }
            } else {
                ++ iterator2;
            }
            return *this;
        }

        bool operator == (const Iterator &other) const {
            if (first != other.first) {
                return false;
            }
            return first ? iterator1 == other.iterator1 : iterator2 == other.iterator2;
        }

        bool operator != (const Iterator &other) const {
            if (first == other.first) {
                return first ? iterator1 != other.iterator1 : iterator2 != other.iterator2;
            }
            return true;
        }
    };

    typedef typename std::conditional<be_const,
            Iterator<typename Range1::const_iterator, typename Range2::const_iterator>,
            Iterator<typename Range1::iterator, typename Range2::iterator>>::type iterator;
    typedef typename std::conditional<be_const,
            Iterator<typename Range1::const_reverse_iterator, typename Range2::const_reverse_iterator>,
            Iterator<typename Range1::reverse_iterator, typename Range2::reverse_iterator>>::type reverse_iterator;
    typedef Iterator<typename Range1::const_iterator, typename Range2::const_iterator> const_iterator;
    typedef Iterator<typename Range2::const_reverse_iterator, typename Range1::const_reverse_iterator> const_reverse_iterator;

    JoinedRange(Range1 &range1, Range2 &range2): range1(range1), range2(range2) {}

    [[nodiscard]] iterator begin() {
        return iterator(range1.begin(), range1.end(), range2.begin());
    }

    [[nodiscard]] iterator end() {
        return iterator(false, range1.end(), range1.end(), range2.end());
    }

    [[nodiscard]] reverse_iterator rbegin() {
        return reverse_iterator(range2.rbegin(), range2.rend(), range1.rbegin());
    }

    [[nodiscard]] reverse_iterator rend() {
        return reverse_iterator(false, range2.rend(), range2.rend(), range1.rend());
    }

    [[nodiscard]] const_iterator begin() const {
        return const_iterator(range1.begin(), range1.end(), range2.begin());
    }

    [[nodiscard]] const_iterator end() const {
        return const_iterator(false, range1.end(), range1.end(), range2.end());
    }

    [[nodiscard]] const_reverse_iterator rbegin() const {
        return const_reverse_iterator(range2.rbegin(), range2.rend(), range1.rbegin());
    }

    [[nodiscard]] const_reverse_iterator rend() const {
        return const_reverse_iterator(false, range2.rend(), range2.rend(), range1.rend());
    }
};

/// Join two ranges
template <typename Range1, typename Range2>
JoinedRange<Range1, Range2> join(Range1 &range1, Range2 &range2) {
    return JoinedRange<Range1, Range2>(range1, range2);
}


/// Join two ranges
template <typename Range1, typename Range2>
JoinedRange<Range1, Range2> join(Range1 &range1, Range2 &&range2) {
    return JoinedRange<Range1, Range2>(range1, range2);
}


/// Join two ranges
template <typename Range1, typename Range2>
const JoinedRange<Range1, const Range2> join(Range1 &range1, const Range2 &range2) {
    return JoinedRange<Range1, const Range2>(range1, range2);
}


/// Join two ranges
template <typename Range1, typename Range2>
JoinedRange<Range1, Range2> join(Range1 &&range1, Range2 &range2) {
    return JoinedRange<Range1, Range2>(range1, range2);
}


/// Join two ranges
template <typename Range1, typename Range2>
JoinedRange<Range1, Range2> join(Range1 &&range1, Range2 &&range2) {
    return JoinedRange<Range1, Range2>(range1, range2);
}


/// Join two ranges
template <typename Range1, typename Range2>
const JoinedRange<Range1, const Range2> join(Range1 &&range1, const Range2 &range2) {
    return JoinedRange<Range1, const Range2>(range1, range2);
}


/// Join two ranges
template <typename Range1, typename Range2>
const JoinedRange<const Range1, Range2> join(const Range1 &range1, Range2 &range2) {
    return JoinedRange<const Range1, Range2>(range1, range2);
}


/// Join two ranges
template <typename Range1, typename Range2>
const JoinedRange<const Range1, Range2> join(const Range1 &range1, Range2 &&range2) {
    return JoinedRange<const Range1, Range2>(range1, range2);
}


/// Join two ranges
template <typename Range1, typename Range2>
const JoinedRange<const Range1, const Range2> join(const Range1 &range1, const Range2 &range2) {
    return JoinedRange<const Range1, const Range2>(range1, range2);
}

#pragma clang diagnostic pop


/// Concat two ranges into a `std::vector`
template <typename Range1, typename Range2, typename value_type = typename Range1::value_type>
std::vector<value_type> concat(const Range1 &range1, const Range2 &range2) {
    static_assert(std::is_same<typename Range1::value_type, typename Range2::value_type>::value);
    std::vector<value_type> vec;
    for (const auto &value: range1) {
        vec.push_back(value);
    }
    for (const auto &value: range2) {
        vec.push_back(value);
    }
    return vec;
}


/// Map all the items into another
template <typename Range, typename Function>
auto map(const Range &range, const Function &f) {
    using value_type = decltype(f(*range.begin()));
    std::vector<value_type> mapped;
    for (const auto &item: range) {
        mapped.push_back(f(item));
    }
    return mapped;
}


/// Sum of all the values in a range
template <typename Range, typename value_type = typename Range::value_type>
value_type sum(const Range &range) {
    value_type sum_value = 0;
    for (const auto &item: range) {
        sum_value += item;
    }
    return sum_value;
};


/// Check whether there are two duplicate items in a range
template <typename Range, typename value_type = typename Range::value_type>
bool check_duplicate(const Range &range) {
    std::set<value_type> set;
    int size = 0;
    for (const auto &item: range) {
        set.insert(item);
        ++ size;
    }
    return set.size() != size;
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


/// An unimplemented error raiser
#define unimplemented() { \
    std::cerr << "Unimplemented part at line " << __LINE__ << " in file " << __FILE__ << std::endl; \
    std::exit(EXIT_FAILURE); \
}


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


/// Dynamic bitset
class Bitset {
private:
    int n = 0;
    size_t *data = nullptr;
    bool hash_calcuated = false;
    uint64_t hash_value = 0;

    void allocate(int bits) {
        n = bits;
        n /= width;
        n += bits % width == 0 ? 0 : 1;
        assert(data == nullptr);
        data = static_cast<size_t*>(std::malloc(n * width));
    }

public:
    static constexpr int width = sizeof(size_t);

    explicit Bitset(int bits) {
        allocate(bits);
    }

    Bitset(int bits, const std::vector<int> &indexes) {
        allocate(bits);
        for (const auto &index: indexes) {
            set_bit(index, true);
        }
    }

    ~Bitset() {
        assert(data != nullptr);
        std::free(data);
    }

    /// Check whether all the bits at indexes are 1
    [[nodiscard]] bool contains(const std::vector<int> &indexes) const {
        return std::all_of(indexes.begin(), indexes.end(), [this](int index) -> bool {
            return get_bit(index);
        });
    }

    /// Set the bit at `index` to `bit` (will not check overflow)
    void set_bit(int index, bool bit) {
        // TODO: maybe add a `BitReference` to achieve `[]` operator
        size_t i = index / width;
        size_t s = index % width;
        data[i] |= 1 << s;
        data[i] ^= 1 << s;
        data[i] |= bit << s;
        hash_calcuated = false;
    }

    /// Get the bit at `index`
    [[nodiscard]] bool get_bit(int index) const {
        size_t i = index / width;
        size_t s = index % width;
        return data[i] >> s;
    }

    /// Get the hash value of the bitset
    [[nodiscard]] uint64_t hash() {
        if (not hash_calcuated) {
            hash_calcuated = true;
            hash_value = 0;
            for (int i = 0; i < n; ++ i) {
                hash_value = hash_value * 133ull + data[i];
            }
        }
        return hash_value;
    }

    /// Clear all the bits
    void clear() {
        std::memset(data, 0, n * width);
    }
};