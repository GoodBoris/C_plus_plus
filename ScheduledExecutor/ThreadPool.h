#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
	ThreadPool(size_t);
	template<typename Fn, typename... Args>
	auto enqueue(Fn&& fn, Args&&... args)
		->std::future<typename std::result_of<Fn(Args...)>::type>;
	~ThreadPool();
private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex queue_mutex;
	std::condition_variable condition;
    std::atomic<bool> stop {false};
};

inline ThreadPool::ThreadPool(size_t threads) {
	if (!threads)
		throw std::invalid_argument("Pool size can not be 0.");
	for (size_t i = 0; i<threads; ++i)
		workers.emplace_back(
		[this]
	{
		for (;;) {
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(this->queue_mutex);
				this->condition.wait(lock, [this] { 
					return this->stop || !this->tasks.empty(); });
				if (this->stop && this->tasks.empty())
					return;
				task = std::move(this->tasks.front());
				this->tasks.pop();
			}
			task();
		}
	}
	);
}

template<typename Fn, typename... Args>
auto ThreadPool::enqueue(Fn&& fn, Args&&... args)
-> std::future<typename std::result_of<Fn(Args...)>::type> {
	using return_type = typename std::result_of<Fn(Args...)>::type;

	auto task = std::make_shared<std::packaged_task<return_type()>>
		(std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));

	std::future<return_type> res = task->get_future();
	if (stop) throw std::runtime_error("ThreadPool was stopped");
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		tasks.emplace([task](){ (*task)(); });
	}
	condition.notify_one();
	return res;
}

inline ThreadPool::~ThreadPool()
{
	stop = true;
	condition.notify_all();
	for (std::thread &worker : workers)
		worker.join();
}

#endif