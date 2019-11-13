#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <BLE2904.h>

#include "task_ble.h"

// BLE services.

// Custom CO2+TVOC service
#define SERVICE_CO2 "09dfed20-fda4-422a-a3a4-8cd5d9a83a11"
// Report tVOC in PPM
#define CHARACTERISTIC_CO2_TVOC "09dfed21-fda4-422a-a3a4-8cd5d9a83a11"
// Report eCO2 in PPM
#define CHARACTERISTIC_CO2_ECO2 "09dfed22-fda4-422a-a3a4-8cd5d9a83a11"

// Standard environment service
#define SERVICE_ENVIRONMENT "0000181A-50c7-4b54-8094-a5dded87cdae"
// BLE: Unit is in pascals with a resolution of 0.1 Pa
#define CHARACTERISTIC_ENVIRONMENT_PRESSURE "00002A6D-50c7-4b54-8094-a5dded87cdae"
// BLE: Unit is in degrees Celsius with a resolution of 0.01 degrees Celsius
#define CHARACTERISTIC_ENVIRONMENT_TEMPERATURE "00002A6E-50c7-4b54-8094-a5dded87cdae"
// BLE: Unit is in percent with a resolution of 0.01 percent
#define CHARACTERISTIC_ENVIRONMENT_HUMIDITY "00002A6F-50c7-4b54-8094-a5dded87cdae"

// Custom particulate matter service
#define SERVICE_PM "d17408d0-f778-4000-8eb5-6d808befd10b"
// Report PM2.5 concentration as ug/m^3
#define CHARACTERISTIC_PM_PM25 "d17408d1-f778-4000-8eb5-6d808befd10b"
// Report PM10 concentration as ug/m^3
#define CHARACTERISTIC_PM_PM10 "d17408d2-f778-4000-8eb5-6d808befd10b"

// Nordic nRF semi-standard UART control channel
#define SERVICE_SERIAL "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
// RX, bytes
#define CHARACTERISTIC_SERIAL_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
// TX, bytes
#define CHARACTERISTIC_SERIAL_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define BUILD_SERIAL "0000"
#define DEVICE_NAME "atmESPhere v2 " BUILD_SERIAL
#define MAX_MTU 517

TaskBleContext taskBleContext;

BLECharacteristic* createNotifyingCharacteristic(BLEService* service, const char* guid);
void addIntegerBLE2904(BLECharacteristic* characteristic);

void taskBle(void* p) {
    BLEDevice::init(DEVICE_NAME);
    BLEDevice::setMTU(MAX_MTU);

    auto server = BLEDevice::createServer();

    auto co2Service = server->createService(SERVICE_CO2);
    auto eCo2Characteristic = createNotifyingCharacteristic(co2Service, CHARACTERISTIC_CO2_ECO2);
    addIntegerBLE2904(eCo2Characteristic);
    auto tVocCharacteristic = createNotifyingCharacteristic(co2Service, CHARACTERISTIC_CO2_TVOC);
    addIntegerBLE2904(tVocCharacteristic);

    co2Service->start();

    auto advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_CO2);
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x06);
    advertising->setMaxPreferred(0x12);
    BLEDevice::startAdvertising();

    PMMeasurement pmMeasurement;
    bool hasFirstPMMeasurement = false;
    CO2Measurement co2Measurement;
    bool newCO2Measurement = false;
    while(1) {
        BleData data;
        if (xQueueReceive(taskBleContext.bleDataQueue, &data, 10)) {
            if (data.type == BLE_DATA_TYPE_CO2) {
                newCO2Measurement = true;
                co2Measurement.eCO2 = data.co2Measurement.eCO2;
                co2Measurement.tVOC = data.co2Measurement.tVOC;
            }
        }

        if (newCO2Measurement) {
            eCo2Characteristic->setValue(co2Measurement.eCO2);
            eCo2Characteristic->notify(true);
            tVocCharacteristic->setValue(co2Measurement.tVOC);
            tVocCharacteristic->notify(true);
            newCO2Measurement = false;
        }

        vTaskDelay(10);
    }
}

BLECharacteristic* createNotifyingCharacteristic(BLEService* service, const char* guid) {
    auto characteristic = service->createCharacteristic(
        guid,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    characteristic->addDescriptor(new BLE2902());
    return characteristic;
}

void addIntegerBLE2904(BLECharacteristic* characteristic) {
    auto ble2904 = new BLE2904();
    ble2904->setFormat(BLE2904::FORMAT_UINT32);
    characteristic->addDescriptor(ble2904);
}