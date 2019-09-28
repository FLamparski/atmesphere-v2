#if !defined(CO2_THING_DISPLAY_H)
#define CO2_THING_DISPLAY_H

#include <Arduino.h>
#include "SSD1306Wire.h"

class CO2ThingDisplay {
    public:
        CO2ThingDisplay(byte addr);

        void begin();
        void clear();

        void showSplash();
        void showMessage(String text);
        void showError(String text);
        void showData(uint16_t eCO2, uint16_t tVOC);
        void showPM(float pm25, float pm10);
    private:
        SSD1306Wire _display;
};

#endif // CO2_THING_DISPLAY_H
