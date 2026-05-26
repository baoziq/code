#include "string_view.h"
#include <cstdint>
#include <cstring>
#include <stdexcept>

myStringView::myStringView() : ptr_(nullptr), len_(0) {}

myStringView::myStringView(const char* str) : ptr_(str), len_(strlen(str)) {}

myStringView::myStringView(const char *ptr, size_t len)
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
    int cur = 0;
    for (int i = sv.size() - 1; i >= 0; i--) {
        if (sv[i] != *(ptr_ + len_ - cur - 1)) {
            return false;
        }
        cur++;
    }
    return true;
}

size_t myStringView::find(const char &ch) {
    for (size_t i = 0; i < len_; i++) {
        if (ch == *(ptr_ + i)) {
            return i;
        }
    }
    return npos;
}

size_t myStringView::find(myStringView sv) {
    if (sv.size() > len_) {
        return npos;
    }
   for (size_t i = 0; i < len_; i++) {
       size_t k = i;
       size_t j = 0;
       while (k < len_ && j < sv.size() && *(ptr_ + k) == sv[j]) {
           k++;
           j++;
       }
       if (j == sv.size()) {
           return i;
       }
   }
    return npos;
}

char myStringView::operator[](size_t index) const {
    return *(ptr_ + index);
}

myStringView myStringView::substr(size_t start, size_t len) const {
    if (start + len > len_) {
        throw std::out_of_range("substr out of index");
    }
    return myStringView(ptr_ + start, len);
}
