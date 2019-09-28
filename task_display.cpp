#include "task_display.h"
#include "co2_thing_display.h"

#define UI_BTN 27
#define DISPLAY_ADDRESS 0x3C
#define STATE_CO2 1
#define STATE_PM 2
#define STATE_ERR 3
// 3000 ticks but each cycle is ~500 ticks
#define PAGE_DURATION 10

CO2ThingDisplay display(DISPLAY_ADDRESS);

TaskDisplayContext taskDisplayContext;

void taskDisplay(void* p) {
    xSemaphoreTake(taskDisplayContext.i2cSemaphore, portMAX_DELAY);
    display.begin();
    display.clear();
    display.showSplash();
    xSemaphoreGive(taskDisplayContext.i2cSemaphore);
    vTaskDelay(1000);
    Serial.println("[display] initialised");

    PMMeasurement pmMeasurement;
    bool hasFirstPMMeasurement = false;
    CO2Measurement co2Measurement;
    bool hasFirstCO2Measurement = false;
    int state = STATE_PM;
    uint32_t nTicks = 0;
    while (1) {
        // Stop accepting requests if we are in an error state;
        if (state == STATE_ERR) {
            vTaskDelay(500);
            continue;
        }

        // Serve the next display request
        DisplayRequest request;
        if (xQueueReceive(taskDisplayContext.displayRequestQueue, &request, 500)) {
            switch (request.type) {
                case DISPLAY_REQUEST_UPDATE_DATA:
                    pmMeasurement.pm25 = request.update.pmMeasurement.pm25;
                    pmMeasurement.pm10 = request.update.pmMeasurement.pm10;
                    hasFirstPMMeasurement = true;
                    co2Measurement = request.update.co2Measurement;
                    hasFirstCO2Measurement = true;
                    break;

                case DISPLAY_REQUEST_MESSAGE:
                    if (request.message.isError) {
                        xSemaphoreTake(taskDisplayContext.i2cSemaphore, portMAX_DELAY);
                        display.clear();
                        display.showError(String(request.message.message));
                        xSemaphoreGive(taskDisplayContext.i2cSemaphore);
                        state = STATE_ERR;
                        continue;
                    } else {
                        xSemaphoreTake(taskDisplayContext.i2cSemaphore, portMAX_DELAY);
                        display.clear();
                        display.showMessage(String(request.message.message));
                        xSemaphoreGive(taskDisplayContext.i2cSemaphore);
                        vTaskDelay(request.message.time);
                    }
                    break;
            }
        }

        nTicks++;
        if (nTicks >= PAGE_DURATION) {
            switch (state) {
                case STATE_PM:
                    state = STATE_CO2;
                    break;
                case STATE_CO2:
                    state = STATE_PM;
                    break;
            }
            nTicks = 0;
        }

        switch (state) {
            case STATE_CO2:
                if (hasFirstCO2Measurement) {
                    if (!xSemaphoreTake(taskDisplayContext.i2cSemaphore, 1000)) continue;
                    display.showData(co2Measurement.eCO2, co2Measurement.tVOC);
                    xSemaphoreGive(taskDisplayContext.i2cSemaphore);
                }
                break;
            case STATE_PM:
                if (hasFirstPMMeasurement) {
                    if (!xSemaphoreTake(taskDisplayContext.i2cSemaphore, 1000)) continue;
                    display.showPM(pmMeasurement.pm25, pmMeasurement.pm10);
                    xSemaphoreGive(taskDisplayContext.i2cSemaphore);
                }
                break;
        }

        vTaskDelay(500);
    }
}
