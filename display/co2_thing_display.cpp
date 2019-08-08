#include "co2_thing_display.h";

#define DEFAULT_FONT ArialMT_Plain_10

const char* LABEL_ECO2 = "eCO2";
#define LABEL_ECO2_LEN 5
const char* LABEL_TVOC = "tVOC";
#define LABEL_TVOC_LEN 5

CO2ThingDisplay::CO2ThingDisplay(byte addr)
: _display(addr, SDA, SCL, GEOMETRY_128_32)
{}

void CO2ThingDisplay::begin() {
    _display.init();
    _display.resetDisplay();
    _display.displayOn();
}

void CO2ThingDisplay::clear() {
    _display.clear();
    _display.normalDisplay();
    _display.display();
}

void CO2ThingDisplay::showSplash() {
    clear();

    _display.setFont(ArialMT_Plain_24);
    _display.drawString(0, 0, "CO2 Thing");
    _display.setFont(DEFAULT_FONT);
    _display.display();
}

void CO2ThingDisplay::showError(String text) {
    clear();

    _display.invertDisplay();
    _display.drawString(0, 0, text);
    _display.display();
}

void CO2ThingDisplay::showData(uint16_t eCO2, uint16_t tVOC) {
    clear();

    String eCO2_str(eCO2, DEC);
    String tVOC_str(tVOC, DEC);

    _display.setFont(ArialMT_Plain_10);
    _display.drawString(0, 0, String(LABEL_ECO2));
    uint16_t tVOC_label_size = _display.getStringWidth(LABEL_TVOC, LABEL_TVOC_LEN);
    _display.drawString(_display.getWidth() - tVOC_label_size, 0, String(LABEL_TVOC));

    _display.setFont(ArialMT_Plain_16);
    _display.drawString(0, 6, eCO2_str);
    uint16_t tVOC_size = _display.getStringWidth(tVOC_str.c_str(), tVOC_str.length());
    _display.drawString(_display.getWidth() - tVOC_size, 6, tVOC_str);

    _display.display();
}
