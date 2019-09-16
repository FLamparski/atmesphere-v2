#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_CCS811.h"
#include "BlueDot_BME280.h"
#include "SdsDustSensor.h"
#include "co2_thing_display.h"
#include "co2_thing_pubsub.h"
#include "measurement.h"

#define DISPLAY_ADDRESS 0x3C
#define CO2_SENSOR_ADDRESS 0x5B
#define BME280_ADDRESS 0x76
#define SDS_011_SERIAL Serial2
#define LED_PIN 4
#define CCS_WAK_PIN 19
#define CCS_RST_PIN 5
#define CCS_OK 0
#define N_SAMPLES_PER_ROUND 10

CO2ThingDisplay display(DISPLAY_ADDRESS);
CO2ThingPubsub pubsub;
Adafruit_CCS811 ccs;
BlueDot_BME280 bme;
SdsDustSensor sds(SDS_011_SERIAL);

void initBME280();
void initCCS881();
void initSDS011();
void doCO2Measurement(CO2Measurement& measurement);
void doPMMeasurement(PMMeasurement& measurement);

void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(LED_PIN, OUTPUT);
    pinMode(CCS_WAK_PIN, OUTPUT);
    pinMode(CCS_RST_PIN, OUTPUT);

    display.begin();
    display.showSplash();
    delay(1000);

    initBME280();
    display.showMessage("BME280 ok");

    initCCS881();
    display.showMessage("CCS881 ok");

    initSDS011();
    display.showMessage("SDS011 ok");

    /*auto pubsubStatus = pubsub.connect();
    if (pubsubStatus != PUBSUB_OK) {
        display.showError("connect fail");
        while (1);
    }*/
    display.showMessage("connect ok");
    delay(1000);

    display.clear();
}

size_t loopCount = 15;
void loop() {
    digitalWrite(LED_PIN, HIGH);
    auto pubsubStatus = pubsub.loop();
    if (pubsubStatus != PUBSUB_OK) {
        ESP.restart();
    }

    CO2Measurement co2Meas;
    co2Meas.err = 0;
    doCO2Measurement(co2Meas);
    pubsub.sendMeasurement(co2Meas);

    // loop() lasts at least 2s per call,
    // so it's at least 30s per 15 calls.
    if (loopCount >= 15) {
        PMMeasurement pmMeas;
        pmMeas.err = 0;
        doPMMeasurement(pmMeas);
        pubsub.sendMeasurement(pmMeas);
        loopCount = 0;
    }

    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(1500);
    loopCount++;
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
    digitalWrite(CCS_WAK_PIN, LOW);
    digitalWrite(CCS_RST_PIN, HIGH);

    if (!ccs.begin(CO2_SENSOR_ADDRESS)) {
        display.showError("sensor fail");
        while (1);
    }
    while (!ccs.available());
}

void initSDS011() {
    sds.begin();
    sds.wakeup();
    sds.setQueryReportingMode(); // only report when asked
    sds.setContinuousWorkingPeriod(); // run continuously
    sds.sleep();
}

void doCO2Measurement(CO2Measurement& measurement) {
    float temperature = bme.readTempC();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure();

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

            // TODO: Decouple UI and main loop() task
            display.showData(eCO2, tVOC);
        }
        else {
            measurement.err = 1;
        }
    }
    else {
        measurement.err = 1;
    }
}

void doPMMeasurement(PMMeasurement& measurement) {
    sds.wakeup();
    delay(1000);

    int n_ok = 0;
    int n_fail = 0;
    float sum_pm25 = 0.0f;
    float sum_pm10 = 0.0f;
    for (int n = 0; n < N_SAMPLES_PER_ROUND; n++) {
        PmResult res = sds.queryPm();
        if (res.isOk()) {
            n_ok++;
            sum_pm25 += res.pm25;
            sum_pm10 += res.pm10;
        } else {
            n_fail++;
        }
        pubsub.loop();
        delay(1000);
    }
    Serial.println();

    float avg_pm25 = sum_pm25 / n_ok;
    float avg_pm10 = sum_pm10 / n_ok;

    measurement.pm25 = avg_pm25;
    measurement.pm10 = avg_pm10;
    measurement.err = n_fail;

    sds.sleep();
}
