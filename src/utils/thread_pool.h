// Forked from https://gist.github.com/jrandom/ddb986d4e2c6e91926657736e97d2d54
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <future>


namespace RTW::Utils {
class ThreadPool {
    public:
        using Mutex_t = std::recursive_mutex;
        using Unique_Lock_t = std::unique_lock<Mutex_t>;
        using Condition_t = std::condition_variable_any;
        using Threads_t = std::vector<std::thread>;

        using Task_Function_t = std::function<void()>;
        using Task_Queue_t = std::queue<Task_Function_t>;

    private:
        Threads_t m_threads;
        Task_Queue_t m_task_queue;
        Condition_t m_pool_notifier;
        Mutex_t m_queue_mutex;

        bool m_should_stop;
        bool m_is_emergency_stop;
        bool m_is_paused;

    public:
        ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}
        ThreadPool(int worker_count)
            : m_should_stop(false), m_is_emergency_stop(false), m_is_paused(false) 
        {
            if(worker_count == 0)
                throw std::runtime_error("ERR: RTW::Utils::ThreadPool() should have at least 1 worker!\n");
            
            m_threads.reserve(worker_count);
            for(int i = 0; i < worker_count; ++i) {
                m_threads.emplace_back([this](){ Worker(); });
            }
        }

        ~ThreadPool() {
            // Set stop flag
            {
                Unique_Lock_t queue_lock(m_queue_mutex);
                m_should_stop = true;
            }

            // Wake up all threads and wait for exit
            m_pool_notifier.notify_all();
            for(auto& thread : m_threads) {
                if(thread.joinable())
                    thread.join();
            }
        }

        // Delete move and copy constructors
        ThreadPool(const ThreadPool& source) = delete;
        ThreadPool& operator=(const ThreadPool& source) = delete;


        template<typename Function_t, typename... Args>
        auto AddTask(Function_t&& function, Args&&... args) 
            -> std::future<typename std::result_of<Function_t(Args...)>::type>
        {
            using return_t = typename std::result_of<Function_t(Args...)>::type;

            // Create packaged task
            auto task = std::make_shared<std::packaged_task<return_t()>> (
                std::bind(
                    std::forward<Function_t>(function),
                    std::forward<Args>(args)...
                )
            );

            std::future<return_t> result = task->get_future();

            // Add task to queue
            {
              Unique_Lock_t queue_lock(m_queue_mutex);
              m_task_queue.emplace([task](){ (*task)(); });  
            }

            m_pool_notifier.notify_one();
            return result;
        }
    
    private:
        void Worker() {
            while(true) {
                Task_Function_t task;

                {
                    // Wait on task or 'stop'
                    Unique_Lock_t queue_lock(m_queue_mutex);
                    m_pool_notifier.wait(queue_lock,
                                         [this](){
                                             return (!m_task_queue.empty() && !m_is_paused)
                                                    || m_should_stop
                                                    || m_is_emergency_stop;
                                        }
                                        );
                    if((m_should_stop &&m_task_queue.empty()) || m_is_emergency_stop)
                        return;
                    
                    task = std::move(m_task_queue.front());
                    m_task_queue.pop();
                }

                task();
            }
        }
};
} // namespace RTW::Utils

#endif