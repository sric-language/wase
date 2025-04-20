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
public:

    void setTimeout(int64_t timeMillis, std::function<void()> callback);

    void clearSchedule();

    void fireTimeEvents();
};

float getScreenScale();
bool getAutoScale();

void setTimeout(int64_t timeMillis, std::function<void()> callback);

void fireTimeEvents();


struct Window
{
    virtual sric::RefPtr<View> view() = 0;

    virtual void repaint(Rect& dirty) = 0;

    virtual Size size() = 0;

    virtual bool hasFocus() = 0;

    virtual void focus() = 0;

    virtual sric::OwnPtr<TextInput> textInput(int inputType) = 0;

    virtual void fileDialog(bool isOpen, const char* accept) = 0;

    virtual void displayKeyboard(bool display) = 0;

    void paint(Graphics& g);

    static int open(sric::OwnPtr<waseGraphics::View> view, const char* name);
    static sric::RefPtr<Window> getCur();
};

}

#endif