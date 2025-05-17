#include <Arduino.h>
#include "WiFiManager.h"

// Define firmware version
#define FIRMWARE_VERSION "1.0.0"

// Status LED pin
const int LED_PIN = 2;  // Use appropriate LED pin for your board

// Create WiFi manager
WiFiManager wifiManager("YourSSID", "YourPassword", LED_PIN);

// Sample sensor data
float temperature = 0.0;
float humidity = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n===== Upload & Monitor Example =====");
  Serial.print("Firmware version: ");
  Serial.println(FIRMWARE_VERSION);
  
  // Connect to WiFi
  if (wifiManager.begin()) {
    Serial.println("Connected to WiFi successfully!");
    
    // Start web server for OTA updates
    wifiManager.beginUploadServer(80);
    
    // Start remote monitoring server
    wifiManager.beginRemoteMonitor(23);
    
    Serial.println("OTA upload and remote monitoring services started");
  } else {
    Serial.println("Failed to connect to WiFi!");
  }
}

void loop() {
  // Check and maintain WiFi connection
  wifiManager.checkConnection();
  
  // Handle web server requests
  wifiManager.handleUploadServer();
  
  // Handle remote monitor
  wifiManager.handleRemoteMonitor();
  
  // Simulate sensor readings
  static unsigned long lastSensorUpdate = 0;
  if (millis() - lastSensorUpdate > 5000) {
    lastSensorUpdate = millis();
    
    // Simulate changing sensor values
    temperature = 20.0 + (random(100) / 10.0);
    humidity = 40.0 + (random(300) / 10.0);
    
    // Log to serial
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    
    // Log to remote monitor if connected
    String sensorData = "Sensor reading: Temp=" + String(temperature) + 
                        "°C, Humidity=" + String(humidity) + "%";
    wifiManager.remoteLog(sensorData);
  }
  
  delay(10); // Small delay to prevent watchdog resets
}
