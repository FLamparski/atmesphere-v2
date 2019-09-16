Tasks to be used with the atmESPhere v2:

1. CO2/Environment sensor
2. Dust/pollution sensor
3. UI
4. Comms

## CO2/Environment sensor

Batch task.

1. Read temperature, pressure, and humidity from the BME280.
2. Update the CCS811 with temperature and humidity.
3. Read tVOC and eCO2 from the CCS811.
4. Post the combined reading to the comms thread somehow.

This task will run every 5 seconds.

## Dust/pollution sensor

Batch task.

1. Wake up sensor.
2. Run for ten seconds, gathering data.
3. Post average reading to the comms thread somehow.
4. Put sensor to sleep.

This task will run every 30 seconds.

## UI

This task will:

* Monitor the button, differentiating a short and a long press.
* Control the LED pattern.
* Update the display.

This task is interactive.

## Comms

This task will:

* Maintain connection with the MQTT server.
* Listen for incoming readings.
* When readings are received, inform the UI thread somehow.
* Send readings by MQTT.
