#ifndef _CHANNEL_H_
#define _CHANNEL_H_ 1

#include <queue>
#include <mutex>
#include <condition_variable>

/*
 * 使用条件锁的线程安全队列
 */
template <typename T>
class Channel
{
  public:
    Channel(size_t size = 512)
    {
        max_size_ = size;
        //queue_
    }
    T pop()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        auto item = queue_.front();
        queue_.pop();
        return item;
    }

    void pop(T &item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        item = queue_.front();
        queue_.pop();
    }

    void push(const T &item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        if(max_size_<=queue_.size()){
            return;
        }
        queue_.push(item);
        if(queue_.size()==1){
            cond_.notify_one();
        }
    }

    void push(T &&item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        if (max_size_ <= queue_.size())
        {
            return;
        }
        queue_.push(std::move(item));
        if (queue_.size() == 1)
        {
            cond_.notify_one();
        }
    }

  private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    size_t max_size_=0;
};

#endif