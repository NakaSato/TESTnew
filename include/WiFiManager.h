#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
private:
    String _ssid;
    String _password;
    unsigned long _connectionTimeout;
    unsigned long _lastReconnectAttempt;
    bool _isConnected;
    
    // Status LED pins
    int _statusLedPin;
    
    // Try advanced connection methods
    bool tryAdvancedConnection();
    
public:
    // Constructor
    WiFiManager(const String& ssid, const String& password, int statusLedPin = -1, unsigned long connectionTimeout = 10000);
    
    // Initialize WiFi connection
    bool begin();
    
    // Check and maintain WiFi connection
    bool checkConnection();
    
    // Reconnect to WiFi if disconnected
    bool reconnect();
    
    // Get connection status
    bool isConnected() const;
    
    // Get WiFi signal strength
    int getSignalStrength() const;
    
    // Get IP address as string
    String getIPAddress() const;
    
    // Get MAC address as string
    String getMACAddress() const;
    
    // Print WiFi connection status code
    void printConnectionStatus(wl_status_t status);
    
    // Print WiFi status information to Serial
    void printStatus() const;
    
    // Scan for available WiFi networks and print results
    void scanNetworks();
};

#endif // WIFI_MANAGER_H
