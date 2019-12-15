/* ===========================================
 *    LCD Handler
 *    2019.12.13 v1 - by Hu
 *        make sekeleton
 *        make print print_time
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
        enum class Mode {
            Alarm,
            Setup,
            Clock
        };
        void setMode(Mode mode) {
            if (mode == Mode::Setup) {
                if (IsDebug()) {
                    cerror("print_time parameters must be passed to switch to setup mode");
                }
                else {
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
                        setMode(Mode::Clock);
                        return;
                    }
            }
            if (mode != _displayMode) {
                _frame_rendered = false;
            }
            _displayMode = mode;
        }
        void setMode(Mode mode, Time *_t) {
            _printable_time = _t;
            if (mode != Mode::Setup) {
                if (IsDebug()) {
                    cerror("only setup mode with print_time");
                }
                else {
                    return;
                }
            }
            if (mode != _displayMode) {
                _frame_rendered = false;
                _displayMode = mode;
            }
            else {
                traceln("already in setup mode. the request is ignored.");
                return;
            }
            traceln("set display to setup mode");
        }
        enum class Position {
            Hour   = 0b001,
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
            _time = &_t;

            blink = false;
            current_change_position = Position::Hour;
        }
        void render() {
            if (is_delay()) {
                switch (_displayMode) {
                    case Mode::Alarm:
                        traceln("render (Alarm mode)");
                        if (_frame_rendered == false) {
                            //_renderFrame();
                            _frame_rendered = true;
                        }
                        break;
                    case Mode::Clock:
                        if (_frame_rendered == false) {
                            _renderFrame();
                            _frame_rendered = true;
                        }
                        print_time();
                        printDHT();
                        break;
                    case Mode::Setup:
                        traceln("render (Setup mode)");
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
        void __render_frame_time() const __attribute__((always_inline)) {
            __print__(2, 0, ':');
            __print__(5, 0, ':');
        }
        void __render_frame_temperature() const __attribute__((always_inline)) {
            __print__(9, 0, _face_icon);
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
        void print_time() {
            if (_printable_time->is_change() == true) {
                _printable_time->disable_change_flag();
                __print__(0, 0, _printable_time->format());
            }
            else {
                __print__(6, 0, _printable_time->format_second());
            }
        }

        const char *_blink(const Position &p, const char *fmt) {
            traceln("blink is called");
            if (current_change_position == p) {
                static byte count = 0;
                ++count;
                if (count > 2) {
                    blink = !blink;
                    count = 0;
                }
                if (blink == true) {
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
                __print__(0, 0, _blink(Position::Hour, _printable_time->format_hour()));
                __print__(3, 0, _blink(Position::Minute, _printable_time->format_minute()));
                __print__(6, 0, _blink(Position::Second, _printable_time->format_second()));
                _printable_time->disable_change_flag();
            }
            else {
                switch(current_change_position) {
                    case Position::Hour:
                        __print__(0, 0, _blink(Position::Hour, _printable_time->format_hour()));
                        break;
                    case Position::Minute:
                        __print__(3, 0, _blink(Position::Minute, _printable_time->format_minute()));
                        break;
                    case Position::Second:
                        __print__(6, 0, _blink(Position::Second, _printable_time->format_second()));
                        break;
                    default:
                        break;
                }
            }
        }
        void printDHT() {
            float temp = getTemperature();
            float diff = SmileFaceMid - temp;
            diff = diff < 0 ? -diff : diff;
            //traceln("Diff : ", diff);
            if (diff > AngryFaceDiff) {  // ~20, 28~
                if (_face_icon != Icon::AngryFace) {
                    _face_icon = Icon::AngryFace;
                    __print__(9, 0, Icon::AngryFace);
                }
            }
            else if (diff < SoSoFaceDiff) {  // 22~26
                if (_face_icon != Icon::SmileFace) {
                    _face_icon = Icon::SmileFace;
                    __print__(9, 0, Icon::SmileFace);
                }
            }
            else if (_face_icon != Icon::SoSoFace) {  // 20~22, 26~28
                _face_icon = Icon::SoSoFace;
                __print__(9, 0, Icon::SoSoFace);
            }
            __print__(10, 0, temp, 1);

            __print__(10, 1, getHumidity(), 1);
        }
        inline bool is_delay() {
            if (_old_time + _display_delay < millis()) {
                _old_time = _old_time + _display_delay;
                return true;
            }
            return false;
        }

    private:
        LiquidCrystal_I2C *lcd;
        Time *_printable_time;
        Time *_time;
        Position current_change_position;
        bool blink;

        uint32_t _old_time = 0;
        const uint32_t _display_delay = 333;
        Mode _displayMode;
        byte _face_icon;
        bool _frame_rendered;
    };
};  // namespace arduino_clock

#endif  //! _LCD_HANDLER_H
