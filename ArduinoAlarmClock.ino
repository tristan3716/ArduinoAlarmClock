//#define _DEBUG

/* //! IMPORTANT
 *  사용한 핀을 기술할 것.
 *      아날로그 핀 : A0, A1, A2, A3, A4, A5
 *      디지털 핀 : P2, P3*, P4, P5*, P6*, P7, P8, P9, P10, P11*, P12*, P13  (* is PWM)
 *      
 *  == Used Pin ==
 *  A4, A5 - for I2C LCD module
 *  P5*, P6* - for timer for millis (timer0)
 *  P9*, P10* - for IR sensor (timer1)
 *  P3*, P11* - for tone (timer2)
 *  P12 - for buzzer
 *  P9 - for ir sensor
 *  P7 - for DHT
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
 *    IR Library - IRremote
 */

#include <IRremote.h>
int IRpin = 9;
IRrecv irrecv(IRpin);
decode_results results;

/* ===========================================
 *   Alram 
 *   2019.12.17 v1 - by Jh
 *   2019.12.19 v2 - by Jh
 */

#include "ClockAlarm.h"
arduino_clock::Alarm alarm;
/* ===========================================
 *    global variable & method
 *    2019.12.13 v1 - by Hu
 */

uint32_t ot;
uint32_t dt = 1000;
//arduino_clock::Time time;
//arduino_clock::ClockView view;

void time_pass() {
    time.increase();
}

#include "IRHandler.h"

/* ===========================================
 *    Arduino main
 */
void setup() {
    randomSeed(analogRead(A0));
    release.init();
    release.log(F("Arduino Alarm Clock v1220"));

    pinMode(12, OUTPUT);

    // initialize time
    time.init();
    // initialize lcd handler
    view.init(time);
    
    // irrecv standby
    irrecv.enableIRIn();

    // alarm
    alarm.init(time);

    ot = millis();
}

void loop() {
    /* Clock Timer */
    if (ot + dt < millis()) {
        ot = ot + dt;
        time_pass();
    }

    /* Update source write here */
    if (irrecv.decode(&results)) {
        IRProcess();
    }

    /* Render source write here */
    view.render();

    /* Alarm source write here */
    if (alarm.isAlarmWarn()) {
        alarm.warn();
    }
}
