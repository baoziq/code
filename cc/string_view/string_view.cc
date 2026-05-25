#include "string_view.h"
#include <cstdint>
#include <cstring>
#include <stdexcept>

myStringView::myStringView() : ptr_(nullptr), len_(0) {}

myStringView::myStringView(const char* str) : ptr_(str), len_(strlen(str)) {}

myStringView::myStringView(const char *ptr, uint64_t len)
    : ptr_(ptr), len_(len) {}

size_t myStringView::size() const {
    return len_;
}

bool myStringView::empty() const {
    return len_ == 0;
}

bool myStringView::start_with(myStringView sv) {
    if (sv.size() > len_) {
        return false;
    }
    for (size_t i = 0; i < sv.size(); i++) {
        if (sv[i] != *(ptr_ + i)) {
            return false;
        }
    }
    return true;
}

bool myStringView::end_with(myStringView sv) {
    if (sv.size() > len_) {
        return false;
    }
    for (size_t i = 0; i < sv.size(); i++) {
        if (sv[i] != *(ptr_ + len_ - i - 1)) {
            return false;
        }
    }
    return true;
}

size_t myStringView::find(const char &ch) {
    for (size_t i = 0; i < len_; i++) {
        if (ch == *(ptr_ + i)) {
            return i;
        }
    }
    return -1;
}

size_t myStringView::find(myStringView sv) {
    (void) sv;
    return -1;
}

char myStringView::operator[](size_t index) const {
    return *(ptr_ + index);
}

myStringView myStringView::substr(size_t start, size_t end) const {
    if (end >= len_) {
        throw std::out_of_range("substr out of index");
    }
    return myStringView(ptr_ + start, end - start);
}
