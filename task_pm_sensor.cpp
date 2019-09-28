#define ARDUINO_SAMD_VARIANT_COMPLIANCE

#include "SdsDustSensor.h"

#include "measurement.h"
#include "task_pm_sensor.h"

#define SDS_011_SERIAL Serial2

void initSDS011(SdsDustSensor& sds);
void doPMMeasurement(SdsDustSensor& sds, PMMeasurement& measurement);

TaskPMSensorContext taskPMSensorContext;

// @thread
void taskPMSensor(void* p) {
    SdsDustSensor sds(SDS_011_SERIAL);

    initSDS011(sds);

    // Wait for the sensor to stabilise
    vTaskDelay(5000);

    while (1) {
        PMMeasurement measurement;
        doPMMeasurement(sds, measurement);
        xQueueSendToBack(taskPMSensorContext.pmDataQueue, &measurement, 1000);
        vTaskDelay(1000);
    }
}

void initSDS011(SdsDustSensor& sds) {
    sds.begin();

    sds.wakeupUnsafe();
    auto wakRes = sds.wakeup();
    if (!wakRes.isOk()) {
        Serial.println(wakRes.statusToString());
        // display.showError("sds fail");
        while (1);
    }

    auto queryRes = sds.setQueryReportingMode(); // only report when asked
    if (!queryRes.isOk()) {
        Serial.println(queryRes.statusToString());
        // display.showError("sds fail");
        while (1);
    }

    auto workRes = sds.setContinuousWorkingPeriod(); // run continuously
    if (!workRes.isOk()) {
        Serial.println(workRes.statusToString());
        // display.showError("sds fail");
        while (1);
    }
}

void doPMMeasurement(SdsDustSensor& sds, PMMeasurement& measurement) {
    PmResult result = sds.queryPm();
    if (result.isOk()) {
        measurement.err = 0;
        measurement.pm25 = result.pm25;
        measurement.pm10 = result.pm10;
    } else {
        Serial.println(result.statusToString());
        measurement.err = 1;
    }
}
