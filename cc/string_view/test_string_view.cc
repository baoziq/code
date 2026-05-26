#include "string_view.h"

#include <iostream>
#include <cassert>
#include <cstring>

void test_default_constructor() {
    myStringView sv;
    assert(sv.size() == 0);
    assert(sv.empty());
}

void test_c_constructor() {
    myStringView sv("hello");

    assert(sv.size() == 5);
    assert(!sv.empty());
    assert(sv[0] == 'h');
    assert(sv[1] == 'e');
    assert(sv[4] == 'o');
}

void test_pointer_len_constructor() {
    const char* s = "hello world";
    myStringView sv(s, 5);

    assert(sv.size() == 5);
    assert(sv[0] == 'h');
    assert(sv[4] == 'o');
}

void test_substr() {
    myStringView sv("hello world");

    myStringView sub = sv.substr(0, 5);
    assert(sub.size() == 5);
    assert(sub[0] == 'h');
    assert(sub[4] == 'o');

    myStringView sub2 = sv.substr(6, 5);
    assert(sub2.size() == 5);
    assert(sub2[0] == 'w');
    assert(sub2[4] == 'd');
}

void test_starts_with() {
    myStringView sv("hello world");

    assert(sv.start_with(myStringView("hello")));
    assert(sv.start_with(myStringView("")));
    assert(!sv.start_with(myStringView("world")));
    assert(!sv.start_with(myStringView("hello world!!!")));
}

void test_ends_with() {
    myStringView sv("hello world");

    assert(sv.end_with(myStringView("world")));
    assert(sv.end_with(myStringView("")));
    assert(!sv.end_with(myStringView("hello")));
    assert(!sv.end_with(myStringView("!!!hello world")));
}

void test_find_char() {
    myStringView sv("hello world");

    assert(sv.find('h') == 0);
    assert(sv.find('o') == 4);
    assert(sv.find('w') == 6);

    // 这里要看你自己怎么定义 npos
    assert(sv.find('x') == myStringView::npos);
}

void test_find_string() {
    myStringView sv("hello world");

    assert(sv.find(myStringView("hello")) == 0);
    assert(sv.find(myStringView("world")) == 6);
    assert(sv.find(myStringView("lo")) == 3);
    assert(sv.find(myStringView("")) == 0);
    assert(sv.find(myStringView("abc")) == myStringView::npos);
}

void test_no_copy() {
    char s[] = "hello";
    myStringView sv(s);

    assert(sv[0] == 'h');

    s[0] = 'H';

    // 如果 StringView 没有拷贝底层字符串，这里应该能看到修改
    assert(sv[0] == 'H');
}

void basic_test() {
    myStringView sv("hello");
    assert(sv.substr(5, 0).empty());
    assert(sv.find(myStringView("")) == 0);
    assert(sv.start_with(myStringView("")));
    assert(sv.end_with(myStringView("")));
    assert(sv.find(myStringView("world")) == myStringView::npos);
}

int main() {
    test_default_constructor();
    test_c_constructor();
    test_pointer_len_constructor();
    test_find_char();
    test_find_string();
    test_ends_with();
    test_no_copy();
    test_pointer_len_constructor();
    test_substr();
    basic_test();
    std::cout << "All test passed\n";
}
