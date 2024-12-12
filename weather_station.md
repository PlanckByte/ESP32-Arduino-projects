# ESP32 Weather Station Project

This readme contains information about the source code  and the hardware for an ESP32-based weather station designed for field use. The system collects environmental data from various sensors and transmits it over a 4G connection to a remote server for monitoring and analysis. It was also pilot tested under the ATTESTED project funded by
DRG4FOOD. In order to use this code you must set up the HTTP request to your server accordingly as well as the SSID and password for your connection.

---

## Features

- **Remote Environmental Monitoring**: Real-time data collection from multiple sensors.
- **ESP32 Microcontroller**: Lightweight and efficient control system with Wi-Fi and Bluetooth capabilities.
- **4G Connectivity**: Integrated modem for reliable data transmission over long distances.
- **Custom Circuitry**: Handles sensor connections and signal stabilization.
## Missing Features anb bugs
Currently the data uploading lack security features or authentication techniques since everything was used in a test environment. The upload also lacks any retry logic in case of failure.
**Known Bugs**:
- **Issue**: The code seems to randomly skip some entries, likely due to inconsistencies in the 4G connection, especially if it drops or experiences intermittent disruptions.
  
- **Proposed Fix**: To address this, a simple retry mechanism can be programmed to handle upload failures. The retry logic should activate if the HTTP return status is outside the range of 200-299, which indicates a failure due to connection inconsistencies or network instability.

**Lux Value Returning NaN**:

- **Issue**: Sometimes, particularly at night, the Lux value may return `NaN` (Not a Number). This can occur when the sensor data is invalid, but the payload still expects a valid numeric value under the `type: "Number"` field.

- **Proposed Fix**: You can handle this by either skipping the Lux calculation entirely if it returns `NaN`, or implementing an exception to catch the error. This way, if the Lux value is invalid, it will not be included in the payload, and the upload can proceed without errors.


---

## Sensors Used

All the sensors were calibrated as accurately as possible before everything was intergrated to the main code. The system integrates the following sensors:

1. **Weather Station**: The following weather station kit was used: https://www.sparkfun.com/products/15901. These sensors contain no active electronics, instead using sealed magnetic reed switches and magnets to take measurements. A voltage must be supplied to each instrument to produce an output.
   - **Anemometer**: The anemometer will be connected to the wind direction meter and is shared with the wind direction meter. A wind speed of 2.4km/h causes the switch to close once per second.  
   - **Wind Vane**: The wind vane’s magnet may close two switches at once, allowing up to 16 different positions to be indicated . An external resistor can be used to form a voltage divider , producing a voltage output that can be measured.
   - **Rain Gauge**:The rain gauge is a self-emptying tipping bucket type. Each 0.2794mm of rain cause one momentary contact closure that can be recorded with a digital counter or microcontroller interrupt input. An RC filter was used to filter out high-frequency noise and debounce from the rain gauge signal.

3. **BME280**:
   - Provides temperature, humidity, and atmospheric pressure readings.

4. **Light Sensors**:
   - **TSL2591**: High-sensitivity lux and light intensity measurements.
   - **VEML6070**: UV-index Measurements.

5. **Soil Sensors**:
   - **DS18B20**: Monitors soil temperature.
   - **Gravity Capacitive Sensor**: Measures soil water content.

## Code Overview:

The code gathers data from all the sensors and then uploads them to a test server used in Json format for logging and further analysis and statistics if needed using POST request.
## WiFi Connectivity:

Before uploading data, the ESP32 must first establish a reliable WiFi connection. This is achieved by providing the WiFi credentials (SSID and password) within the code. The `WiFi.begin(ssid, password)` function is used to initiate the connection. The connection status is monitored with `WiFi.status()` in a loop until a successful connection is made. During this process, the device attempts to reconnect if the connection fails, ensuring that the ESP32 is always connected before any data upload takes place.

- **Error Handling**: If the ESP32 fails to connect to the WiFi network, it retries periodically. The serial monitor provides feedback on the connection status.
- **Considerations**: WiFi connection consumes power, which should be kept in mind during extended operation periods.

## HTTP Communication:

The sensor data is uploaded to a remote server using HTTP POST requests. After the ESP32 connects to the WiFi network, the sensor readings are packaged into a JSON payload, which contains the values of the sensors and their corresponding types. The payload is then sent to the server using the following URL /v2/entities.
Each sensor value is included as a key-value pair in the JSON object, where the key is the sensor's name and the value includes both the sensor reading and its data type.
After sending the data to the server, the code checks if the connection was successful. If the client is unable to connect to the server, it will print a connection failure message to the serial monitor
