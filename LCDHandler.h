/* ===========================================
 *    LCD Handler
 *    2019.12.13 v1 - by Hu
 *        make sekeleton
 *        make print print_clock_time
 *    2019.12.14 v2 - by Hu
 *        make render()
 *        conceal almost functions except render(), init(), clear(), setMode()
 */
#if !defined(_LCD_HANDLER_H)
#define _LCD_HANDLER_H

#include "CustomCharacter.h"
#include "I2CSearch.h"
#include "trace.h"

#include "Temperature.h"

#include <LiquidCrystal_I2C.h>

#define AngryFaceDiff 4
#define SoSoFaceDiff 2
#define SmileFaceMid 24

namespace arduino_clock {
    class ClockView {
    public:
        /* ========================================
        * functions related to mode
        */
        enum class Mode {
            Alarm,
            Setup,
            Clock
        };
        void setMode(Mode mode) {
            if (mode == Mode::Setup) {
                if (IsDebug()) {
                    cerror("time parameters must be passed to switch to setup mode");
                }
                else {
                    release.log("time parameters must be passed to switch to setup mode");
                    release.log("the request is ignored");
                    return;
                }
            }
            switch (mode) {
                case Mode::Alarm:
                    traceln("set display to alarm mode");
                    break;
                case Mode::Clock:
                    traceln("set display to clock mode");
                    break;
                default:
                    if (IsDebug()) {
                        cerror("illegal mode");
                    }
                    else {
                        release.log("illegal mode");
                        release.log("the request is ignored");
                        setMode(Mode::Clock);
                        return;
                    }
            }
            if (mode != _displayMode) {
                _frame_rendered = false;
                if (_displayMode == Mode::Setup) {
                    traceln("Update time");
                    *_clock_time = *_printable_time;
                }
            }
            _printable_time->changed = true;
            _displayMode = mode;
        }
        void setMode(Mode mode, Time *_t) {
            if (mode != Mode::Setup) {
                if (IsDebug()) {
                    cerror("only setup mode with time");
                }
                else {
                    release.log("only setup mode with time");
                    release.log("the request is ignored");
                    return;
                }
            }
            if (mode != _displayMode) {
                _frame_rendered = false;
                _displayMode = mode;
            }
            else {
                release.log("already in setup mode");
                release.log("the request is ignored");
                return;
            }
            _printable_time = _t;
            traceln("set display to setup mode");
        }
        enum class Position {
            Hour = 0b001,
            Minute = 0b010,
            Second = 0b100
        };
        void setPosition(const Position &p) {
#if defined(_DEBUG)
            traceln("Position changed to ");
            switch (p) {
                case Position::Hour:
                    traceln("Hour");
                    break;
                case Position::Minute:
                    traceln("Minute");
                    break;
                case Position::Second:
                    traceln("Second");
                    break;
                default:
                    break;
            }
#endif
            current_change_position = p;
        }

    public:
        /* ========================================
        * externally released functions except that related to set mode
        *   init(Time)
        *   render()
        */
        void init(Time &_t) {
            traceBegin();
            traceln("initialize LCD");
            AddressScanner as;
            static LiquidCrystal_I2C _lcd(as.search(), 16, 2);
            lcd = &_lcd;
            lcd->init();
            lcd->backlight();

            lcd->createChar(Icon::WaterDrop, water_drop);
            lcd->createChar(Icon::AngryFace, face_angry);
            lcd->createChar(Icon::SoSoFace, face_soso);
            lcd->createChar(Icon::SmileFace, face_smile);

            _frame_rendered = false;
            setMode(Mode::Clock);
            _printable_time = &_t;
            _clock_time = &_t;

            _old_time = 0;
            _blink = false;
            current_change_position = Position::Hour;
        }
        void render() {
            if (is_delay()) {
                switch (_displayMode) {
                    case Mode::Alarm:
                        clear();
                        //traceln("render (Alarm mode)");
                        if (_frame_rendered == false) {
                            //_renderFrame();
                            _frame_rendered = true;
                        }
                        __print__(0, 0, "Alarm..."); 
                        break;
                    case Mode::Clock:
                        if (_frame_rendered == false) {
                            _renderFrame();
                            _frame_rendered = true;
                        }
                        print_clock_time();
                        printDHT();
                        break;
                    case Mode::Setup:
                        if (_frame_rendered == false) {
                            _renderFrame();
                            _frame_rendered = true;
                        }
                        print_setup_time();
                        printDHT();
                        break;
                    default:
                        break;
                }
            }
        }
        
        void clear() {
            traceln("clear lcd");
            lcd->clear();
        }

    private:
        /* ========================================
        * functions that communicate with LCD
        */
        void __print__(uint8_t x, uint8_t y, const char *msg) const __attribute__((always_inline)) {
            lcd->setCursor(x, y);
            lcd->print(msg);
        }
        void __print__(uint8_t x, uint8_t y, uint8_t value) const __attribute__((always_inline)) {
            lcd->setCursor(x, y);
            lcd->write(value);
        }
        void __print__(uint8_t x, uint8_t y, double value, int p) const __attribute__((always_inline)) {
            lcd->setCursor(x, y);
            lcd->print(value, p);
        }
        // __declspec(deprecated)
        // void __print__(const char *msg) const __attribute__((always_inline)) {
        //     lcd->print(msg);
        // }
        // __declspec(deprecated)
        // void __print__(uint8_t value) const __attribute__((always_inline)) {
        //     lcd->write(value);
        // }
        // __declspec(deprecated)
        // void __setCursor__(int x, int y) const __attribute__((always_inline)) {
        //     lcd->setCursor(x, y);
        // }

        /* ========================================
        * functions that render frame
        */
        void __render_frame_time() const __attribute__((always_inline)) {
            __print__(2, 0, ':');
            __print__(5, 0, ':');
        }
        void __render_frame_temperature() const __attribute__((always_inline)) {
            __print__(9, 0, _current_face_icon);
            __print__(14, 0,
                      "\xDF"
                      "C");
        }
        void __render_frame_humidity() const __attribute__((always_inline)) {
            __print__(9, 1, Icon::WaterDrop);
            __print__(15, 1, "%");
        }
        void _renderFrame() const {
            traceln("render frames");
            __render_frame_time();
            __render_frame_temperature();
            __render_frame_humidity();
        }

        /* ========================================
        * functions that render time
        */
        void print_clock_time() {
            if (_printable_time->is_change() == true) {
                _printable_time->disable_change_flag();
                __print__(0, 0, _printable_time->format());
            }
            else {
                __print__(6, 0, _printable_time->format_second());
            }
        }

        const char *__blink(const Position &p, const char *fmt) {
            traceln("_blink is called");
            if (current_change_position == p) {
                static byte count = 0;
                ++count;
                if (count > 2) {
                    _blink = !_blink;
                    count = 0;
                }
                if (_blink == true) {
                    return "__";
                }
                else {
                    return fmt;
                }
            }
            else {
                return fmt;
            }
        }
        void print_setup_time() {
            if (_printable_time->is_change() == true) {
                __print__(0, 0, __blink(Position::Hour, _printable_time->format_hour()));
                __print__(3, 0, __blink(Position::Minute, _printable_time->format_minute()));
                __print__(6, 0, __blink(Position::Second, _printable_time->format_second()));
                _printable_time->disable_change_flag();
            }
            else {
                switch (current_change_position) {
                    case Position::Hour:
                        __print__(0, 0, __blink(Position::Hour, _printable_time->format_hour()));
                        break;
                    case Position::Minute:
                        __print__(3, 0, __blink(Position::Minute, _printable_time->format_minute()));
                        break;
                    case Position::Second:
                        __print__(6, 0, __blink(Position::Second, _printable_time->format_second()));
                        break;
                    default:
                        break;
                }
            }
        }

        /* ========================================
        * functions that render temperature & humidity
        */
        void printDHT() {
            float temp = getTemperature();
            float diff = SmileFaceMid - temp;
            diff = diff < 0 ? -diff : diff;
            //traceln("Diff : ", diff);
            if (diff > AngryFaceDiff) {  // ~20, 28~
                if (_current_face_icon != Icon::AngryFace) {
                    _current_face_icon = Icon::AngryFace;
                    __print__(9, 0, Icon::AngryFace);
                }
            }
            else if (diff < SoSoFaceDiff) {  // 22~26
                if (_current_face_icon != Icon::SmileFace) {
                    _current_face_icon = Icon::SmileFace;
                    __print__(9, 0, Icon::SmileFace);
                }
            }
            else if (_current_face_icon != Icon::SoSoFace) {  // 20~22, 26~28
                _current_face_icon = Icon::SoSoFace;
                __print__(9, 0, Icon::SoSoFace);
            }
            __print__(10, 0, temp, 1);

            __print__(10, 1, getHumidity(), 1);
        }

        /* ========================================
        * functions that check delay
        */
        inline bool is_delay() {
            if (_old_time + _display_delay < millis()) {
                _old_time = _old_time + _display_delay;
                return true;
            }
            return false;
        }

    private:
        LiquidCrystal_I2C *lcd;
        Time *_printable_time;  // replaceable in setup
        Time *_clock_time; // set when initialization

        Mode _displayMode;
        uint32_t _old_time;
        const uint32_t _display_delay = 333;

        Position current_change_position;
        bool _blink;
        byte _current_face_icon;
        bool _frame_rendered;
    };
};  // namespace arduino_clock

#endif  //! _LCD_HANDLER_H
