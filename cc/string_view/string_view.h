#include <cstdint>
#include <stdint.h>
#include <cstddef>

class myStringView {
public:
    myStringView();
    myStringView(const char* str);
	myStringView(const char *ptr, size_t len);
	~myStringView() = default;
	size_t size() const;
	bool empty() const;
	myStringView substr(size_t start, size_t len) const;
	char operator[](size_t index) const;
	bool start_with(myStringView sv);
	bool end_with(myStringView sv);
	size_t find(const char &ch);
	size_t find(myStringView sv);
	static constexpr size_t npos = static_cast<size_t>(-1);
private:
	const char *ptr_;
	size_t len_;
};
