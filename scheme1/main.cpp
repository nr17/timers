#include <cstdio>
#include <chrono>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "orderedList.hpp"

using namespace std;
typedef std::function<void(int)> Cb_t;

TimerMgr tmrMgr;

class EventLoop {
    public:
        EventLoop() {
            m_epollFd = epoll_create (m_size);
            if (m_epollFd == -1) {
                std::cerr << strerror (errno) << "\n";
            }
        }

        ~EventLoop() {
            close(m_epollFd);
        } 

        bool addFd(int fd, Cb_t cb) {
            epoll_event event;

            // add timer to reactor
            event.events = EPOLLIN; // notification is a read event
            event.data.fd = fd; // user data
            int r = epoll_ctl (m_epollFd, EPOLL_CTL_ADD, fd, &event);
            if (r == -1) {
                std::cerr << strerror (errno) << "\n";
                close (m_epollFd);
                close (fd);
                return false;
            }
            m_cbMap[fd] = cb;
            return true;
        }

        bool start() {
            while (true) {
                const int infinity = -1;
                // and wait for events
                epoll_event events[m_size];
                int r = epoll_wait (m_epollFd, events, m_size, infinity);
                if (r == -1) {
                    close (m_epollFd);
                    return false;
                }

                // demultiplex events
                int i = 0;
                while (i < r) {
                    Cb_t cb = m_cbMap[events[i].data.fd];
                    cb(events[i].data.fd);
                    ++i;
                }
            }
            return true;
        }

    private:
        const int m_size = 10; //ignored by newer linux kernels
        int m_epollFd = -1;
        unordered_map<int, Cb_t> m_cbMap;
};

void tickCb(int fd) {
    // timer is triggered
    size_t s = 0;
    int i = read (fd, &s, sizeof (s));
    if (i != -1) {
        std::cout << "Timer triggered (" << s << ")\n";
        tmrMgr.Tick();
    }
}

bool startTick(EventLoop &evtLoop) {
    int fd = timerfd_create (CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (fd == -1) {
        std::cerr << strerror (errno);
        return false;
    }

    // add event to be triggered
    int flags = 0;
    itimerspec new_timer;
    // 1-second periodic timer
    new_timer.it_interval.tv_sec = 1;
    new_timer.it_interval.tv_nsec = 0;
    new_timer.it_value.tv_sec = 1;
    new_timer.it_value.tv_nsec = 0;
    itimerspec old_timer;
    int i = timerfd_settime (fd, flags, &new_timer, &old_timer);
    if (i == -1) {
        std::cerr << strerror (errno) << "\n";
        close (fd);
        return false;
    }

    return evtLoop.addFd(fd, tickCb);
}


int main() {
    EventLoop evtLoop;
    startTick(evtLoop);

    //start timers
    int t5 = tmrMgr.StartTimer(5000, [](int id) { 
            std::cout << "5 sec timer expired, id=" << id << std::endl;
            });

    printf("is t5 running:%s\n", tmrMgr.IsRunning(t5)?"true":"false");

    int t7 = tmrMgr.StartTimer(7000, [](int id) { 
            std::cout << "7 sec timer expired, id=" << id << std::endl;
            });

    int t3 = tmrMgr.StartTimer(3000, [](int id) { 
            std::cout << "3 sec timer expired, id=" << id << std::endl;
            });
    tmrMgr.StopTimer(t3);
    printf("is t3 running:%s\n", tmrMgr.IsRunning(t3)?"true":"false");

    int t4 = tmrMgr.StartTimer(4000, [](int id) { 
            std::cout << "4 sec timer expired, id=" << id << std::endl;
            std::cout << "starting another 10s timer" << std::endl;
            int t10 = tmrMgr.StartTimer(10000, [](int id2) { 
                    std::cout << "10 sec timer expired, id=" << id2 << std::endl;
                    });
            });

    evtLoop.start();
}
