#include "task_display.h"
#include "co2_thing_display.h"

#define DISPLAY_ADDRESS 0x3C
#define STATE_CO2 1
#define STATE_PM 2
#define STATE_ERR 3

CO2ThingDisplay display(DISPLAY_ADDRESS);

void taskDisplay(void* p) {
    display.begin();
    display.showSplash();
    delay(1000);

    display.clear();

    PMMeasurement* pmMeasurement = NULL;
    CO2Measurement* co2Measurement = NULL;
    int state = STATE_PM;
    
    while (1) {
        //
    }
}
