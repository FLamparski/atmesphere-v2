#if !defined(MEASUREMENT_H)
#define MEASUREMENT_H

typedef struct Measurement {
    int eCO2;
    int tVOC;
    float temperature;
    float pressure;
    float humidity;
    int err;
} Measurement;

#endif // MEASUREMENT_H
