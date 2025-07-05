#include "orderedList.hpp"
#include <cassert>

int main() {
    TimerMgr mgr;
    int fired = 0;
    int id = mgr.StartTimer(2000, [&](int){ ++fired; });

    mgr.Tick(); // 1 second elapsed
    assert(mgr.IsRunning(id));
    assert(fired == 0);

    mgr.Tick(); // timer should expire
    assert(!mgr.IsRunning(id));
    assert(fired == 1);

    return 0;
}
