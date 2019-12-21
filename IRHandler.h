#if !defined(_IR_HANDLER_H)
#define _IR_HANDLER_H
#include "ClockTime.h"
#include "IRremote.h"
#include "ClockAlarm.h"

#define IRPIN  9
IRrecv irrecv(IRPIN);
decode_results results;
extern IRrecv irrecv;
extern decode_results results;

void IRinit() {
    // irrecv standby
    irrecv.enableIRIn();
}

int set_check = 0, level = 0, current = 0, a_current = 0;
int Temp_time[2] = {0, 0}, answer[2] = {0, 0};

arduino_clock::Time* t = new arduino_clock::Time();
arduino_clock::Time* alarm_t = new arduino_clock::Time();
int alarm_h, alarm_m, alarm_s;
/* ============================================
 *    enum of IRsignal values
 *    2019.12.17 - by Jo
 */
enum IRSignal : long unsigned int {
    POWER = 0xFD00FF,
    VOLPLUS = 0xFD807F,
    FUNC = 0xFD40BF,
    LSHIFT = 0xFD20DF,
    PLAY = 0xFDA05F,
    RSHIFT = 0xFD609F,
    DOWN_ARROW = 0xFD10EF,
    VOLMINUS = 0xFD906F,
    UP_ARROW = 0xFD50AF,
    EQ = 0xFDB04F,
    ST = 0xFD708F,
    N0 = 0xFD30CF,
    N1 = 0xFD08F7,
    N2 = 0xFD8877,
    N3 = 0xFD48B7,
    N4 = 0xFD28D7,
    N5 = 0xFDA857,
    N6 = 0xFD6897,
    N7 = 0xFD18E7,
    N8 = 0xFD9867,
    N9 = 0xFD58A7,
    er = 0xFFFFFFFF
};

/* ==============================================
 *    decoding (decode_results -> char*)
 *    2019.12.17 - by Jo
 */

const char* decoding() {
    IRSignal IR;
    IR = (IRSignal)results.value;
    switch (IR) {
        case N1:
            return "1";
        case N2:
            return "2";
        case N3:
            return "3";
        case N4:
            return "4";
        case N5:
            return "5";
        case N6:
            return "6";
        case N7:
            return "7";
        case N8:
            return "8";
        case N9:
            return "9";
        case N0:
            return "0";
        case er:
            return "-1";
        case FUNC:
            return "fn";
    }
}

void IRProcess() {
    const char* sig_in = decoding();
        Serial.println(sig_in);
        Serial.println(set_check);
        Serial.println(alarm.isAlarmWarn());
        if (sig_in == "fn" && set_check == 0) {
            alarm.disable();
            t = new arduino_clock::Time();
            view.setMode(arduino_clock::ClockView::Mode::Setup, t);
            set_check = 1;
        }
        else if (sig_in == "fn" && set_check == 1) {
            view.setMode(arduino_clock::ClockView::Mode::Clock);
            alarm_t = new arduino_clock::Time();
            view.setMode(arduino_clock::ClockView::Mode::Setup, alarm_t);
            set_check = 2;
        }

        if (sig_in != "fn" && sig_in != "-1" && set_check != 0) {
            Temp_time[current] = atoi(sig_in);
            current++;
            if (current == 2) {
                int t_time = Temp_time[0] * 10 + Temp_time[1];
                switch (level) {
                    case 0:
                        if (set_check == 2) {
                            alarm_h = t_time;
                            alarm_t->hour = t_time;
                            alarm_t->changed = true;
                            view.setPosition(arduino_clock::ClockView::Position::Minute);
                        }
                        else {
                            t->hour = t_time;
                            t->changed = true;
                            view.setPosition(arduino_clock::ClockView::Position::Minute);
                        }
                        break;
                    case 1:
                        if (set_check == 2) {
                            alarm_m = t_time;
                            alarm_t->minute = t_time;
                            alarm_t->changed = true;
                            view.setPosition(arduino_clock::ClockView::Position::Second);
                        }
                        else {
                            t->minute = t_time;
                            t->changed = true;
                            view.setPosition(arduino_clock::ClockView::Position::Second);
                        }
                        break;
                    case 2:
                        if (set_check == 2) {
                            alarm_s = t_time;
                            alarm_t->second = t_time;
                            alarm_t->changed = true;
                            view.setPosition(arduino_clock::ClockView::Position::Hour);
                        }
                        else {
                            t->second = t_time;
                            t->changed = true;
                            view.setPosition(arduino_clock::ClockView::Position::Hour);
                            break;
                        }
                }
                
                level++;
                current = 0;
            }
            if (level == 3) {
                if (set_check == 1) {
                    view.setMode(arduino_clock::ClockView::Mode::Clock);
                }
                else {
                    view.setMode(arduino_clock::ClockView::Mode::Clock, false);
                }
                set_check--;
                level = 0;
                real_time = *t;
                if (set_check != 0) {
                    alarm.setAlarm(alarm_h, alarm_m, alarm_s);
                    alarm.enable();
                    t = new arduino_clock::Time();
                    view.setMode(arduino_clock::ClockView::Mode::Setup, t);
                }
            }
        }
         if (alarm.isAlarmWarn() && a_current == 0 && sig_in != "-1"){
            answer[a_current++] = atoi(sig_in);
            view.input(answer[0]);
        }else if(alarm.isAlarmWarn() && a_current == 1 && sig_in != "-1"){
            answer[a_current] = atoi(sig_in);
            a_current = 0;
            view.input(answer[0],answer[1]);
            int ans = answer[0] * 10 + answer[1];
            if(alarm.solve(ans)){
              alarm.disable();
              view.setMode(arduino_clock::ClockView::Mode::Clock);
     
            }else{
                view.reset();
            }
        }
        //알람 문제 답 입력코드 ver3
        
        irrecv.resume();
}

#endif //! _IR_HANDLER_H
