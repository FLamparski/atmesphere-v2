#include <Arduino.h>
#include <Wire.h>
#include "co2_thing_pubsub.h"
#include "measurement.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/StackMacros.h>
#include "task_pm_sensor.h"
#include "task_co2_sensor.h"

#define LED_PIN 4

CO2ThingPubsub pubsub;

xQueueHandle pmDataQueue;
xQueueHandle co2DataQueue;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(LED_PIN, OUTPUT);

    pmDataQueue = xQueueCreate(1, sizeof(PMMeasurement*));
    co2DataQueue = xQueueCreate(1, sizeof(CO2Measurement*));

    taskPMSensorContext.pmDataQueue = pmDataQueue;
    xTaskCreate(taskPMSensor, "pmSensor", 2048, NULL, 1, NULL);

    taskCO2SensorContext.co2DataQueue = co2DataQueue;
    xTaskCreate(taskCO2Sensor, "co2Sensor", 2048, NULL, 1, NULL);
}

void loop() {
    // digitalWrite(LED_PIN, HIGH);
    // auto pubsubStatus = pubsub.loop();
    // if (pubsubStatus != PUBSUB_OK) {
    //     ESP.restart();
    // }

    // CO2Measurement co2Meas;
    // co2Meas.err = 0;
    // doCO2Measurement(co2Meas);
    // pubsub.sendMeasurement(co2Meas);

    // // loop() lasts at least 2s per call,
    // // so it's at least 30s per 15 calls.
    // if (loopCount >= 15) {
    //     PMMeasurement pmMeas;
    //     pmMeas.err = 0;
    //     doPMMeasurement(pmMeas);
    //     pubsub.sendMeasurement(pmMeas);
    //     loopCount = 0;
    // }

    // delay(500);
    // digitalWrite(LED_PIN, LOW);
    // delay(1500);
    // loopCount++;

    PMMeasurement measurement;
    if (xQueueReceive(taskPMSensorContext.pmDataQueue, &measurement, 1000)) {
        Serial.print("Received measurement (pm_2.5, pm_10, err): ");
        Serial.print(measurement.pm25);
        Serial.print(" ");
        Serial.print(measurement.pm10);
        Serial.print(" ");
        Serial.print(measurement.err);
        Serial.println();
    }
}





