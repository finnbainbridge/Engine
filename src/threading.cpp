#include "Engine/Engine.hpp"
// #define ENGINE_NO_THREADING
#include <queue>
#include <mutex>
using namespace Engine;

std::vector<std::thread> pool;

std::queue<Threading::Task> important_tasks;
std::queue<Threading::Task> lesser_tasks;
std::mutex tasks_lock;

std::mutex free_lock;

int free_threads;
int total_threads;

bool running = false;

void Threading::startThreads()
{
    pool = std::vector<std::thread>();
    important_tasks = std::queue<Task>();
    lesser_tasks = std::queue<Task>();
    total_threads = 0;
    free_threads = 0;

#ifndef ENGINE_NO_THREADING
    int amount = std::thread::hardware_concurrency();
    if (amount == 0)
    {
        // This may or may not happen on windows
        // Default to... 4...?
        amount = 4;
    }

    int lesser_threads = 1;
    total_threads = amount - lesser_threads;
    free_threads = total_threads;
    running = true;

    // Thread-up
    for (size_t i = 0; i < amount; i++)
    {
        if (lesser_threads > 0)
        {
            pool.push_back(std::thread(lesserThreadWorker));
            lesser_threads -= 1;
        }
        else {
            pool.push_back(std::thread(threadWorker));
        }
    }
#endif
}

void Threading::addTask(std::function<void()> function)
{
#ifndef ENGINE_NO_THREADING
    tasks_lock.lock();
    Task t;
    t.function = function;
    important_tasks.push(t);
    tasks_lock.unlock();
#else
    function();
#endif
}

void Threading::addLesserTask(std::function<void()> function)
{
#ifndef ENGINE_NO_THREADING
    tasks_lock.lock();
    Task t;
    t.function = function;
    important_tasks.push(t);
    tasks_lock.unlock();
#else
    function();
#endif
}

void Threading::waitForCompletion()
{
#ifndef ENGINE_NO_THREADING
    bool end = false;
    while (!(end))
    {
        tasks_lock.lock();
        if (important_tasks.empty() && total_threads == free_threads)
        {
            end = true;
        }
        tasks_lock.unlock();
    }
#endif
}

void Threading::cleanup()
{
#ifndef ENGINE_NO_THREADING
    running = false;
    for (size_t i = 0; i < pool.size(); i++)
    {
        if (pool[i].joinable())
        {
            pool[i].join();
        }
    }
#endif
}

void Threading::threadWorker()
{
#ifndef ENGINE_NO_THREADING
    Task current_task;
    bool has_task = false;

    // This is the function that runs on the threads
    while (true)
    {
        if (running == false)
        {
            break;
        }

        tasks_lock.lock();
        if (!important_tasks.empty())
        {
            current_task = important_tasks.front();
            important_tasks.pop();
            has_task = true;
            free_lock.lock();
            free_threads -= 1;
            free_lock.unlock();
        }
        tasks_lock.unlock();

        if (has_task == true)
        {
            current_task.function();
            has_task = false;

            free_lock.lock();
            free_threads += 1;
            free_lock.unlock();
        }
    }
#endif
}

void Threading::lesserThreadWorker()
{
#ifndef ENGINE_NO_THREADING
    Task current_task;
    bool has_task = false;

    // This is the function that runs on the threads
    while (true)
    {
        if (running == false)
        {
            break;
        }

        tasks_lock.lock();
        if (!lesser_tasks.empty())
        {
            current_task = lesser_tasks.front();
            lesser_tasks.pop();
            has_task = true;
        }
        tasks_lock.unlock();

        if (has_task == true)
        {
            current_task.function();
            has_task = false;
        }
    }
#endif
}