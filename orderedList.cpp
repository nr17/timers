#include <chrono>
#include <cstdio>

typedef std::function<void(int)> CallbackFunction;

struct TimerT {
    int                                            id;
    std::chrono::high_resolution_clock::time_point absTime;
    int                                            interval;
    CallbackFunction                               cb;
};

class Timer {
    Timer() : m_timerList(), m_startTime() {
        m_tickThread = std::thread(&Timer::startTick);
    }

    ~Timer() {
    }

   public:
    int StartTimer(int timeMs, CallbackFunction cb);
    bool StopTimer(int id);

   private:
    std::list<map<int, TimerT>> m_timerList;
    std::chrono::high_resolution_clock::time_point m_startTime;
    long long                                      m_id = 0;

    int getId() {
        return m_id++;
    }

    void startTick();
};

int Timer::startTick() {
    while (true) {
        std::this_thread::sleep_for(2s);
        processTimers();
    }
}

int Timer::startTimer(int timeMs, CallbackFunction cb) {
    std::chrono::high_resolution_clock::time_point now =
        std::chrono::high_resolution_clock::now();
    // first timer
    if (m_timerList.empty()) {
        TimerT t(getId(), now, timeMs, cb);
        m_timerList.emplace_back(t);
        m_startTime = now;
        return t.id;
    }

    duration<double, std::milli> time_span = now - m_startTime;

    int relativeTimeMs = time_span

        auto it = timerList.begin();
    for (; it != timerList.end(); ++it) {
      if (it->interval
    }
}

int Timer::stopTimer(int id) {
}

int main() {
}
