
#include <iostream>
#include "ScheduledExecutor.h"

size_t cnt = 0;

void printFunction() {
    std::cout << "This is the print function " << cnt++ << std::endl;
}

void checkLazyTasks() {
    ScheduledExecutor scheduledExecutorService(4);
    std::vector< std::future<int> > results;
    
    for(int i = 0; i < 8; ++i) {
        results.emplace_back(
                             scheduledExecutorService.ScheduleLazyTask([i] {
            std::cout << "hello " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "world " << i << std::endl;
            return i*i;
        })
                             );
    }
    for(auto && result: results)
        std::cout << result.get() << ' ';
    std::cout << std::endl;
}

void checkSimpleDelayedTasks() {
    ScheduledExecutor scheduledExecutorService(4);
    std::cout << "id = " << scheduledExecutorService.ScheduleDelayedTask(printFunction, 1000) << "; delay = 1000" << std::endl;
    std::cout << "id = " << scheduledExecutorService.ScheduleDelayedTask(printFunction, 0) << " delay = 0" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

void checkPeriodicTasks()
{
    ScheduledExecutor scheduledExecutorService(4);
    std::cout << "id = " << scheduledExecutorService.SchedulePeriodicTask(printFunction, 0, 1000) << " period = 1000" << std::endl;
    std::cout << "id = " << scheduledExecutorService.SchedulePeriodicTask(printFunction, 1000, 1000) << " delay = 100; period = 1000" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    scheduledExecutorService.CancelPeriodicTask(0);
    scheduledExecutorService.CancelPeriodicTask(1);
}

void checkCombainTasks()
{
    ScheduledExecutor scheduledExecutorService(4);
    std::cout << "id = " << scheduledExecutorService.SchedulePeriodicTask(printFunction, 0, 1000) << " period = 1000" << std::endl;
    std::cout << "id = " << scheduledExecutorService.ScheduleDelayedTask(printFunction, 0) << " delay = 0" << std::endl;
    std::vector< std::future<int> > results;
    for(int i = 0; i < 8; ++i) {
        results.emplace_back(
                             scheduledExecutorService.ScheduleLazyTask([i] {
            std::cout << "hello " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "world " << i << std::endl;
            return i*i;
        })
                             );
    }
    for(auto && result: results)
        std::cout << result.get() << ' ';
    std::cout << std::endl;
}

int main()
{
    checkLazyTasks();
    checkPeriodicTasks();
    checkSimpleDelayedTasks();
    checkCombainTasks();
    std::cout << "End" << std::endl;
    return 0;
}