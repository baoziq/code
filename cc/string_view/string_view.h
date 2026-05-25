#include <stdint.h>
#include <string>

class myStringView {
public:
	myStringView(const std::string &str);
	size_t size() const;
	bool empty() const;
	myStringView substr(uint64_t index) const;
	myStringView operator[](size_t index) const;

private:
	const char *ptr_;
	uint64_t len_;
};
