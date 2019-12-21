# Arduino Alarm Clcok
> Alarm clock using arduino

## Description
  아두이노 우노 보드와 LCD를 이용하여 만든 알람시계입니다. 현재 시간, 온습도를 출력하는 기본적인 시계 기능이 있습니다.  
  리모콘을 이용해 현재 시간 및 알람 시간을 설정하고 시간이 되면 알람이 울립니다. 알람이 울리면 간단한 수학문제를 풀도록 하여 맞출 경우 알람이 꺼지게 됩니다.  

![clock view example](https://drive.google.com/uc?id=1ONEi9GIYaKnlxnfzg-SIc_v1KZ7UiIsg)

## Circuit
![circuit](https://drive.google.com/uc?id=1l1QtgGdZYFnbxV_Xk3Sjc1VKFYMkkoij)

### Used pins
PIN    | Use
------ | --------------
P12    | for Buzzer
P9     | for IR sensor
P7     | for DHT sensor
A4, A5 | for I2C (LCD)
timer0 | for millis
timer1 | for IR
timer2 | for tone

## Requirements
[DHT sensor library](https://github.com/adafruit/DHT-sensor-library) by Adafruit v 1.3.8  
[LiquidCrystal I2C](https://github.com/johnrickman/LiquidCrystal_I2C) by Frank de Brabander v 1.1.2  
[IRremote](https://github.com/z3t0/Arduino-IRremote) by shirriff v 2.2.3  


## Credits
* tristan3716
* NeedIDcard
* a4016065
