#include <stdint.h>
#include <string>

class myStringView {
public:
	myStringView(const std::string &str);
	~myStringView() = default;
	size_t size() const;
	bool empty() const;
	myStringView substr(uint64_t index) const;
	myStringView operator[](size_t index) const;
	bool start_with(const std::string &str);
	bool end_with(const std::string &str);
	size_t find(const char &ch);
	size_t find(const std::string &str);
private:
	const char *ptr_;
	uint64_t len_;
};
