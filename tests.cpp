#include <assert.h>
#include <iostream>
#include "my_binary_tree_set.hpp"

void testConstructor() {
    MyBTreeSet<int> set1;
    assert(set1.length() == 0);
    std::cout << "constructor test passed\n";
}

void testBasicInsert() {
    MyBTreeSet<int> set;
    assert(set.insert(1));
    assert(set.length() == 1);
    assert(!set.insert(1));
    std::cout << "basic insert test passed\n";
}

void testBasicContains() {
    MyBTreeSet<int> set;
    set.insert(2);
    assert(set.contains(2));
    assert(!set.contains(1));
    std::cout << "basic contains tests passed\n";
}

void testBasicRemove() {
    MyBTreeSet<int> set;
    assert(set.insert(2));
    assert(set.length() == 1);
    assert(!set.remove(1));
    assert(set.length() == 1);
    assert(set.remove(2));
    assert(set.length() == 0);
    assert(!set.remove(2));
    std::cout << "basic remove tests passed\n";
}

void testInsert() {
    MyBTreeSet<int> set;
    for (int i = -100; i <= 100; i++)
        assert(set.insert(i));
    assert(set.length() == 201);
    for (int i = -100; i <= 100; i++)
        assert(set.contains(i));
    assert(set.length() == 201);
    std::cout << "insert tests passed\n";
}

void testInsertDelete() {
    MyBTreeSet<int> set;
    for (int i = -100; i <= 100; i++)
        assert(set.insert(i));
    assert(set.length() == 201);
    for (int i = -100; i <= 100; i++){
        assert(set.remove(i));
        assert(!set.contains(i));
    }
    assert(set.length() == 0);
    std::cout << "insert + delete tests passed\n";
}

void testInsertDeleteObject() {
    MyBTreeSet<std::string> set;
    assert(set.insert("hello"));
    assert(set.contains("hello"));
    assert(!set.contains("aaa"));
    assert(!set.remove("aaa"));
    assert(set.remove("hello"));
    std::cout << "string tests passed\n";
}

int main() {
    testConstructor();
    testBasicInsert();
    testBasicContains();
    testBasicRemove();
    testInsert();
    testInsertDelete();
    testInsertDeleteObject();
    std::cout << "all test passed\n";
    return 0;
}