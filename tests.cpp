#include <gtest/gtest.h>
#include "my_binary_tree_set.hpp"


using TestedTypes = ::testing::Types<int, double, std::string>;

/**
 * @invariant All array numbered `1` have length `40`
 * @invariant All array numbered `2` have length `60`
 */
template <typename T>
class MyBTreeSetTest : public ::testing::Test {

protected:

    static inline const int i_array_1[40] = {73, 12, 98, 45, 21, 67, 34, 89, 5, 56,18, 92, 41, 77, 29, 63, 10, 84, 37, 50,96, 14, 58, 25, 71, 3, 88, 46, 19, 65,32, 81, 7, 54, 99, 27, 60, 39, 16, 75};
    static inline const int i_array_2[60] = {42, 17, 89, 3, 56, 91, 24, 68, 12, 77, 35, 8, 94, 51, 26, 73, 14, 60, 99, 31, 47, 5, 82, 19, 66, 38, 11, 97, 53, 28, 75, 9, 44, 86, 21, 63, 16, 90, 34, 58, 7, 80, 25, 49, 93, 13, 70, 36, 54, 2, 88, 41, 65, 18, 79, 30, 95, 57, 22, 84};
    static inline const double f_array_1[40] = {14.72, 83.19, 27.56, 91.04, 35.88, 62.41, 7.93, 48.15, 76.82, 19.67, 54.39, 88.01, 23.74, 69.58, 12.46, 95.30, 41.87, 58.22, 3.91, 81.65, 29.14, 66.78, 17.53, 99.42, 45.26, 72.90, 8.34, 57.11, 31.69, 84.05, 20.48, 63.97, 10.25, 93.56, 38.71, 51.84, 25.09, 79.33, 6.18, 87.60};
    static inline const double f_array_2[60] = {12.48, 93.15, 47.62, 8.39, 55.71, 21.04, 76.88, 3.56, 64.29, 18.91, 82.47, 39.75, 7.14, 91.63, 28.50, 54.87, 11.22, 68.94, 35.16, 79.08, 24.61, 97.35, 42.83, 6.19, 58.44, 13.77, 85.92, 31.58, 72.40, 49.06, 16.33, 94.71, 26.89, 61.25, 38.47, 9.82, 87.14, 44.53, 70.68, 19.37, 53.91, 5.28, 80.46, 33.72, 96.18, 14.95, 66.81, 27.39, 74.56, 41.03, 89.67, 22.14, 57.88, 10.49, 63.27, 36.75, 98.04, 29.61, 51.92, 17.86};
    static inline const std::string s_array_1[40] = {"alpha", "bravo", "charlie", "delta", "echo", "foxtrot", "golf", "hotel", "india", "juliet", "kilo", "lima", "mike", "november", "oscar", "papa", "quebec", "romeo", "sierra", "tango", "uniform", "victor", "whiskey", "xray", "yankee", "zulu", "crimson", "emerald", "sapphire", "amber", "violet", "silver", "gold", "bronze", "copper", "onyx", "pearl", "ruby", "topaz", "jade"};
    static inline const std::string s_array_2[60] = {"apple", "banana", "cherry", "dragonfruit", "elderberry", "fig", "grape", "honeydew", "kiwi", "lemon", "mango", "nectarine", "orange", "papaya", "quince", "raspberry", "strawberry", "tangerine", "ugli", "vanilla", "watermelon", "xigua", "yam", "zucchini", "apricot", "blackberry", "coconut", "date", "eggplant", "feijoa", "guava", "hazelnut", "iceberg", "jackfruit", "kumquat", "lime", "mulberry", "nutmeg", "olive", "peach", "pear", "pineapple", "plum", "pomegranate", "radish", "spinach", "tomato", "turnip", "walnut", "yambean", "artichoke", "broccoli", "cabbage", "carrot", "celery", "cucumber", "garlic", "lettuce", "onion", "pepper"};

    size_t get_len_1() { return 40; }
    size_t get_len_2() { return 60; }
    
    /**
     * @return The array number `1` of the corresponding type.
     * 
     * @note The returned array has length `get_len_1()`.
     */
    T* get_arr_1() {
        if constexpr (std::is_same_v<T, int>) {
            return const_cast<int*>(i_array_1);
        } else if constexpr (std::is_same_v<T, double>) {
            return const_cast<double*>(f_array_1);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return const_cast<std::string*>(s_array_1);
        }
    } 

    /**
     * @return The array number `2` of the corresponding type.
     * 
     * @note The returned array has length `get_len_2()`.
     */
    T* get_arr_2() {
        if constexpr (std::is_same_v<T, int>) {
            return const_cast<int*>(i_array_2);
        } else if constexpr (std::is_same_v<T, double>) {
            return const_cast<double*>(f_array_2);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return const_cast<std::string*>(s_array_2);
        }
    }
    
    /**
     * @post `s.length() >= get_len_1()`
     * @post All element from the array 1 or present in `s`.
     */
    void populate_set_1(MyBTreeSet<T>& s) {
        T* array = get_arr_1();
        size_t len = get_len_1();
        for (size_t i = 0; i < len; i++)
            s.insert(array[i]);
    }

    /**
     * @post `s.length() >= get_len_2()`
     * @post All element from the array 2 or present in `s`.
     */
    void populate_set_2(MyBTreeSet<T>& s) {
        T* array = get_arr_2();
        size_t len = get_len_2();
        for (size_t i = 0; i < len; i++)
            s.insert(array[i]);
    }

};

TYPED_TEST_SUITE(MyBTreeSetTest, TestedTypes);

TYPED_TEST(MyBTreeSetTest, DefaulftConstructor) {
    MyBTreeSet<TypeParam> s;
    EXPECT_EQ(0, s.length());
}

TYPED_TEST(MyBTreeSetTest, Populating) {
    MyBTreeSet<TypeParam> s;
    this->populate_set_1(s);
    EXPECT_EQ(this->get_len_1(), s.length());
    for (size_t i = 0; i < this->get_len_1(); i++)
        EXPECT_TRUE(s.contains(this->get_arr_1()[i]));
}

TYPED_TEST(MyBTreeSetTest, MoveConstructor) {
    MyBTreeSet<TypeParam> s1;
    this->populate_set_1(s1);
    MyBTreeSet<TypeParam> s2(std::move(s1));
    EXPECT_EQ(0, s1.length());
    EXPECT_EQ(this->get_len_1(), s2.length());
    for (size_t i = 0; i < this->get_len_1(); i++)
        EXPECT_TRUE(s2.contains(this->get_arr_1()[i]));
}

TYPED_TEST(MyBTreeSetTest, MoveAssignment) {
    MyBTreeSet<TypeParam> s1;
    this->populate_set_1(s1);
    MyBTreeSet<TypeParam> s2;
    s2 = std::move(s1);
    EXPECT_EQ(0, s1.length());
    EXPECT_EQ(this->get_len_1(), s2.length());
    for (size_t i = 0; i < this->get_len_1(); i++)
        EXPECT_TRUE(s2.contains(this->get_arr_1()[i]));
}

TYPED_TEST(MyBTreeSetTest, BasicInsertion) {
    MyBTreeSet<TypeParam> s;
    TypeParam* array = this->get_arr_1();
    for (size_t i = 0; i < 10; i++)
        EXPECT_TRUE(s.insert(array[i]));
    for (size_t i = 0; i < 10; i++)
        EXPECT_TRUE(s.contains(array[i]));
    for (size_t i = 10; i < this->get_len_1(); i++)
        EXPECT_FALSE(s.contains(array[i]));
}

TYPED_TEST(MyBTreeSetTest, InsertionFails) {
    MyBTreeSet<TypeParam> s;
    this->populate_set_1(s);
    for (size_t i = 0; i < this->get_len_1(); i++) {
        TypeParam value = this->get_arr_1()[i];
        EXPECT_TRUE(s.contains(value));
        EXPECT_FALSE(s.insert(value));
    }
}

TYPED_TEST(MyBTreeSetTest, Contains) {
    MyBTreeSet<TypeParam> s;
    this->populate_set_1(s);
    for (size_t i = 0; i < this->get_len_1(); i++)
        EXPECT_TRUE(s.contains(this->get_arr_1()[i]));
}

TYPED_TEST(MyBTreeSetTest, BasicRemove) {
    MyBTreeSet<TypeParam> s;
    this->populate_set_1(s);
    size_t len = s.length();
    for (size_t i = 0; i < s.length(); i++) {
        TypeParam value = this->get_arr_1()[i];
        EXPECT_TRUE(s.contains(value));
        EXPECT_EQ(len - i, s.length());
        EXPECT_TRUE(s.remove(value));
        EXPECT_FALSE(s.contains(value));
        EXPECT_EQ(len - i - 1, s.length());
    }
}