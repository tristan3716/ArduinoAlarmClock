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
arduino_clock::Time time;
arduino_clock::lcd_handle::ClockView view;

void time_pass(){
    time.increase();
}
int set_check = 0;              
int Temp_time[2] = {0,0}
int level = 0;
/* ============================================
 *    enum of IRsignal values
 *    2019.12.17 - by Jo
 */
arduino_clock::enum IRSignal {
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

arduino_clock::char* decoding() {
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
    
    // initialize time
    time.init();
    // initialize lcd handler
    view.init(time);
    
    // initialize Timer
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(time_pass);
    
    
    // irrecv standby
    irrecv.enableIRIn();
}

void loop() {
    /* Update source write here */
    //! TODO:
    /* turn to setup mode*/
    if(irrecv.decode(&results){
        sig_in = decoding();
        if(sig_in == "fn" && set_check != 1){
            arduino_clock::Time *t = new arduino_clock::Time();
            view.setMode(arduino_clock::Clock_View::Mode::setup, t);
        }elif(sig_in != "fn" && sig_in != "-1" && set_check = 1){
            Temp_time[current] = atoi(sig_in);
            ++current;
            if(current == 2){
                int t_time = Temp_time[0] * 10 + Temp_time[1];
                switch(level){
                    case 0:
                        t->hour = t_time;
                        t->changed = true;
                        break;
                    case 1:
                        t->minuate = t_time;
                        t->changed = true;
                        break;
                    case 2:
                        t->second = t_time;
                        t->changed = true;
                        break;
                }
                level++;
                current = 0;
            }
            if(level == 3){
                set_check = 0;
                view.setMode(arduino_clock::Clock_View::Mode::Clock,t);
                level = 0;
            
    } //오후 5시에 시험 끝나니 그 후에 수정및 추가 더 하겠습니다. 일단 세팅하는 것만 생각나는 대로 짜봤습니다. by-JO
    
    
    /* Render source write here */
    view.render();
}
