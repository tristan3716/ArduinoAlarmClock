#define _DEBUG

/* //! IMPORTANT
 *  사용한 핀을 기술할 것.
 *      아날로그 핀 : A0, A1, A2, A3, A4, A5
 *      디지털 핀 : P2, P3*, P4, P5*, P6*, P7, P8, P9, P10, P11*, P12*, P13  (* is PWM)
 *      
 *  == Used Pin ==
 *  A4, A5 - for I2C LCD module
 *  P3*, P11* - for tone()
 *  P5*, P6* - for timer for time_increase
 *  P9 - for IR sensor
 *  p12 - for Alarm Sound
 */

/* ===========================================
 *    Time class
 *    2019.12.14 v2 - by Hu
 */
#include "ClockTime.h"

/* ===========================================
 *    LCD Handler
 *    2019.12.14 v2 - by Hu
 */
#include "LCDHandler.h"

/* ===========================================
 *    Timer Library - TimerOne
 */
#include <TimerOne.h>

/* ===========================================
 *    IR Library - IRremote
 */ 

#include <IRremote.h>
int IRpin = 9;
IRrecv irrecv(IRpin);
decode_results results;

/* ===========================================
 *    global variable & method
 *    2019.12.13 v1 - by Hu
 */

#include "ClockAlarm.h"
arduino_clock::Alarm alarm;
/*
*
*   Alram 
*   2019.12.18 v1 - by Jh
*   2019.12.19 v2 - by Jh
*/ 

arduino_clock::Time time;
arduino_clock::ClockView view;

void time_pass(){
    time.increase();
}
int set_check = 0, level = 0, current = 0;              
int Temp_time[2] = {0,0};
arduino_clock::Time* t = new arduino_clock::Time();
arduino_clock::Time* alarm_t = new arduino_clock::Time();
int alarm_h, alarm_m, alarm_s;
/* ============================================
 *    enum of IRsignal values
 *    2019.12.17 - by Jo
 */
enum IRSignal {
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

char* decoding() {
  enum IRSignal IR;
  IR = results.value;
  switch(IR){
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

/* ===========================================
 *    Arduino main
 */
void setup() {
    // initialize Serial
    Serial.begin(9600);
    Serial.println("Serial Start");
    pinMode(12, OUTPUT);
    // initialize time
    time.init();
    // initialize lcd handler
    view.init(time);
    
    // initialize Timer
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(time_pass);
    
    
    // irrecv standby
    irrecv.enableIRIn();

    // alarm
    alarm.init(time);
    arduino_clock::Time t;
}

void loop() {
    /* Update source write here */
    //! TODO:
    /* turn to setup mode*/
    if(irrecv.decode(&results)){
        char* sig_in = decoding();
        Serial.println(sig_in);
        Serial.println(set_check);
        if(sig_in == "fn" && set_check == 0){
            t = new arduino_clock::Time();
            view.setMode(arduino_clock::ClockView::Mode::Setup, t);
            set_check = 1;
            
        }else if(sig_in == "fn" && set_check == 1){
          view.setMode(arduino_clock::ClockView::Mode::Clock);
          alarm_t = new arduino_clock::Time();
          view.setMode(arduino_clock::ClockView::Mode::Setup, alarm_t);
          set_check = 2;
        }

        
        if(sig_in != "fn" && sig_in != "-1" && set_check !=0){
        
            Temp_time[current] = atoi(sig_in);
            ++current;
            if(current == 2){
                int t_time = Temp_time[0] * 10 + Temp_time[1];
                switch(level){
                    case 0:
                        if(set_check == 2){
                          alarm_h = t_time;
                          alarm_t -> hour = t_time;
                          alarm_t -> changed = true;
                          view.setPosition(arduino_clock::ClockView::Position::Minute);
                        }else{
                        t->hour = t_time;
                        t->changed = true;
                        view.setPosition(arduino_clock::ClockView::Position::Minute);
                        }
                        break;
                    case 1:
                        if(set_check == 2){
                          alarm_m = t_time;
                          alarm_t -> minute = t_time;
                          alarm_t -> changed = true;
                          view.setPosition(arduino_clock::ClockView::Position::Second);
                        }else{
                        t->minute = t_time;
                        t->changed = true;
                        view.setPosition(arduino_clock::ClockView::Position::Second);
                        }
                        break;
                    case 2:
                        if(set_check == 2){
                          alarm_s = t_time;
                          alarm_t -> second = t_time;
                          alarm_t -> changed = true;
                          view.setPosition(arduino_clock::ClockView::Position::Hour);
                        }else{
                        t->second = t_time;
                        t->changed = true;
                        view.setPosition(arduino_clock::ClockView::Position::Hour);
                        break;
                        }
                    
                }
                alarm.setAlarm( alarm_h, alarm_m, alarm_s);
                alarm.enable();
                level++;
                current = 0;
            }
            if(level == 3){
                view.setMode(arduino_clock::ClockView::Mode::Clock);
                set_check--;
                level = 0;
                if(set_check != 0){
                  t = new arduino_clock::Time();
                  view.setMode(arduino_clock::ClockView::Mode::Setup,t);
                }
               
            }
        }
        irrecv.resume();
    }
    
    
    /* Render source write here */
    view.render();
}
