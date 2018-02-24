#ifndef _ORDERED_LIST_HPP_
#define _ORDERED_LIST_HPP_

#include <list>
#include <map>
#include <unordered_map>
#include <chrono>
#include <functional>

typedef std::function<void(int)> CallbackFunction;

struct TimerT {
  int id;
  int interval;
  CallbackFunction cb;
};

//Timer manager class
class TimerMgr {
  struct TimerListElem {
    TimerListElem() {}
    ~TimerListElem() {}
    TimerListElem(int argint, std::map<int, TimerT> &argm):timeToExp(argint), m(argm) {
    }
    bool empty() {return m.empty();}
    int timeToExp;
    std::map<int, TimerT> m;
  };

  public:
  TimerMgr():m_timerList(),m_id() {}
  ~TimerMgr() {}

  int StartTimer(int timeMs, CallbackFunction cb);
  bool StopTimer(int id);
  bool IsRunning(int id);
  void Tick();

  private:
  int getId() {
    return m_id++;
  }

  //ordered list of timer metadata. We can have multiple timers for the same
  //interval, hence we store it as a map of timers for every interval, where key
  //is timer Id;
  std::list<TimerListElem> m_timerList;
  typedef std::list<TimerListElem>::iterator listIterT;
  std::unordered_map<int, listIterT> m_stopMap;
  long long                   m_id = 0;
};

#endif
