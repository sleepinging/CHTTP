#ifndef _CHANNEL_H_
#define _CHANNEL_H_ 1

// #include <stdio.h>
// #include <unistd.h>
// #include <semaphore.h>
// #include <pthread.h>
// #include <time.h>
// #include <queue>

// using namespace std;

// template <typename T>
// class Channel
// {
//     public:
//         Channel(unsigned int maxsize)
//         {
//             _maxsize = maxsize;
//             sem_init(&queueSpace, 0, maxsize);
//             sem_init(&datalen, 0, 0);
//             pthread_mutex_init(&datalock, NULL);
//             data = new T[maxsize];
//             begin = 0;
//             end = 0;
//         }
    
//         bool push(T t,int maxtimes = 0, time_t tryinterval = 0)
//         {
//             if(maxtimes > 0)
//             {
//                 int trytimes = 0;
//                 int result = -1;
//                 while(1)
//                 {
//                     trytimes++;
//                     result = sem_trywait(&queueSpace);
//                     if(result == 0 || trytimes >= maxtimes)
//                         break;
//                     else
//                         usleep(tryinterval);
//                 }
//                 if(result != 0)
//                     return false;
//             }
//             else
//             {
//                 sem_wait(&queueSpace);
//             }
//             pthread_mutex_lock(&datalock);
//             data[end] = t;
//             end = (end + 1) % _maxsize;
//             pthread_mutex_unlock(&datalock);
//             sem_post(&datalen);
//             return true;
//         }

//         T pop()
//         {
//             //printf("start pop,datalen:%d,wait...\n",datalen);
//             //等待队列中有数据
//             sem_wait(&datalen);
//             //printf("datalock\n");
//             pthread_mutex_lock(&datalock);
//             T t = data[begin];
//             begin = (begin + 1) % _maxsize;
//             pthread_mutex_unlock(&datalock);
//             //printf("sem_post\n");
//             sem_post(&queueSpace);
//             return t;
//         }

//         ~Channel()
//         {
//             pthread_mutex_destroy(&datalock);
//             delete[] data;
//         }

//     private:
//         int _maxsize;
//         int begin;
//         int end;
//         T *data;
//         pthread_mutex_t datalock;
//         sem_t queueSpace;
//         sem_t datalen;
// };

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>

//注意缓冲不能是0，无缓冲的通道有空再写
template <typename T>
class Channel{
public:
    Channel(size_t bufsize=0){
        if(bufsize<=0){
            throw "bufsize must bigger than 0";
        }
        bufsize_=bufsize;
    }

    //弹出元素,并获取值,若队列已空会阻塞
    T pop(){
        std::unique_lock<std::mutex> clk(cvmtx_);
        cv_.wait(clk,[&]{return que_.size()>0;});
        //此时队列非空
        //上锁
        std::unique_lock<std::mutex> lk(mtx_);
        auto r=que_.front();
        que_.pop();
        //如果之前是满的
        if(que_.size()==bufsize_-1){
            //通知压入时阻塞的函数
            cv_.notify_one();
        }
        return r;
    }

    //压入元素,若队列已满会阻塞
    bool push(T v,int maxtimes=0,int tryinterval=0){
        if(maxtimes>0)
        {
            while(1){
                if(--maxtimes<0)return false;
                if(que_.size()<bufsize_){
                    break;
                }else{
                    std::this_thread::sleep_for(std::chrono::seconds(tryinterval));
                    continue;
                }
                
            }
        }
        
        std::unique_lock<std::mutex> clk(cvmtx_);
        cv_.wait(clk,[&]{return que_.size()<bufsize_;});
        //此时队列未满
        //上锁
        std::unique_lock<std::mutex> lk(mtx_);
        que_.push(v);
        //如果之前是空的
        if(que_.size()==1){
            //通知弹出时阻塞的函数
            cv_.notify_one();
        }
        return true;
    }

    //获取目前通道元素个数
    size_t getsize(){
        return que_.size();
    }

    //获取通道缓冲大小(最大元素个数)
    size_t getbuffsize(){
        return bufsize_;
    }

    Channel(const Channel&)=delete;
    Channel& operator = (const Channel&)=delete;
private:
    //元素队列
    std::queue<T> que_;

    //缓冲大小
    size_t bufsize_=0;

    //pop和push的锁
    std::mutex mtx_;
    
    //条件变量
    std::condition_variable cv_;

    //条件变量锁
    std::mutex cvmtx_;
};

#endif