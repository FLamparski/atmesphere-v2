#include "co2_thing_pubsub.h"

CO2ThingPubsub::CO2ThingPubsub()
: wifi(), mqtt(wifi)
{}

void CO2ThingPubsub::connect() {
    if (!WiFi.isConnected()) {
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        WiFi.setAutoConnect(true);

        for (size_t n = 0; n < WIFI_MAX_TRIES && !WiFi.isConnected(); n++) {
            delay(1000);
        }

        if (!WiFi.isConnected()) {
            Serial.println("Could not connect to WiFi");
            return;
        }
    }

    mqtt.setServer(MQTT_HOST, MQTT_PORT);
    
    for (size_t n = 0; n < WIFI_MAX_TRIES && !mqtt.connected(); n++) {
        mqtt.connect(CLIENT_ID);
        delay(1000);
    }
}

void CO2ThingPubsub::sendMeasurement(const Measurement& measurement) {
    StaticJsonDocument<512> doc;
    doc["client"] = CLIENT_ID;
    doc["eCO2"] = measurement.eCO2;
    doc["tVOC"] = measurement.tVOC;
    doc["temperature"] = measurement.temperature;
    doc["humidity"] = measurement.humidity;
    doc["pressure"] = measurement.pressure;
    doc["err"] = measurement.err;

    size_t jsonLen = MEASURE_JSON_BUFFER(doc);
    char payload[jsonLen];
    serializeJson(doc, payload, jsonLen);

    mqtt.publish(TOPIC, payload);
}

void CO2ThingPubsub::loop() {
    mqtt.loop();
}
