#if !defined(TASK_PM_SENSOR_H)
#define TASK_PM_SENSOR_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

typedef struct TaskPMSensorContext {
    xQueueHandle pmDataQueue;
} TaskPMSensorContext;

extern TaskPMSensorContext taskPMSensorContext;

void taskPMSensor(void* p);

#endif // TASK_PM_SENSOR_H
