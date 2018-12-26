#ifndef _H_WaitGroup_H_
#define _H_WaitGroup_H_
#include <thread>
#include <mutex>

//暂时先这么写吧
class WaitGroup{
public:
    //添加n个任务
    void add(int n){
        //使用RAII的锁
        std::unique_lock<std::mutex> lk(nmtx);
        num_+=n;
    }

    //添加一个任务
    void add(){add(1);}

    //完成一个任务
    void down(){down(1);}

    //完成n个任务
    void down(int n){
        //如果都已经完成了就返回
        if(num_<=0)return;
        //上锁
        std::unique_lock<std::mutex> lk(nmtx);
        num_-=n;
        //先解锁
        lk.unlock();
        //如果任务都完成就通知所有等待的线程
        if(num_<=0){
            cv_.notify_all();
            return;
        }
    }

    //等待任务完成
    void wait(){
        if(num_<=0)return;
        std::unique_lock<std::mutex> lk(cmtx);
        //等待任务完成
        cv_.wait(lk,[&]{return num_<=0;});
    }

private:
    //当前要等待的任务数
    int num_=0;

    //锁
    std::mutex nmtx;

    //条件变量
    std::condition_variable cv_;

    //条件变量锁
    std::mutex cmtx;
};

#endif