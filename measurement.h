#if !defined(MEASUREMENT_H)
#define MEASUREMENT_H

typedef struct CO2Measurement {
    int eCO2;
    int tVOC;
    float temperature;
    float pressure;
    float humidity;
    int err;
} CO2Measurement;

typedef struct PMMeasurement {
    float pm25;
    float pm10;
    int err;
} PMMeasurement;

#endif // MEASUREMENT_H
