/* ===========================================
 *    Address Scanner
 *    2019.12.14 v1 - by Hu
 *      implement address scanning method
 */
#if !defined(_ADDRESS_SCAN_H)
#define _ADDRESS_SCAN_H

#include "trace.h"
#include <Wire.h>
class AddressScanner {
public:
    AddressScanner() {
        if (!Wire.available()) {
            Wire.begin();
        }
        traceBegin();
        traceln("Start scanning I2C address ...");
    }
    uint8_t search() {
        uint8_t nDevices = 0;
        uint8_t first_found = 0;
        byte error, address;
        for (address = 1; address < 127; ++address) {
            Wire.beginTransmission(address);
            error = Wire.endTransmission();
            if (error == 0) {
                if (nDevices == 0) {
                    first_found = address;
                }
                
                traceln("I2C device found at address ", address);
                nDevices++;
            }
            else if (error == 4) {
                traceln("Unknown error at address ", address);
            }
        }
        if (nDevices == 0) {
            cerror("No I2C devices found");
        }
        else if (nDevices != 1) {
            traceln("Multiple device is available - address ", first_found, " retruned");
        }

        traceln("End scanning I2C address!");
        return first_found;
    }
};

#endif
