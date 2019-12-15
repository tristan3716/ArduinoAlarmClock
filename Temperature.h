#if !defined (_TEMPERATURE_H)
#define _TEMPERATURE_H

#include "DHT.h"

#define DHTPIN 7
#define DHTTYPE DHT11
extern DHT dht;
DHT dht(DHTPIN, DHTTYPE);

float getTemperature(){
    return dht.readTemperature();
}

float getHumidity() {
    return dht.readHumidity();
}



#endif //! _TEMPERATURE_H
