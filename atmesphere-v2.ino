#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_CCS811.h"
#include "BlueDot_BME280.h"
#include "co2_thing_display.h"

#define DISPLAY_ADDRESS 0x3C
#define CO2_SENSOR_ADDRESS 0x5A
#define BME280_ADDRESS 0x76

CO2ThingDisplay display(DISPLAY_ADDRESS);
Adafruit_CCS811 ccs;
BlueDot_BME280 bme;

void initBME280();
void initCCS881();

void setup() {
    Serial.begin(115200);
    Wire.begin();

    display.begin();
    display.showSplash();

    initBME280();

    initCCS881();

    display.clear();
}

void loop() {
    ccs.setEnvironmentalData(bme.readHumidity(), bme.readTempC());

    if (ccs.available()) {
        if (ccs.readData() == 0) {
            display.showData(ccs.geteCO2(), ccs.getTVOC());
        }
    }

    Serial.print("temp = ");
    Serial.print(bme.readTempC());
    Serial.println("C");

    Serial.print("pressure = ");
    Serial.print(bme.readPressure());
    Serial.println("hPa");

    Serial.print("humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println("%");

    delay(1000);
}

void initBME280() {
    bme.parameter.communication = 0; // I2C
    bme.parameter.I2CAddress = BME280_ADDRESS;
    bme.parameter.sensorMode = 0b11;
    bme.parameter.IIRfilter = 0b100;
    bme.parameter.humidOversampling = 0b101;
    bme.parameter.tempOversampling = 0b101;
    bme.parameter.pressOversampling = 0b101;
    bme.parameter.pressureSeaLevel = 1013.25;

    uint8_t bmeId = bme.init();
    if (bmeId != 0x60) {
        display.showError("bme fail");
        Serial.print("BME280 fail? Got ID ");
        Serial.println(bmeId, HEX);
        while (1);
    }
}

void initCCS881() {
    if (!ccs.begin(CO2_SENSOR_ADDRESS)) {
        display.showError("sensor fail");
        while (1);
    }
    while (!ccs.available());
}
