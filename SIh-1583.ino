#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <MPU6050.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "Bharath";
const char* password = "Vengeance";
String URL = "http://192.168.43.173/project/data.php";
const char* mqtt_server = "broker.hivemq.com";

#define ONE_WIRE_BUS 15 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WiFiClient espClient;
PubSubClient client(espClient);
MPU6050 mpu;

volatile int pulseCount = 0;  
float rpm = 0.0;              
float voltage = 0.0;
float current = 0.0;
float noise = 0.0;
float temperature = 0.0;
float vibration = 0.0;
float efficiency = 0.0;
unsigned long lastMeasurementTime = 0; 

void setup() {
    Serial.begin(115200);
    connectWiFi();  // Connect to WiFi
    client.setServer(mqtt_server, 1883);
    Wire.begin();
    mpu.initialize(); 
    sensors.begin(); 
    pinMode(33, INPUT_PULLUP); 
    attachInterrupt(digitalPinToInterrupt(33), countPulse, RISING); // Count RPM pulses
}

void loop() {
    if (!client.connected()) {
        reconnect();  // Ensure MQTT connection
    }
    client.loop();

    readSensors();  // Read sensor values
    efficiency = calculateEfficiency(voltage, current); 

    if (millis() - lastMeasurementTime >= 1000) {
        rpm = calculateRPM(); // Calculate RPM every second
        lastMeasurementTime = millis();
    }

    // Send data via HTTP POST
    String postData = "vibration=" + String(vibration) +
                      "&noise=" + String(noise) +
                      "&voltage=" + String(voltage) +
                      "&current=" + String(current) +
                      "&rpm=" + String(rpm) +
                      "&temperature=" + String(temperature) +
                      "&efficiency=" + String(efficiency);
    
    HTTPClient http;
    http.begin(URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(postData);

    if (httpCode > 0) {
        Serial.printf("HTTP Response code: %d\n", httpCode);
    } else {
        Serial.printf("HTTP POST failed: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();

    // Publish sensor data to MQTT
    client.publish("iotfrontier/vibration", String(vibration).c_str());
    client.publish("iotfrontier/noise", String(noise).c_str());
    client.publish("iotfrontier/voltage", String(voltage).c_str());
    client.publish("iotfrontier/current", String(current).c_str());
    client.publish("iotfrontier/rpm", String(rpm).c_str());
    client.publish("iotfrontier/temperature", String(temperature).c_str());
    client.publish("iotfrontier/efficiency", String(efficiency).c_str());

    delay(5000);  // Wait for 5 seconds before the next loop
}

void readSensors() {
    voltage = readVoltage();  
    current = readCurrent();  
    noise = readNoise();      
    temperature = readTemperature(); 
    vibration = readVibration(); 
}

float readVoltage() {
    int sensorValue = analogRead(27); 
    return (sensorValue / 1023.0) * 5.0 * (220.0 / 5.0); 
}

float readCurrent() {
    int sensorValue = analogRead(25); 
    float currentValue = (sensorValue / 1023.0) * 5.0; // Scale based on circuit
    currentValue = (currentValue - 2.5) / 0.185; // Convert to amps
    return currentValue;
}

float readNoise() {
    int noiseValue = analogRead(26); 
    return noiseValue; // Return raw noise value
}

float readTemperature() {
    sensors.requestTemperatures(); 
    return sensors.getTempCByIndex(0); // Get temperature in Celsius
}

float readVibration() {
    Vector rawAcc = mpu.readRawAcceleration();
    return sqrt(pow(rawAcc.XAxis, 2) + pow(rawAcc.YAxis, 2) + pow(rawAcc.ZAxis, 2)); // Calculate vibration
}

float calculateEfficiency(float voltage, float current) {
    float ratedVoltage = 220.0; // V
    float ratedCurrent = 1.5;    // A
    float ratedRPM = 1750.0;      // RPM

    float powerInput = voltage * current; 
    float powerOutput = (ratedVoltage * ratedCurrent * ratedRPM) / 1000.0; 
    return (powerOutput != 0) ? (powerInput / powerOutput) * 100 : 0; 
}

void connectWiFi() {
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

void reconnect() {
    while (!client.connected()) {
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str())) {
            client.subscribe("iotfrontier/mqtt");
        } else {
            delay(5000);
        }
    }
}

void countPulse() {
    pulseCount++; // Increment pulse count for RPM
}

float calculateRPM() {
    float rpmValue = (pulseCount / 1.0) * 60.0; // Convert pulse count to RPM
    pulseCount = 0; // Reset pulse count
    return rpmValue;
}
