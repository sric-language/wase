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

float getScreenScale() SC_NOTHROW;
bool getAutoScale() SC_NOTHROW;

void setTimeout(int64_t timeMillis, std::function<void()> callback) SC_NOTHROW;

void fireTimeEvents() SC_NOTHROW;


struct Window
{
    virtual sric::RefPtr<View> view() SC_NOTHROW = 0;

    virtual void repaint(Rect& dirty) SC_NOTHROW = 0;

    virtual Size size() SC_NOTHROW = 0;

    virtual bool hasFocus() SC_NOTHROW = 0;

    virtual void focus() SC_NOTHROW = 0;

    virtual sric::OwnPtr<TextInput> textInput(int inputType) SC_NOTHROW = 0;

    virtual void fileDialog(bool isOpen, const char* accept) SC_NOTHROW = 0;

    virtual void displayKeyboard(bool display) SC_NOTHROW = 0;

    void paint(Graphics& g) SC_NOTHROW;

    static int open(sric::OwnPtr<waseGraphics::View> view, const char* name) SC_NOTHROW;
    static sric::RefPtr<Window> getCur() SC_NOTHROW;
};

}

#endif