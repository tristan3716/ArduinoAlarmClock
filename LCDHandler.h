/* ===========================================
 *    LCD Handler
 *    2019.12.13 v1 - by Hu
 *        make sekeleton
 *        make print time
 *    2019.12.14 v2 - by Hu
 *        make render()
 *        conceal almost functions except render(), init(), clear(), setMode()
 */
#if !defined(_LCD_HANDLER_H)
#define _LCD_HANDLER_H

#include "I2CSearch.h"
#include "trace.h"

#include <LiquidCrystal_I2C.h>
namespace arduino_clock {
    namespace lcd_handle {
        static LiquidCrystal_I2C _lcd(0x20, 16, 2);
        class ClockView {
        public:
            enum class Mode {
                Alarm,
                Setup,
                Clock
            };
            void setMode(Mode mode) {
                _displayMode = mode;
                #if defined (_DEBUG)
                switch(mode) {
                    case Mode::Alarm:
                        traceln("set display to alarm mode");
                        break;
                    case Mode::Setup:
                        traceln("set display to setup mode");
                        break;
                    case Mode::Clock:
                        traceln("set display to clock mode");
                        break;
                    default:
                        cerror("illegal mode");
                }
                #endif
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

                setMode(Mode::Clock);
                time = &_t;
            }
            void render() {
                if (is_delay()) {
                    trace("render ");
                    switch (_displayMode) {
                        case Mode::Alarm:
                            traceln("(Alarm mode)");
                            break;
                        case Mode::Clock:
                            traceln("(clock mode)");
                            print_time();
                            break;
                        case Mode::Setup:
                            traceln("(Setup mode)");
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
            void setCursor(int x, int y) {
                lcd->setCursor(x, y);
            }
            void print(const char *msg) {
                lcd->print(msg);
            }
            void print_time() {
                if (time->is_change() == true) {
                    lcd->clear();
                    time->disable_change_flag();
                    lcd->setCursor(0, 0);
                    lcd->print(time->format());
                    traceln("render time : ", time->format());
                } else {
                    lcd->setCursor(6, 0);
                    lcd->print(time->format_second());
                    traceln("render only second : ", time->format_second());
                }
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
            Time *time;

            volatile uint32_t _old_time = 0;
            const uint32_t _display_delay = 333;
            Mode _displayMode;
        };
    };  // namespace lcd_handle
};      // namespace arduino_clock

#endif //! _LCD_HANDLER_H
