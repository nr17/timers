#include "../scheme1/orderedList.hpp"
#include <cassert>

int main() {
    TimerMgr mgr;
    int fired = 0;
    int id = mgr.StartTimer(2000, [&](int timerId){ ++fired; });

    mgr.Tick(); // 1 second passed
    assert(fired == 0);
    assert(mgr.IsRunning(id));

    mgr.Tick(); // timer should fire
    assert(fired == 1);
    assert(!mgr.IsRunning(id));

    fired = 0;
    int id1 = mgr.StartTimer(1000, [&](int){ ++fired; });
    int id2 = mgr.StartTimer(1000, [&](int){ ++fired; });

    mgr.Tick();
    assert(fired == 2);
    assert(!mgr.IsRunning(id1));
    assert(!mgr.IsRunning(id2));

    return 0;
}
