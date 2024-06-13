#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <iostream>
#include <chrono>
#include <syncstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

class ThreadPool 
{
    using JobType = std::function<void()>;
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
private:
    std::vector<std::thread> m_workers;
    std::queue<JobType> m_tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    void run(int);
    bool stop;
};

inline ThreadPool::ThreadPool(size_t threads): stop(false) 
{
    for(size_t i = 1;i<=threads;++i)
        m_workers.emplace_back( &ThreadPool::run, std::ref(*this), i);
}

void ThreadPool::run(int id)
{
    std::osyncstream(std::cout)<<"Starting the thread "<<id<<' '<<std::this_thread::get_id()<<' '<<syscall(SYS_gettid)<<'\n';
    for(;;)
    {
        JobType job;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            condition.wait(lock, [this](){ return !m_tasks.empty();});
            if(stop && m_tasks.empty())
                return;
            job = m_tasks.front(); m_tasks.pop();
        }
        job();
    }
}

// so this enque can take any functaion as an argument, returns future as res, so caller can get the result back
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> 
{
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()> >( std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        m_tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}
// simple job type 
/*
void ThreadPool::enqueue(JobType f)  
{
    {   
        std::unique_lock<std::mutex> lock(queue_mutex);
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace(f);
    }   
    condition.notify_one();
}*/

inline ThreadPool::~ThreadPool() 
{
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: m_workers)
        worker.join();
}


int Func(int i, int j)
{
    std::osyncstream(std::cout)<<"Func gets called \n";
    return i + j;
}
int main() {
    // Create a ThreadPool with 4 worker threads
    ThreadPool pool(4);

    // Enqueue a few tasks
    for(int i = 0; i < 8; ++i) 
    {
        pool.enqueue([i]
        {
            std::osyncstream(std::cout) << "Task " << i << " is being processed by thread " << std::this_thread::get_id() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::osyncstream(std::cout) << "Task " << i << " has been processed." << std::endl;
        });
    }

    std::future<int> result = pool.enqueue(Func, 10, 20);
    std::osyncstream(std::cout)<<"Func Result "<<result.get()<<'\n';

    // The ThreadPool will be automatically cleaned up when it goes out of scope
    return 0;
}
