/* ===========================================
 *    Time class
 *    2019.12.13 v1 - by Hu
 *    2019.12.14 v2 - by Hu
 *      add is_change()
 *      rename minute_changed to changed
 */
#if !defined(_CLOCK_TIME_H)
#define _CLOCK_TIME_H

#include <TimerOne.h>

namespace arduino_clock {
    class Time {
    public:
        bool changed;

        uint8_t hour;
        uint8_t minute;
        uint8_t second;

    private:
        char _buffer[9];

    public:
        Time() {
            init();
        }
        void init() {
            changed = true;
            hour = 0;
            minute = 0;
            second = 0;
        };
        void disable_change_flag() __attribute__((always_inline)) {
            changed = false;
        }
        bool is_change() const {
            return changed;
        }
        void increase() __attribute__((always_inline)) {
            this->operator++();
        }
        const char *format() {
            _format(hour, 0);  //01
            _buffer[2] = ':';
            _format(minute, 3);  // 34
            _buffer[5] = ':';
            _format(second, 6);  // 67
            _buffer[8] = 0;
            return _buffer;
        }
        const char *format_hour() {
            _format(hour, 0);
            _buffer[2] = 0;
            return &_buffer[0];
        }
        const char *format_minute() {
            _format(minute, 3);
            _buffer[5] = 0;
            return &_buffer[3];
        }
        const char *format_second() {
            _format(second, 6);
            _buffer[8] = 0;
            return &_buffer[6];
        }
        Time &operator=(const Time &rhs) {
            if (this != &rhs) {
                changed = rhs.changed;
                hour = rhs.hour;
                minute = rhs.minute;
                second = rhs.second;
            }

            return *this;
        }

        bool operator==(const Time &rhs) const {
            return hour == rhs.hour && minute == rhs.minute && second == rhs.second;
        }

    private:
        void _format(int x, int p) __attribute__((always_inline)) {
            _buffer[p] = x / 10 + '0';
            _buffer[p + 1] = x % 10 + '0';
        }
        void operator++() {
            ++second;
            if (second == 60) {
                changed = true;
                second = 0;
                ++minute;
                if (minute == 60) {
                    minute = 0;
                    ++hour;
                    if (hour == 24) {
                        hour = 0;
                    }
                }
            }
        }
    };
};  // namespace arduino_clock

#define SECOND 1000
namespace {
    uint32_t _old_time = millis();
    
    inline bool is_delay() {
        if (_old_time + SECOND < millis()) {
            _old_time = _old_time + SECOND;
            return true;
        }
        return false;
    }
}
arduino_clock::Time real_time;


void time_pass() {
    if (is_delay()) {
        real_time.increase();
    }
}

extern arduino_clock::Time real_time;

#endif  //! _CLOCK_TIME_H
