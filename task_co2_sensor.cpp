#include "Adafruit_CCS811.h"
#include "BlueDot_BME280.h"

#include "task_co2_sensor.h"
#include "measurement.h"

#define CO2_SENSOR_ADDRESS 0x5B
#define BME280_ADDRESS 0x76
#define CCS_WAK_PIN 19
#define CCS_RST_PIN 5
#define CCS_OK 0

void initBME280(BlueDot_BME280& bme);
void initCCS881(Adafruit_CCS811& ccs);
void doCO2Measurement(BlueDot_BME280& bme, Adafruit_CCS811& ccs, CO2Measurement& measurement);

TaskCO2SensorContext taskCO2SensorContext;

void taskCO2Sensor(void* p) {
    Adafruit_CCS811 ccs;
    BlueDot_BME280 bme;

    pinMode(CCS_WAK_PIN, OUTPUT);
    pinMode(CCS_RST_PIN, OUTPUT);

    Serial.println("[co2Sensor] waiting for I2C semaphore...");
    xSemaphoreTake(taskCO2SensorContext.i2cSemaphore, portMAX_DELAY);
    Serial.println("[co2Sensor] I2C semaphore acquired");
    initBME280(bme);
    initCCS881(ccs);
    xSemaphoreGive(taskCO2SensorContext.i2cSemaphore);
    Serial.println("[co2Sensor] I2C semaphore released, all initialised");

    while (1) {
        CO2Measurement measurement;
        doCO2Measurement(bme, ccs, measurement);
        xQueueSendToBack(taskCO2SensorContext.co2DataQueue, &measurement, 1000);
        vTaskDelay(1000);
    }
}

void doCO2Measurement(BlueDot_BME280& bme, Adafruit_CCS811& ccs, CO2Measurement& measurement) {
    xSemaphoreTake(taskCO2SensorContext.i2cSemaphore, portMAX_DELAY);
    float temperature = bme.readTempC();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure();
    xSemaphoreGive(taskCO2SensorContext.i2cSemaphore);

    measurement.temperature = temperature;
    measurement.humidity = humidity;
    measurement.pressure = pressure;

    xSemaphoreTake(taskCO2SensorContext.i2cSemaphore, portMAX_DELAY);
    if (ccs.available()) {
        ccs.setEnvironmentalData(humidity, temperature);

        if (ccs.readData() == CCS_OK) {
            int eCO2 = ccs.geteCO2();
            int tVOC = ccs.getTVOC();
            measurement.eCO2 = eCO2;
            measurement.tVOC = tVOC;
        }
        else {
            measurement.err = 1;
        }
    }
    else {
        measurement.err = 1;
    }
    xSemaphoreGive(taskCO2SensorContext.i2cSemaphore);
}

void initBME280(BlueDot_BME280& bme) {
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
        // display.showError("bme fail");
        Serial.print("[taskCO2Sensor] BME280 fail? Got ID ");
        Serial.println(bmeId, HEX);
        xSemaphoreGive(taskCO2SensorContext.i2cSemaphore);
        vTaskDelete(NULL);
    }
}

void initCCS881(Adafruit_CCS811& ccs) {
    digitalWrite(CCS_WAK_PIN, LOW);
    digitalWrite(CCS_RST_PIN, HIGH);

    if (!ccs.begin(CO2_SENSOR_ADDRESS)) {
        Serial.println("[taskCO2Sensor] CCS881 fail");
        xSemaphoreGive(taskCO2SensorContext.i2cSemaphore);
        vTaskDelete(NULL);
    }
    while (!ccs.available());
}
