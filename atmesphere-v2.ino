#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_CCS811.h"
#include "BlueDot_BME280.h"
#include "co2_thing_display.h"
#include "co2_thing_pubsub.h"
#include "measurement.h"

#define DISPLAY_ADDRESS 0x3C
#define CO2_SENSOR_ADDRESS 0x5A
#define BME280_ADDRESS 0x76
#define LED_PIN 35
#define CCS_OK 0

CO2ThingDisplay display(DISPLAY_ADDRESS);
CO2ThingPubsub pubsub;
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

    auto pubsubStatus = pubsub.connect();
    if (pubsubStatus != PUBSUB_OK) {
        display.showError("connect fail");
        while (1);
    }

    display.clear();
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    auto pubsubStatus = pubsub.loop();
    if (pubsubStatus != PUBSUB_OK) {
        ESP.restart();
    }

    float temperature = bme.readTempC();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure();

    Measurement measurement;
    measurement.temperature = temperature;
    measurement.humidity = humidity;
    measurement.pressure = pressure;
    if (ccs.available()) {
        ccs.setEnvironmentalData(humidity, temperature);

        if (ccs.readData() == CCS_OK) {
            int eCO2 = ccs.geteCO2();
            int tVOC = ccs.getTVOC();
            measurement.eCO2 = eCO2;
            measurement.tVOC = tVOC;
            display.showData(eCO2, tVOC);
        }
        else {
            measurement.err = 1;
        }
    }
    else {
        measurement.err = 1;
    }

    pubsub.sendMeasurement(measurement);

    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(1500);
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
