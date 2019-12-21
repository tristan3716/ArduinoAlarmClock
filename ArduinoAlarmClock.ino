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

#include "ClockTime.h"
#include "LCDHandler.h"
#include "IRHandler.h"
#include "ClockAlarm.h"

/* ===========================================
 *    Arduino main
 */
void setup() {
    randomSeed(analogRead(A0));
    release.init();
    release.log(F("Arduino Alarm Clock v1220"));
    
    // initialize time
    real_time.init();
    // initialize lcd handler
    view.init(real_time);
    // initialize alarm
    alarm.init(real_time);
    IRinit();
}

void loop() {
    /* Clock Timer */
    time_pass();

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
