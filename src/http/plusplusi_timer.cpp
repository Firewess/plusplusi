/*
 *@author: Jie Feng
 *@date: 19-2-17 下午11:53
 */
#include "plusplusi_timer.h"

#define _CRT_SECURE_NO_WARNINGS
#ifdef _MSC_VER
# include <sys/timeb.h>
#else

# include <sys/time.h>

#endif

bool compare_timer(const HeapEntry &timer1, const HeapEntry &timer2)
{
    return timer1.timer->expires_time > timer2.timer->expires_time;
}


//////////////////////////////////////////////////////////////////////////
// Timer

Timer::Timer(TimerManager &manager)
        : timer_manager(manager), heap_index(-1)
{
}

Timer::~Timer()
{
    stop();
}

void Timer::start(TIMER_CALL_BACK_FUN fun, Epoll_Data_S* ptr, unsigned interval, TimerType timeType)
{
    stop();
    this->interval = interval;
    this->callback_fun = fun;
    this->epoll_data_ptr = ptr;
    this->timer_type = timeType;
    this->expires_time = interval + TimerManager::get_curr_microsec();
    timer_manager.add_timer(this);
}

void Timer::stop()
{
    if (heap_index != -1)
    {
        timer_manager.del_timer(this);
        heap_index = -1;
    }
}

void Timer::time_out(unsigned long long now)
{
    if (timer_type == Timer::CIRCLE)
    {
        expires_time = interval + now;
        timer_manager.add_timer(this);
    } else
    {
        heap_index = -1;
    }
    callback_fun(epoll_data_ptr);
}

//////////////////////////////////////////////////////////////////////////
// TimerManager

void TimerManager::add_timer(Timer *timer)
{
    timer->heap_index = timer_min_heap.size();
    HeapEntry entry = {timer->expires_time, timer};
    timer_min_heap.push_back(entry);
    std::push_heap(timer_min_heap.begin(), timer_min_heap.end(), compare_timer);
}

void TimerManager::del_timer(Timer *timer)
{
    size_t index = timer->heap_index;
    if (!timer_min_heap.empty() && index < timer_min_heap.size())
    {
        if (index == -1) return;    //avoid re-delete
        if (index == timer_min_heap.size() - 1)
        {
            timer_min_heap.pop_back();
        } else
        {
            swap_heap(index, timer_min_heap.size() - 1);
            timer_min_heap.pop_back();
            std::make_heap(timer_min_heap.begin(), timer_min_heap.end(), compare_timer);
        }
    }
}

void TimerManager::work_cycle()
{
    unsigned long long now = get_curr_microsec();
    while (!timer_min_heap.empty() && timer_min_heap[0].expire_time <= now)
    {
        Timer *timer = timer_min_heap[0].timer;
        del_timer(timer);
        timer->time_out(now);
    }
}

void TimerManager::swap_heap(size_t index1, size_t index2)
{
    HeapEntry tmp = timer_min_heap[index1];
    timer_min_heap[index1] = timer_min_heap[index2];
    timer_min_heap[index2] = tmp;
    timer_min_heap[index1].timer->heap_index = index1;
    timer_min_heap[index2].timer->heap_index = index2;
}

unsigned long long TimerManager::get_curr_microsec()
{
#ifdef _MSC_VER
    _timeb timebuffer;
    _ftime(&timebuffer);
    unsigned long long ret = timebuffer.time;
    ret = ret * 1000 + timebuffer.millitm;
    return ret;
#else
    timeval tv{};
    ::gettimeofday(&tv, nullptr);
    unsigned long long ret = tv.tv_sec;
    return ret * 1000 + tv.tv_usec / 1000;
#endif
}