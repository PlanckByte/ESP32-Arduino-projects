//Weather Station Test Code
//Definitions, Libraries and Constants
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "Adafruit_VEML6070.h"
#include <Adafruit_BME280.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <esp_http_client.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define DEBOUNCE_TIME 200 //for the tipping bucket
#define PrintInterval 500
#define ANEMOMETER_PIN 32 // Interrupt pin tied to anemometer reed switch
#define WIND_DIRECTION_PIN 34 // Pin for wind direction analog input
#define RainPin 36 // Pin connected to the rain gauge reed switch
#define bucketAmount 0.279 // Rainfall per tip in millimeters
int moistPin4 = 39; // Only one moisture sensor connected to pin 39

// WiFi credentials
const char* ssid = "ATTESTED_3";
const char* password = "u8eb6vhk";
//server 
const char* server = "89.58.4.230";
const int port = 1026;

//I2C initiations
Adafruit_BME280 bme;
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)
Adafruit_VEML6070 uv = Adafruit_VEML6070();

const int oneWireBus = 4;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

//Variables used for calculations
unsigned long lastPrint = 0;
//Anemometer
volatile int anemometerCounter = 0;
int windSpd = 0; // Wind speed
volatile unsigned long lastMicrosAn = 0; //last anemometer event in micros

//timer for printing
unsigned long timer = 0;

//Rain Bucket
float dailyrain = 0; // Total rainfall

bool volatile rainPinStateFlag= false;
unsigned long lastDebounceTimeRain = 0; 
unsigned long currentMillis = 0; // Variable to track bucket position
//Interrupt function for rain
void IRAM_ATTR tipInterrupt() {
    rainPinStateFlag = true;
}

// Function to connect to WiFi
void connectToWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(9600); // Initialize serial monitor communication
    connectToWiFi(); // Connect to WiFi
    
    // Initialize sensors
    bme.begin(0x76);
    tsl.begin();
    tsl.setGain(TSL2591_GAIN_LOW); // Lux sensor gain
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS); // Lux sensor integration time 
    sensors.begin();
    uv.begin(VEML6070_1_T);
    
    // Initialize pins
    pinMode (RainPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(RainPin), tipInterrupt, FALLING);
    pinMode(ANEMOMETER_PIN, INPUT_PULLUP); // Set the wind speed pin as input with pull-up resistor
    attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), countAnemometer, FALLING); // Wind speed interrupt
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Read and display sensor data every second
      if ((currentMillis - lastPrint) >= 360000) {
        lastPrint = currentMillis;
        getAndDisplayData(); // Read and display all sensor data
        readAndPrintWind(); // Read and display wind speed and direction
        readAndPrintRainfall(); // Read and display rainfall data
        connectToServer();
    }
}

void getAndDisplayData() {
    // Read and print air temperature from BME280 sensor
    float temperature = bme.readTemperature();
    Serial.print("Air Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");

    // Read and print soil temperature from DS18B20 sensor
    sensors.requestTemperatures(); // Send the command to get temperatures
    float soilTemperature = sensors.getTempCByIndex(0);
    Serial.print("Soil Temperature: ");
    Serial.print(soilTemperature);
    Serial.println(" *C");

    // Read and print air humidity from BME280
    float humidity = bme.readHumidity();
    Serial.print("Air Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Read and print barometric pressure from BME280
    float pressure = bme.readPressure() / 100.0F; // Convert pressure to hPa
    Serial.print("Barometric Pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");

    // Read and print luminosity from TSL2591
    Serial.println("Luminosity:");
    uint32_t lum = tsl.getFullLuminosity(); //32-bit unsigned integer containing full spectrumw
    uint16_t ir, full; //two 16-bit unsigned integer variables  used to store the IR and full spectrum light data extracted from the 'lum' variable.
    ir = lum >> 16;
    full = lum & 0xFFFF;
    Serial.print("IR: "); Serial.print(ir); Serial.print("  ");
    Serial.print("Full: "); Serial.print(full); Serial.print("  ");
    Serial.print("Visible: "); Serial.print(full - ir); Serial.print("  ");
    Serial.print("Lux: "); Serial.println(2.379 * tsl.calculateLux(full, ir), 3);
    
    // Read and print UV index from VEML6070
    Serial.print("UV: "); Serial.println(uv.readUV());

    // Read soil moisture value from pin 39
    int soilMoistureValue = analogRead(moistPin4);
    Serial.println("Soil Moisture Condition:");
    printSoilMoistureCondition(soilMoistureValue);
}

void printSoilMoistureCondition(int soilMoistureValue) {
    if (soilMoistureValue >= 2350) {
        Serial.println("Soil Moisture: Dry Soil");
    } else if (soilMoistureValue < 1800) {
        Serial.println("Soil Moisture: Very Wet Soil");
    } else if (soilMoistureValue >= 2000 && soilMoistureValue <= 2350) {
        Serial.println("Soil Moisture: Moist Soil");
    } else if (soilMoistureValue >= 1800 && soilMoistureValue <= 2000) {
        Serial.println("Soil Moisture: Damp Soil");
    } else {
        Serial.println("Moisture: Unknown");
    }
}

void readAndPrintWind() {
    // Update and print wind speed
    windSpd = readWindSpd();
    Serial.print("Wind Speed: ");
    Serial.print(windSpd);
    Serial.println(" km/h");

    // Read and print wind direction
    int direction = analogRead(WIND_DIRECTION_PIN);
    printWindDirection(direction);
}

// Function to read and print rainfall data
void readAndPrintRainfall() {
    if  ((millis() - lastDebounceTimeRain) < DEBOUNCE_TIME) {
        rainPinStateFlag = false;
    } else if (rainPinStateFlag == true) {
        Serial.println(rainPinStateFlag);
        dailyrain += bucketAmount; // Increment daily rainfall
        Serial.print("Rainfall incremented: ");
        Serial.print(dailyrain);
        Serial.println(" mm");
        rainPinStateFlag = false;
        lastDebounceTimeRain = millis();
    }
    Serial.print("Daily Rainfall: ");
    Serial.print(dailyrain);
    Serial.println(" mm");
}

// Function to calculate wind speed
int readWindSpd() {
    long spd = 14920 * 1.60934; // One turn = 1.492 miles per hour; modify as necessary for your instrument
    spd *= anemometerCounter;
    spd /= 10000;
    anemometerCounter = 0;
    return (int) spd;
}

// Interrupt service routine to count anemometer tips
void countAnemometer() {
    if ((long)(micros() - lastMicrosAn) >= DEBOUNCE_TIME * 1000) {
        anemometerCounter++;
        lastMicrosAn = micros();
    }
}

void connectToServer() {
    WiFiClient client;
    if (!client.connect(server, port)) {
        Serial.println("Connection to server failed!");
        return;
    }

    // Generate a unique ID for each data point, e.g., using the current time
    String uniqueID = "sensorData_device03_" + String(millis());

    String payload = "{";
    payload += "\"id\":\"" + uniqueID + "\",";
    payload += "\"type\":\"WeatherStation\",";
    payload += "\"temperature\":{\"value\":" + String(bme.readTemperature()) + ",\"type\":\"Number\"},";
    sensors.requestTemperatures();
    payload += "\"soilTemperature\":{\"value\":" + String(sensors.getTempCByIndex(0)) + ",\"type\":\"Number\"},";
    payload += "\"humidity\":{\"value\":" + String(bme.readHumidity()) + ",\"type\":\"Number\"},";
    payload += "\"pressure\":{\"value\":" + String(bme.readPressure() / 100.0F) + ",\"type\":\"Number\"},";
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir = lum >> 16;
    uint16_t full = lum & 0xFFFF;
    float lux = 2.379 * tsl.calculateLux(full, ir);
    payload += "\"lux\":{\"value\":" + String(lux, 3) + ",\"type\":\"Number\"},";
    payload += "\"uv\":{\"value\":" + String(uv.readUV()) + ",\"type\":\"Number\"},";
    payload += "\"windSpeed\":{\"value\":" + String(windSpd) + ",\"type\":\"Number\"},";
    int windDirection = analogRead(WIND_DIRECTION_PIN);
    payload += "\"windDirection\":{\"value\":" + String(windDirection) + ",\"type\":\"Number\"},";
    payload += "\"dailyRain\":{\"value\":" + String(dailyrain) + ",\"type\":\"Number\"},";
    int soilMoistureValue = analogRead(moistPin4);
    payload += "\"soilMoisture\":{\"value\":" + String(soilMoistureValue) + ",\"type\":\"Number\"},";
    payload += "\"timestamp\":{\"value\":\"" + String(millis()) + "\",\"type\":\"Text\"}";
    payload += "}";

    String url = "/v2/entities";
    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + String(payload.length()) + "\r\n" +
                 "Connection: close\r\n\r\n" +
                 payload + "\r\n");

    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            break;
        }
    }
    while (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
    }

    client.stop();
}

// Function to print wind direction based on analog reading
void printWindDirection(int direction) {
    if (direction >= 170 && direction <= 270) {
        Serial.println("Wind Direction: E");
    } else if (direction >= 500 && direction <= 660) {
        Serial.println("Wind Direction: SE");
    } else if (direction >= 900 && direction <= 1100) {
        Serial.println("Wind Direction: S");
    } else if (direction >= 2300 && direction <= 2550) {
        Serial.println("Wind Direction: SW");
    } else if (direction >= 3800 && direction <= 4090) {
        Serial.println("Wind Direction: W");
    } else if (direction >= 3450 && direction <= 3700) {
        Serial.println("Wind Direction: NW");
    } else if (direction >= 2900 && direction <= 3250) {
        Serial.println("Wind Direction: N");
    } else if (direction >= 1640 && direction <= 1850) {
        Serial.println("Wind Direction: NE");
    }
}
