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
            _format(hour, 0); //01
            _buffer[2] = ':';
            _format(minute, 3); // 34
            _buffer[5] = ':';
            _format(second, 6); // 67
            _buffer[8] = 0;
            return _buffer;
        }
        const char *format_second() {
            _format(second, 6);
            _buffer[8] = 0;
            return &_buffer[6];
        }
        
    private:
        void _format(int x, int p) __attribute__((always_inline)) {
            _buffer[p] = x / 10 + '0';
            _buffer[p+1] = x % 10 + '0'; 
        }
        void operator++() __attribute__((always_inline)) {
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
};

#endif //! _CLOCK_TIME_H
