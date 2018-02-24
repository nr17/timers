#include "orderedList.hpp"

// case 1) first timer
// case 2) 10 --> 5(15) --> 3(18). Add 5 at t=2
// case 3) 10 --> 5(15) --> 3(18). Add 5 at t=7
// case 4) 10 --> 5(15) --> 3(18). Add 15 at t=2
// case 5) 10 --> 5(15) --> 3(18). Add 30 at t=5

int TimerMgr::StartTimer(int timeMs, CallbackFunction cb) {
    TimerT t{getId(), timeMs, cb};
    //first timer
    if (m_timerList.empty()) {
        std::map<int, TimerT> m;
        m[t.id] = t;
        m_timerList.emplace_back(TimerListElem(timeMs, m));
        m_stopMap[t.id] = std::prev(m_timerList.end());
        return t.id;
    }

    auto it = m_timerList.begin();
    bool added = false;
    int relativeTimeMs = timeMs;
    for (; it != m_timerList.end(); ++it) {
        std::map<int, TimerT> &m = it->m;
        TimerListElem elem = *it;
        if (relativeTimeMs == elem.timeToExp) {
            m[t.id] = t;
            m_stopMap[t.id] = it;
            added = true;
            break;
        } else if (relativeTimeMs <  elem.timeToExp) {
            std::map<int, TimerT> mNew;
            mNew[t.id] = t;
            m_stopMap[t.id] = m_timerList.insert(it, {relativeTimeMs, mNew});
            added = true;
            break;
        }
        relativeTimeMs -= elem.timeToExp;
    }
    if (!added) {
        std::map<int, TimerT> mNew;
        mNew[t.id] = t;
        m_timerList.emplace_back(TimerListElem(relativeTimeMs, mNew));
        m_stopMap[t.id] = std::prev(m_timerList.end());
    }
    return t.id;
}


bool TimerMgr::StopTimer(int id) {
    auto it = m_stopMap.find(id);
    if (it != m_stopMap.end()) {
        listIterT &lit = it->second;
        lit->m.erase(id);
        if (lit->empty()) {
            //remove this list elem
            m_timerList.erase(lit);
        }
        m_stopMap.erase(it);
    }

    return true;
}

void TimerMgr::Tick() {
    auto it = m_timerList.begin();
    if (it != m_timerList.end()) {
        TimerListElem &elem = *it;
        elem.timeToExp -= 1000;
        if (elem.timeToExp == 0) {
            //fire all timers
            std::map<int, TimerT> &m = it->m;
            for (auto itm = m.begin(); itm != m.end(); ++itm) {
                TimerT tMeta = itm->second;
                tMeta.cb(tMeta.id);
            }
            //remove this list elem
            m_timerList.pop_front();
        }
    }
}

bool TimerMgr::IsRunning(int id) {
    auto it = m_stopMap.find(id);
    if (it != m_stopMap.end()) {
        return true;
    }
    return false;
}
