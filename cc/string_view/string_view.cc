#include "string_view.h"

myStringView::myStringView(const std::string &str)
    : ptr_(str.data()), len_(str.size()) {}

size_t myStringView::size() const { return len_; }

bool myStringView::empty() const { return len_ == 0; }
