/* ===========================================
      Alarm class
      2019.12.17 v1 - by JH
      2019.12.19 v2 - by JH
*/
#if !defined(_CLOCK_ALARM_H)
#define _CLOCK_ALARM_H

#include "ClockTime.h"
#include "trace.h"
#include "LCDHandler.h"

#define C4 262  // do4
#define D4 294  // re4
#define E4 330  // mi4
#define F4 349  // pa4
#define G4 392  // sol4
#define A4 440  // la4
#define B4 494  // shi4
#define C5 523  // do5
#define D5 587  // re5
#define E5 659  // mi5
#define F5 698  // pa5
#define G5 784  // sol5
#define A5 880  // la5
#define B5 988  // shi5

namespace arduino_clock {
    namespace reveille {
        constexpr int melody[] = {
            G5, E5, E5, E5, E5, E5, G5, E5, C5, E5, G5, E5, C5, E5, G5, E5, E5, C5, C5, C5, C5, E5, E5,
            E5, E5, G5, E5, C5, E5, G5, E5, C5, E5, G5, E5, E5, C5, C5, C5, C5, C5, G5, G5, E5, C5,
            E5, C5, E5, C5, E5, G5, G5, C5, C5, E5, E5, G5, G5, E5, C5, E5, C5, E5, C5, E5, G5, G5};
        constexpr int noteDurations[] = {
            4, 4, 8, 8, 4, 8, 8, 8, 8, 8, 16, 16, 8, 8, 8, 8, 4, 8, 16, 16, 4, 8, 8, 4,
            8, 8, 8, 8, 8, 16, 16, 8, 8, 8, 8, 4, 8, 16, 16, 4, 2, 8, 16, 16, 8, 8,
            16, 16, 16, 16, 4, 2, 8, 8, 8, 8, 8, 16, 16, 8, 8, 16, 16, 16, 16, 4, 2};
    };  // namespace reveille

    class Alarm {
    private:
        /* ========================================
         * functions related to alaraming
         */

    public:
        void init(Time &t) {
            _old_time = 0;
            _enable = false;
            _warn = false;
            _real_time = NULL;
            thisNote = 0;
            duration = 16 / reveille::noteDurations[thisNote];
            _real_time = &t;
        }
        void enable() __attribute__((always_inline)) {
            _enable = true;
        }
        void disable() __attribute__((always_inline)) {
            _enable = false;
            view.setMode(ClockView::Mode::Clock);
        }
        void setAlarm(const int alarm_h, const int alarm_m, const int alarm_s) __attribute__((always_inline)) {
            release.log("alarm set to ", alarm_h, ":", alarm_m, ":", alarm_s);
            _seted_alarm_time.hour = alarm_h;
            _seted_alarm_time.minute = alarm_m;
            _seted_alarm_time.second = alarm_s;
        }

        bool isAlarmWarn() const __attribute__((always_inline)) {
            return _enable == true && (_seted_alarm_time == *_real_time || _warn == true);
        }
        void warn() {
            if (_warn == false) {
                generate();
                release.log(F("Alarm is warn ..."));
                _old_time = millis();
                view.setMode(ClockView::Mode::Alarm);
                _warn = true;
                tone(12, reveille::melody[thisNote], tempo * reveille::noteDurations[thisNote]);
            }
            if (is_delay()) {
                duration--;
                if (duration == 0) {
                    thisNote++;
                    if (thisNote == 67) {
                        thisNote = 0;
                    }
                    duration = 16 / reveille::noteDurations[thisNote];
                    noTone(12);
                    tone(12, reveille::melody[thisNote], tempo * duration);
                }
            }
        }

    private:
        inline bool is_delay() __attribute__((always_inline)) {
            if (_old_time + tempo < millis()) {
                _old_time = _old_time + tempo;
                return true;
            }
            return false;
        }
    public:
        void _format(int x, int p) __attribute__((always_inline)) {
            if (x < 10) {
                _buffer[p] = ' ';
            }
            else {
                _buffer[p] = x / 10 + '0';
            }
            _buffer[p + 1] = x % 10 + '0';
        }
        void generate() {
            traceln("generate problme");
            int a = random(1, 99);
            int b = random(max(1,10-a), 100-a);
            traceln(a, "+", b, "=", a+b);
            _format(a, 0);
            _buffer[2] = ' ';
            _buffer[3] = '+';
            _buffer[4] = ' ';
            _format(b, 5);
            _buffer[7] = ' ';
            _buffer[8] = '=';
            _buffer[9] = 0;
            answer = a+b;
            view.set_problem(_buffer);
        }
        bool solve(int x) const {
            traceln("answer is ", answer);
            traceln("res is ", x);
            return x == answer;
        }
    private:
        char _buffer[10];
    private:
        int answer;
        Time _seted_alarm_time;
        Time *_real_time;
        bool _enable;
        bool _warn;
        int thisNote;
        int duration;
        uint32_t _old_time;
        const uint32_t tempo = 1950 / 16;

        byte current_;
    };
};  // namespace arduino_clock

#endif  //! _CLOCK_ALARM_H
