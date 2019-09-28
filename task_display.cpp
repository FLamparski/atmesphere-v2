#include "task_display.h"
#include "co2_thing_display.h"

#define UI_BTN 27
#define DISPLAY_ADDRESS 0x3C
#define STATE_CO2 1
#define STATE_PM 2
#define STATE_ERR 3

CO2ThingDisplay display(DISPLAY_ADDRESS);

void taskDisplay(void* p) {
    pinMode(UI_BTN, INPUT);

    display.begin();
    display.showSplash();
    delay(1000);

    display.clear();

    PMMeasurement* pmMeasurement = NULL;
    CO2Measurement* co2Measurement = NULL;
    int state = STATE_PM;

    int btnPress = HIGH;
    while (1) {
        // Serve the next display request
        DisplayRequest request;
        if (xQueueReceive(taskDisplayContext.displayRequestQueue, &request, 0)) {
            switch (request.type) {
                case DISPLAY_REQUEST_UPDATE_DATA:
                    pmMeasurement = request.update.pmMeasurement;
                    co2Measurement = request.update.co2Measurement;
                    break;

                case DISPLAY_REQUEST_MESSAGE:
                    if (request.message.isError) {
                        display.clear();
                        display.showError(String(request.message.message));
                        state = STATE_ERR;
                    } else {
                        display.clear();
                        display.showMessage(String(request.message.message));
                        vTaskDelay(request.message.time);
                    }
                    break;
            }
        }

        // Update the state if the button has been pressed
        if (btnPress == HIGH && digitalRead(UI_BTN) == LOW) {
            switch (state)
            {
                case STATE_CO2:
                    state = STATE_PM;
                    break;

                case STATE_PM:
                    state = STATE_CO2;
                    break;

                default:
                    state = STATE_CO2;
                    break;
            }
            btnPress = LOW;
        } else if (btnPress == LOW && digitalRead(UI_BTN) == HIGH) {
            btnPress = HIGH;
        }

        switch (state) {
            case STATE_CO2:
                display.showData(co2Measurement->eCO2, co2Measurement->tVOC);
                break;
            case STATE_PM:
                display.showPM(pmMeasurement->pm25, pmMeasurement->pm10);
                break;
        }
    }
}
