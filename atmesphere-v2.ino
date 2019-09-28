#include <Arduino.h>
#include <Wire.h>
// #include "co2_thing_pubsub.h"
#include "measurement.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/StackMacros.h>
#include "task_pm_sensor.h"
#include "task_co2_sensor.h"
#include "task_display.h"

#define LED_PIN 4

// CO2ThingPubsub pubsub;

xQueueHandle pmDataQueue;
xQueueHandle co2DataQueue;
xQueueHandle displayQueue;
xSemaphoreHandle i2cSemaphore;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(LED_PIN, OUTPUT);

    i2cSemaphore = xSemaphoreCreateMutex();

    pmDataQueue = xQueueCreate(1, sizeof(PMMeasurement));
    co2DataQueue = xQueueCreate(1, sizeof(CO2Measurement));
    displayQueue = xQueueCreate(1, sizeof(DisplayRequest));

    taskDisplayContext.displayRequestQueue = displayQueue;
    taskDisplayContext.i2cSemaphore = i2cSemaphore;
    xTaskCreate(taskDisplay, "display", 16 * 1024, NULL, 1, NULL);
    Serial.println("display task started");

    taskPMSensorContext.pmDataQueue = pmDataQueue;
    xTaskCreate(taskPMSensor, "pmSensor", 2048, NULL, 1, NULL);
    Serial.println("pmSensor task started");

    taskCO2SensorContext.co2DataQueue = co2DataQueue;
    taskCO2SensorContext.i2cSemaphore = i2cSemaphore;
    xTaskCreate(taskCO2Sensor, "co2Sensor", 2048, NULL, 1, NULL);

    Serial.println("co2Sensor task started");
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    PMMeasurement pmMeasurement;
    CO2Measurement co2Measurement;
    if (xQueueReceive(taskPMSensorContext.pmDataQueue, &pmMeasurement, 1000) &&
        xQueueReceive(taskCO2SensorContext.co2DataQueue, &co2Measurement, 1000)) {
        digitalWrite(LED_PIN, LOW);

        Serial.print("[main] pm2.5 = ");
        Serial.print(pmMeasurement.pm25);
        Serial.print("; pm10 = ");
        Serial.println(pmMeasurement.pm10);

        DisplayUpdateDataRequest dataReq;
        dataReq.co2Measurement = co2Measurement;
        dataReq.pmMeasurement = pmMeasurement;
        DisplayRequest req;
        req.type = DISPLAY_REQUEST_UPDATE_DATA;
        req.update = dataReq;
        xQueueSendToFront(displayQueue, &req, 500);
    }
    vTaskDelay(1000);
}
