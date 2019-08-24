# atmesphere v2

An air quality station using:

* AMS CCS811
* Bosch BME280
* nova SDS011

Plus: an OLED screen, a programmable LED, a button, and ESP32 for Bluetooth, BLE, and WiFi
connectivity.

This is very much a work in progress - I don't even have the V2 boards yet.

## Hardware

The hardware design is hosted on EasyEDA:

https://easyeda.com/filiplamparski/atmesphere-v2

## Wait, where's V1?

See this writeup: https://dev.to/minkovsky/working-on-my-iot-air-quality-monitoring-setup-40a5
I also have a board without the UART connector that fits the CCS811 breakout and a BME280 breakout.

## Building

You'll need to add a `wifi_config.h` - use the sample in `docs/` to customise for your own
network.

## Visual Studio Code settings

Check `docs/c_cpp_properties.sample.json`. Replace `%USERNAME%` with your own username or change
the paths to where you installed Arduino + libraries. Replace the compiler path to the one you
have with your ESP32 Arduino core.

Use this as good defaults for the `arduino.json` file:

```json
{
    "board": "esp32:esp32:esp32",
    "configuration": "PSRAM=disabled,PartitionScheme=default,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none"
}
```