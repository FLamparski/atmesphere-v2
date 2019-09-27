#if !defined(TASK_DISPLAY_H)
#define TASK_DISPLAY_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "measurement.h"

typedef struct TaskDisplayContext {
    xQueueHandle displayRequestQueue;
} TaskDisplayContext;

extern TaskDisplayContext taskDisplayContext;

void taskDisplay(void* p);

enum DisplayRequestType {
    DISPLAY_REQUEST_UPDATE_DATA,
    DISPLAY_REQUEST_MESSAGE,
    DISPLAY_REQUEST_SPLASH
};

typedef struct DisplayUpdateDataRequest {
    PMMeasurement* pmMeasurement;
    CO2Measurement* co2Measurement;
} DisplayUpdateDataRequest;

typedef struct DisplayMessageRequest {
    const char* message;
    bool isError;
    int time;
} DisplayMessageRequest;

typedef struct DisplayRequest {
    DisplayRequestType type;
    union {
        DisplayUpdateDataRequest update;
        DisplayMessageRequest message;
    };
} DisplayRequest;

#endif // TASK_DISPLAY_H
