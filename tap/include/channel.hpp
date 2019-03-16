#ifndef _CHANNEL_H_
#define _CHANNEL_H_ 1

// #include <queue>
// #include <mutex>
// #include <condition_variable>

// /*
//  * 使用条件锁的线程安全队列
//  */
// template <typename T>
// class Channel
// {
//   public:
//     Channel(size_t size = 512)
//     {
//         max_size_ = size;
//         //queue_
//     }
//     T pop()
//     {
//         std::unique_lock<std::mutex> mlock(mutex_);
//         while (queue_.empty())
//         {
//             cond_.wait(mlock);
//         }
//         auto item = queue_.front();
//         queue_.pop();
//         return item;
//     }

//     void pop(T &item)
//     {
//         std::unique_lock<std::mutex> mlock(mutex_);
//         while (queue_.empty())
//         {
//             cond_.wait(mlock);
//         }
//         item = queue_.front();
//         queue_.pop();
//     }

//     void push(const T &item)
//     {
//         std::unique_lock<std::mutex> mlock(mutex_);
//         if(max_size_<=queue_.size()){
//             return;
//         }
//         queue_.push(item);
//         if(queue_.size()==1){
//             cond_.notify_one();
//         }
//     }

//     void push(T &&item)
//     {
//         std::unique_lock<std::mutex> mlock(mutex_);
//         if (max_size_ <= queue_.size())
//         {
//             return;
//         }
//         queue_.push(std::move(item));
//         if (queue_.size() == 1)
//         {
//             cond_.notify_one();
//         }
//     }

//   private:
//     std::queue<T> queue_;
//     std::mutex mutex_;
//     std::condition_variable cond_;
//     size_t max_size_=0;
// };

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

template <typename T>
class Channel
{
  public:
    Channel(int = 512) {}
    ~Channel() {}

    void push(const T &new_data)
    {
        std::lock_guard<std::mutex> lk(mut);  // 1.全局加锁
        data_queue.push(new_data); // 2.push时独占锁
        cond.notify_one();
    }

    //move
    void push(T &&new_data)
    {
        std::lock_guard<std::mutex> lk(mut);  // 1.全局加锁
        data_queue.push(std::move(new_data)); // 2.push时独占锁
        cond.notify_one();
    }

    void wait_and_pop(T &val)
    {
        std::unique_lock<std::mutex> ulk(mut);                    // 3.全局加锁
        cond.wait(ulk, [this]() { return !data_queue.empty(); }); // 4.front 和 pop_front时独占锁
        val = std::move(data_queue.front());
        data_queue.pop();
    }
    // std::shared_ptr<T> wait_and_pop()
    // {
    // 	std::unique_lock<std::mutex> ulk(mut);
    // 	cond.wait(ulk,[this]() { return !data_queue.empty(); });
    // 	std::shared_ptr<T> val(std::make_shared<T>(std::move(data_queue.front())));
    // 	data_queue.pop();
    // 	return val;
    // }
    T pop()
    {
        std::unique_lock<std::mutex> ulk(mut);
        cond.wait(ulk, [this]() { return !data_queue.empty(); });
        auto item = data_queue.front();
        data_queue.pop();
        return item;
    }

    bool try_pop(T &val)
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return false;
        val = std::move(data_queue.front());
        data_queue.pop();
        return true;
    }
    std::shared_ptr<T> try_pop()
    {
        std::shared_ptr<T> val;
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return val;
        val = std::make_shared<T>(std::move(data_queue.front()));
        data_queue.pop();
        return val;
    }
    bool empty()
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }

  private:
    std::queue<T> data_queue;
    std::mutex mut;
    std::condition_variable cond;
};

#endif