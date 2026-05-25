#include "string_view.h"

myStringView::myStringView(const std::string &str)
    : ptr_(str.data()), len_(str.size()) {}

size_t myStringView::size() const {
    return len_;
}

bool myStringView::empty() const {
    return len_ == 0;
}

bool myStringView::start_with(const std::string &str) {
    if (str.size() > len_) {
        return false;
    }
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] != *(ptr_ + i)) {
            return false;
        }
    }
    return true;
}

bool myStringView::end_with(const std::string &str) {
    if (str.size() > len_) {
        return false;
    }
    for (size_t i = str.size() - 1; i >= 0; i--) {
        if (str[i] != *(ptr_ + len_ - 1 - i)) {
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

size_t myStringView::find(const std::string &str) {

    return -1;
}
