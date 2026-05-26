#include "buffer.h"

Buffer::Buffer(size_t capacity) : capacity_(capacity), buffer_(capacity) {}

size_t Buffer::ReadableBytes() const {
    return write_index_ - read_index_;
}

size_t Buffer::WritableBytes() const {
    return capacity_ - write_index_;
}

bool Buffer::append(char *ch, size_t len) {

}

std::string_view Buffer::peek(size_t start, size_t end) {
    std::string_view sv(buffer_.data() + start, end - start);
    return sv;
}
