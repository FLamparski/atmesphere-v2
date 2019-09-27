#if !defined(TASK_CO2_SENSOR_H)
#define TASK_CO2_SENSOR_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

typedef struct TaskCO2SensorContext {
    xQueueHandle co2DataQueue;
} TaskCO2SensorContext;

extern TaskCO2SensorContext taskCO2SensorContext;

void taskCO2Sensor(void* p);

#endif // TASK_CO2_SENSOR_H
