#include <cstddef>
#include <string_view>
#include <vector>

class Buffer {
public:
    Buffer(size_t capacity);
    size_t ReadableBytes() const;
    size_t WritableBytes() const;
    std::string_view peek(size_t start, size_t end);
    bool append(char *ch, size_t len);

private:
    size_t read_index_;
    size_t write_index_;
    size_t capacity_;
    std::vector<char> buffer_;
};
