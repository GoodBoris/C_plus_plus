#ifndef SCHEDULED_EXECUTOR_H
#define SCHEDULED_EXECUTOR_H

#include <list>
#include <chrono>
#include <algorithm>
#include "ThreadPool.h"


class ScheduledExecutor {
public:
    // Уникальный в рамках одного executor-а идентификатор задачи.
    // Нужно определить самим, какого он будет типа. Можно взять,
    // например, size_t.
    typedef size_t TaskID;

    
public:
    explicit ScheduledExecutor(size_t);
    ~ScheduledExecutor();
    ScheduledExecutor(ScheduledExecutor const &) = delete;
    ScheduledExecutor(ScheduledExecutor &&) = delete;
    ScheduledExecutor & operator = (ScheduledExecutor const &) = delete;
    ScheduledExecutor & operator = (ScheduledExecutor &&) = delete;
    
    // Запускает отложенную задачу, которая будет выполнена 1 раз с
    // заданным delay. Если delay == 0, задача будет немедленно
    // поставлена в очередь.
    // Параметры:
    // 	fn - функция, которую следует запустить.
    //	delay - время в миллисекундах, через которое нужно
    //	запустить задачу.
    template<typename Fn>
    TaskID ScheduleDelayedTask(Fn && fn, long delay = 0) {
        
        return SchedulePeriodicTask(std::forward<Fn>(fn), delay, 0);
    }
    
    // Запускает отложенную задачу, которая будет выполнена 1 раз с
    // заданным delay. Если delay == 0, задача будет _немедленно_
    // запущена.
    // Параметры:
    // 	fn - функция, которую следует запустить.
    //	delay - время, через которое нужно запустить задачу, в
    //	миллисекундах.
    //	period - время в миллисекундах, через которое задача будет
    //	повторяться.
    template<typename Fn>
    TaskID SchedulePeriodicTask(Fn && fn, long delay = 0, long period = 0) {
        
        if (stop)
            throw std::runtime_error("ScheduledExecutor was stopped.");
        std::unique_lock<std::mutex> lock(mutex);
        tasks.emplace_back(std::forward<Fn>(fn), id, delay, period);
        lock.unlock();
        condition.notify_one();
        return id++;
    }
    
    // Запускает задачу, которая будет посчитана только в тот
    // момент, когда на объекте std::future, возвращаемом этой
    // функцией, будет вызван метод get().
    // Параметры:
    //	fn - задача, которая будет запущена.
    template<typename Fn, typename... Args>
    auto ScheduleLazyTask(Fn&& fn, Args&&... args) ->
    std::future < typename std::result_of<Fn(Args...)>::type > {
        if (stop)
            throw std::runtime_error("ScheduledExecutor was stopped.");
        return std::async(std::launch::deferred, fn, args...);
    }
    
    // Прекращает запуски задания с заданным id. Если в данный
    // момент это задание выполняется, то прерывать выполнение не
    // требуется.
    void CancelPeriodicTask(TaskID const & id) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            tasks.remove_if([id](const Task& t) { return t._id == id; });
        }
        condition.notify_one();
    }
    
    // Прекращает запуски всех заданий. Если какие-то из них
    // выполняются в данный момент, то прерывать их выполнение не
    // требуется.
    void Shutdown();
    
private:
    void run() {
        while (!stop) {
            std::unique_lock<std::mutex> lock(mutex);
            if (tasks.empty())
                condition.wait(lock);
            if (stop)
                return;
            auto priorityTask = std::min_element(tasks.begin(), tasks.end(), [](const Task& t, const Task& p)
            {
                return t._execTime < p._execTime;
            });
            auto condition_status = condition.wait_until(lock, priorityTask->_execTime);
            if (stop)
                return;
            if (std::cv_status::timeout == condition_status)
                threadPool.enqueue(priorityTask->_fn);
            if (std::cv_status::no_timeout == condition_status)
                continue;
            if (!priorityTask->_period)
                tasks.erase(priorityTask);
            else
                priorityTask->_execTime += std::chrono::milliseconds(priorityTask -> _period + priorityTask -> _delay);
        }

    }

    struct Task {
        Task(std::function<void()> fn, ScheduledExecutor::TaskID id, long delay, long period) :
        _fn(fn), _id(id), _delay(delay), _period(period), _execTime(std::chrono::milliseconds(delay) + std::chrono::system_clock::now()) { }
        std::function<void()> _fn = nullptr;
        ScheduledExecutor::TaskID _id = 0;
        long _delay = 0;
        long _period = 0;
        std::chrono::system_clock::time_point _execTime = std::chrono::system_clock::time_point();
    };
    TaskID id = 0;
    std::list<Task> tasks;
    std::thread thread;
    ThreadPool threadPool;
    std::atomic<bool> stop {false};
    std::mutex mutex;
    std::condition_variable condition;
};


inline ScheduledExecutor::ScheduledExecutor(size_t threadPoolSize) : threadPool(threadPoolSize),
                                thread(&ScheduledExecutor::run, this) { }

inline void ScheduledExecutor::Shutdown() {
    stop = true;
    
    {
        std::unique_lock<std::mutex> lock(mutex);
        tasks.clear();
    }
    condition.notify_one();
    thread.join();
}

inline ScheduledExecutor::~ScheduledExecutor() {
    Shutdown();
}
    
#endif
