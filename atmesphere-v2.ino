#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_CCS811.h"
#include "display/co2_thing_display.h"

#define DISPLAY_ADDRESS 0x3C
#define CO2_SENSOR_ADDRESS 0x5A

CO2ThingDisplay display(DISPLAY_ADDRESS);
Adafruit_CCS811 ccs;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    display.begin();
    display.showSplash();

    if (!ccs.begin(CO2_SENSOR_ADDRESS)) {
        display.showError("sensor fail");
        while (1);
    }

    while (!ccs.available());
}

void loop() {
    if (ccs.available()) {
        if (ccs.readData() == 0) {
            display.showData(ccs.geteCO2(), ccs.getTVOC());
        }
    }
    delay(1000);
}
