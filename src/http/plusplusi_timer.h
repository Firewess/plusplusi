/*
 *@author: Jie Feng
 *@desription: this file implements the timer class used for http keep-alive
 *@reference: https://www.cnblogs.com/junye/p/5836552.html, I rewrite the code used for
 * min heap based on C++11 STL
 *@date: 19-2-17 下午11:53
 */
#ifndef _plusplusi_timer_H
#define _plusplusi_timer_H

#include <algorithm>
#include <vector>
#include "plusplusi_epoll.h"

class TimerManager;
struct HeapEntry;

typedef void (*TIMER_CALL_BACK_FUN)(Epoll_Data_S *);

class Timer
{
public:
    enum TimerType
    {
        ONCE, CIRCLE
    };

    explicit Timer(TimerManager &manager);
    ~Timer();
    void start(TIMER_CALL_BACK_FUN fun, Epoll_Data_S* ptr, unsigned interval, TimerType timeType = ONCE);
    void stop();

private:
    void time_out(unsigned long long now);

private:
    friend class TimerManager;
    friend bool compare_timer(const HeapEntry&, const HeapEntry&);

    TimerManager &timer_manager;
    TimerType timer_type;
    TIMER_CALL_BACK_FUN  callback_fun;
    unsigned interval;
    unsigned long long expires_time;

    size_t heap_index;

    Epoll_Data_S* epoll_data_ptr;
};

struct HeapEntry
{
    unsigned long long expire_time;
    Timer *timer;
};

class TimerManager
{
public:
    static unsigned long long get_curr_microsec();
    void work_cycle();

private:
    friend class Timer;
    void add_timer(Timer *timer);
    void del_timer(Timer *timer);
    void swap_heap(size_t index1, size_t index2);

    std::vector<HeapEntry> timer_min_heap;
};

#endif //_plusplusi_timer_H
