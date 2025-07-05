#include "../scheme1/orderedList.hpp"
#include <cassert>

int main() {
    // single timer expiration after two ticks
    {
        TimerMgr mgr;
        int fired = 0;
        int id = mgr.StartTimer(2000, [&](int){ ++fired; });

        mgr.Tick(); // 1 second passed
        assert(fired == 0);
        assert(mgr.IsRunning(id));

        mgr.Tick(); // timer should fire
        assert(fired == 1);
        assert(!mgr.IsRunning(id));
    }

    // two timers with the same interval
    {
        TimerMgr mgr;
        int fired = 0;
        int id1 = mgr.StartTimer(1000, [&](int){ ++fired; });
        int id2 = mgr.StartTimer(1000, [&](int){ ++fired; });

        mgr.Tick();
        assert(fired == 2);
        assert(!mgr.IsRunning(id1));
        assert(!mgr.IsRunning(id2));
    }

    // timer stopped before it ever fires
    {
        TimerMgr mgr;
        int fired = 0;
        int id = mgr.StartTimer(2000, [&](int){ ++fired; });
        assert(mgr.IsRunning(id));
        mgr.StopTimer(id);
        assert(!mgr.IsRunning(id));
        mgr.Tick();
        mgr.Tick();
        assert(fired == 0);
    }

    // timers with different intervals fire in order
    {
        TimerMgr mgr;
        int fired = 0;
        int idA = mgr.StartTimer(3000, [&](int){ fired += 1; });
        int idB = mgr.StartTimer(1000, [&](int){ fired += 10; });

        mgr.Tick(); // idB should fire
        assert(fired == 10);
        assert(!mgr.IsRunning(idB));
        assert(mgr.IsRunning(idA));

        mgr.Tick();
        assert(fired == 10); // still waiting for idA

        mgr.Tick();
        assert(fired == 10); // still waiting for idA

        mgr.Tick(); // idA should fire now
        assert(fired == 11);
        assert(!mgr.IsRunning(idA));
    }

    // insert timer after some ticks have already passed
    {
        TimerMgr mgr;
        int fired = 0;
        int idLong = mgr.StartTimer(3000, [&](int){ fired += 1; });
        mgr.Tick(); // 1 second elapsed, 2 seconds remaining on idLong

        int idShort = mgr.StartTimer(1000, [&](int){ fired += 10; });
        mgr.Tick(); // idShort should fire
        assert(fired == 10);
        assert(!mgr.IsRunning(idShort));
        assert(mgr.IsRunning(idLong));

        mgr.Tick();
        assert(fired == 10); // idLong still waiting

        mgr.Tick(); // idLong should fire
        assert(fired == 11);
        assert(!mgr.IsRunning(idLong));
    }

    return 0;
}
