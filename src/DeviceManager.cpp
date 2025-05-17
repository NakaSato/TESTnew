#include "DeviceManager.h"

// Constructor
DeviceManager::DeviceManager(
    WiFiManager* wifiManager, 
    MQTTManager* mqttManager,
    const String& deviceName,
    const String& firmwareVersion,
    unsigned long dataSendInterval
) : 
    _wifiManager(wifiManager),
    _mqttManager(mqttManager),
    _deviceName(deviceName),
    _firmwareVersion(firmwareVersion),
    _dataSendInterval(dataSendInterval),
    _lastDataPublish(0),
    _wifiLedPin(-1),
    _mqttLedPin(-1),
    _dataLedPin(-1)
{
}

// Initialize device manager
bool DeviceManager::begin() {
    Serial.println("Initializing device manager...");
    
    // Connect to WiFi
    bool wifiConnected = _wifiManager->begin();
    if (!wifiConnected) {
        Serial.println("WiFi connection failed. Continuing with limited functionality.");
    }
    
    // Connect to MQTT if WiFi is connected
    bool mqttConnected = false;
    if (wifiConnected) {
        mqttConnected = _mqttManager->begin();
        if (!mqttConnected) {
            Serial.println("MQTT connection failed. Continuing with limited functionality.");
        }
    }
    
    // Send initial status
    if (mqttConnected) {
        sendStatusInfo();
    }
    
    return wifiConnected && mqttConnected;
}

// Main loop function
void DeviceManager::loop() {
    // Check connections
    checkConnections();
    
    // Update status LEDs
    updateStatusLEDs();
    
    // Process MQTT messages
    if (_mqttManager->isConnected()) {
        _mqttManager->loop();
        
        // Send telemetry data at intervals
        unsigned long currentMillis = millis();
        if (currentMillis - _lastDataPublish >= _dataSendInterval) {
            _lastDataPublish = currentMillis;
            sendTelemetryData();
        }
    }
}

// Check connections status
bool DeviceManager::checkConnections() {
    // Check WiFi connection
    bool wifiConnected = _wifiManager->checkConnection();
    
    // Check MQTT connection if WiFi is connected
    bool mqttConnected = false;
    if (wifiConnected) {
        mqttConnected = _mqttManager->checkConnection();
    }
    
    return wifiConnected && mqttConnected;
}

// Send device status information
void DeviceManager::sendStatusInfo() {
    if (!_mqttManager->isConnected()) {
        return;
    }
    
    // Create JSON document for device status
    JsonDocument statusDoc;
    
    statusDoc["device"] = _deviceName;
    statusDoc["firmware"] = _firmwareVersion;
    statusDoc["ip"] = _wifiManager->getIPAddress();
    statusDoc["mac"] = _wifiManager->getMACAddress();
    statusDoc["rssi"] = _wifiManager->getSignalStrength();
    statusDoc["uptime"] = millis() / 1000; // Uptime in seconds
    statusDoc["heap"] = ESP.getFreeHeap();
    
    // Publish status information
    _mqttManager->publishJson("status/info", statusDoc, true);
    
    Serial.println("Device status information sent");
}

// Send telemetry data - base implementation
void DeviceManager::sendTelemetryData() {
    // Base implementation just sends a heartbeat
    if (!_mqttManager->isConnected()) {
        return;
    }
    
    JsonDocument telemetryDoc;
    telemetryDoc["timestamp"] = millis() / 1000;
    telemetryDoc["heap"] = ESP.getFreeHeap();
    
    _mqttManager->publishJson("telemetry/heartbeat", telemetryDoc, false);
    
    Serial.println("Heartbeat telemetry sent");
}

// Process MQTT commands
void DeviceManager::processCommand(const String& topic, const String& payload) {
    Serial.println("Command received: " + topic + " - " + payload);
    
    // Handle common commands
    if (topic.endsWith("/restart")) {
        Serial.println("Restart command received");
        restart();
    } 
    else if (topic.endsWith("/status/request")) {
        Serial.println("Status request received");
        sendStatusInfo();
    }
    
    // Additional command processing can be implemented in derived classes
}

// Handle device restart
void DeviceManager::restart() {
    Serial.println("Restarting device...");
    
    // Send offline status if connected
    if (_mqttManager->isConnected()) {
        _mqttManager->publish("status", "offline", true);
        delay(100); // Short delay to allow message to be sent
    }
    
    // Restart the ESP32
    ESP.restart();
}

// Set status LED states
void DeviceManager::updateStatusLEDs() {
    // Update WiFi status LED if defined
    if (_wifiLedPin >= 0) {
        digitalWrite(_wifiLedPin, _wifiManager->isConnected() ? HIGH : LOW);
    }
    
    // Update MQTT status LED if defined
    if (_mqttLedPin >= 0) {
        digitalWrite(_mqttLedPin, _mqttManager->isConnected() ? HIGH : LOW);
    }
}
