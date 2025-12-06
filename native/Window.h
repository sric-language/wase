#ifndef WINDOW_H_
#define WINDOW_H_

#include "waseGraphics.h"
#include <vector>
#include <queue>
#include <mutex>

namespace waseGraphics {

class EventTimer {
    struct TimeEvent
    {
        TimeEvent(int64_t time, std::function<void()> callback) : time(time), callback(callback) {
        }
        bool operator<(const TimeEvent& v) const {
            return time > v.time;
        }
        int64_t time;
        std::function<void()> callback;
        void* cookie = nullptr;
    };

    std::priority_queue<TimeEvent, std::vector<TimeEvent>, std::less<TimeEvent> > _timeEvents;     // Contains the scheduled time events.
    std::mutex _scheduleLock;

    static EventTimer* _cur;
public:
    static EventTimer* getCur();
    std::function<void()> onFrame;

    void setTimeout(int64_t timeMillis, std::function<void()> callback);

    void clearSchedule();

    void fireTimeEvents();
};

float getScreenScale() SC_NOTHROW;
bool getAutoScale() SC_NOTHROW;

void setTimeout(int64_t timeMillis, std::function<void()> callback) SC_NOTHROW;

void fireTimeEvents() SC_NOTHROW;


}

#endif