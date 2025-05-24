#include "Window.h"
#include "sric.h"
using namespace waseGraphics;


void EventTimer::setTimeout(int64_t timeMillis, std::function<void()> callback) {
    int64_t time = sric::millisTicks() + timeMillis;
    TimeEvent timeEvent(time, callback);
    _scheduleLock.lock();
    _timeEvents.push(timeEvent);
    _scheduleLock.unlock();
}

void EventTimer::clearSchedule() {
    _scheduleLock.lock();
    while (_timeEvents.size() > 0)
    {
        _timeEvents.pop();
    }
    _scheduleLock.unlock();
}

void EventTimer::fireTimeEvents() {
    int64_t frameTime = sric::millisTicks();
    std::vector<TimeEvent> toFire;
    _scheduleLock.lock();
    while (_timeEvents.size() > 0)
    {
        const TimeEvent timeEvent = _timeEvents.top();
        if (timeEvent.time > frameTime)
        {
            break;
        }
        //if (timeEvent.listener)
        //{
        toFire.push_back(timeEvent);
        //}
        _timeEvents.pop();
    }
    _scheduleLock.unlock();

    for (auto timeEvent : toFire) {
        if (timeEvent.callback) {
            timeEvent.callback();
        }
    }
}

float g_screenScle = 1;
bool g_autoScale = true;
EventTimer _eventTimer;

void waseGraphics::setTimeout(int64_t timeMillis, std::function<void()> callback) SC_NOTHROW {
    _eventTimer.setTimeout(timeMillis, callback);
}

float waseGraphics::getScreenScale() SC_NOTHROW {
    return g_screenScle;
}

bool waseGraphics::getAutoScale() SC_NOTHROW {
    return g_autoScale;
}

void waseGraphics::fireTimeEvents() SC_NOTHROW {
    _eventTimer.fireTimeEvents();
}

void waseGraphics::Window::paint(Graphics& g) SC_NOTHROW {
    if (view() != nullptr) {
        g.save();
        view()->onPaint(g);
        g.restore();
    }
}