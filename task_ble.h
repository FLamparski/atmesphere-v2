#if !defined(TASK_BLE_H)
#define TASK_BLE_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "measurement.h"

typedef struct TaskBleContext {
    xQueueHandle bleDataQueue;
    xQueueHandle bleTxQueue;
    xQueueHandle bleRxQueue;
} TaskBleContext;

void taskBle(void* p);

extern TaskBleContext taskBleContext;

enum BleDataType {
    BLE_DATA_TYPE_PM,
    BLE_DATA_TYPE_CO2
};

typedef struct BleData {
    BleDataType type;
    union {
        PMMeasurement pmMeasurement;
        CO2Measurement co2Measurement;
    };
} BleData;

#endif // TASK_BLE_H
