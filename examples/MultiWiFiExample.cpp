#include <Arduino.h>
#include "WiFiManager.h"

// Status LED pin
const int LED_PIN = 2;  // Use appropriate LED pin for your board

// Create a WiFi manager instance with multi-network support
WiFiManager wifiManager(LED_PIN, 30000);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n===== Multi-WiFi Network Example =====");
  
  // Add multiple WiFi networks (priority order - first added = highest priority)
  wifiManager.addNetwork("PrimaryNetwork", "password1");
  wifiManager.addNetwork("BackupNetwork", "password2");
  wifiManager.addNetwork("WorkNetwork", "password3");
  
  // Scan for available networks
  wifiManager.scanNetworks();
  
  // Try to connect (will try networks in the order they were added)
  if (wifiManager.begin()) {
    Serial.println("Connected to WiFi successfully!");
  } else {
    Serial.println("Failed to connect to any WiFi network!");
  }
}

void loop() {
  // Periodically check WiFi connection
  if (!wifiManager.checkConnection()) {
    Serial.println("WiFi connection lost!");
    
    // Attempt to reconnect (will try all networks)
    if (wifiManager.reconnect()) {
      Serial.println("Reconnected to WiFi!");
    } else {
      Serial.println("Failed to reconnect to any WiFi network!");
    }
  }
  
  delay(10000);  // Check connection every 10 seconds
}
