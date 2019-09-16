#if !defined(CO2_THING_PUBSUB_H)
#define CO2_THING_PUBSUB_H

#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "measurement.h"
#include "wifi_config.h"

// measureJson from ArduinoJson reports the JSON size WITHOUT trailing null byte,
// this helper macro adds it
#define MEASURE_JSON_BUFFER(doc) (measureJson(doc) + 1)

enum PubsubConnectResult {
    PUBSUB_OK = 0,
    PUBSUB_WIFI_FAIL,
    PUBSUB_MQTT_FAIL
};

class CO2ThingPubsub {
    public:
        CO2ThingPubsub();

        PubsubConnectResult connect();
        PubsubConnectResult loop();
        void sendMeasurement(const CO2Measurement& measurement);
        void sendMeasurement(const PMMeasurement& measurement);
    private:
        WiFiClient wifi;
        PubSubClient mqtt;
};

#endif // CO2_THING_PUBSUB_H
