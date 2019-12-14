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
 *    global variable & method
 *    2019.12.13 v1 - by Hu
 */
arduino_clock::Time time;
arduino_clock::lcd_handle::ClockView view;

void time_pass(){
    time.increase();
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
}

void loop() {
    /* Update source write here */
    //! TODO:
    
    /* Render source write here */
    view.render();
}
