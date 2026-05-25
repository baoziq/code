#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
  public:
	ThreadPool(size_t num_threads);
	void enqueue(std::function<void()> task);
	~ThreadPool();

  private:
	std::vector<std::thread> worker_;
	std::queue<std::function<void()>> tasks_;
	std::mutex mtx_;
	std::condition_variable cv_;
	bool stop_;
};
