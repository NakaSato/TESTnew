#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <Arduino.h>
#include "WiFiManager.h"
#include "MQTTManager.h"

class DeviceManager {
private:
    WiFiManager* _wifiManager;
    MQTTManager* _mqttManager;
    
    unsigned long _lastDataPublish;
    unsigned long _dataSendInterval;
    
    // LED pins for status indication
    int _wifiLedPin;
    int _mqttLedPin;
    int _dataLedPin;
    
    // Device information
    String _deviceName;
    String _firmwareVersion;
    
public:
    // Constructor
    DeviceManager(
        WiFiManager* wifiManager, 
        MQTTManager* mqttManager,
        const String& deviceName,
        const String& firmwareVersion,
        unsigned long dataSendInterval = 30000
    );
    
    // Initialize device manager
    bool begin();
    
    // Main loop function - call this in the main Arduino loop
    void loop();
    
    // Check connections status (WiFi and MQTT)
    bool checkConnections();
    
    // Send device status information
    void sendStatusInfo();
    
    // Send telemetry data (should be implemented in derived classes)
    virtual void sendTelemetryData();
    
    // Process MQTT commands
    void processCommand(const String& topic, const String& payload);
    
    // Handle device restart
    void restart();
    
    // Set status LED states
    void updateStatusLEDs();
};

#endif // DEVICE_MANAGER_H
