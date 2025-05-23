#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#if defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266httpUpdate.h>
#elif defined(ESP32)
    #include <WiFi.h>
    #include <HTTPUpdate.h>
#else
    #error "This library only supports ESP8266 and ESP32 boards"
#endif

// Include required libraries for web server functionality
#include <WebServer.h>
#include <Update.h>

// Maximum number of WiFi networks to store
#define MAX_WIFI_NETWORKS 5

class WiFiManager {
public:
    // Constructor
    WiFiManager(const String& ssid, const String& password, int statusLedPin = -1, 
                unsigned long connectionTimeout = 30000);
    
    // Multi-network constructor
    explicit WiFiManager(int statusLedPin = -1, unsigned long connectionTimeout = 30000);
    
    // Initialize WiFi connection
    bool begin();
    
    // Add a WiFi network to the list
    bool addNetwork(const String& ssid, const String& password);
    
    // Check and maintain WiFi connection
    bool checkConnection();
    
    // Reconnect to WiFi if disconnected
    bool reconnect();
    
    // Try advanced connection methods when standard approach fails
    bool tryAdvancedConnection();
    
    // Connection status
    bool isConnected() const;
    
    // Get WiFi signal strength
    int getSignalStrength() const;
    
    // Get IP address as string
    String getIPAddress() const;
    
    // Get MAC address as string
    String getMACAddress() const;
    
    // Print WiFi status to Serial
    void printStatus() const;
    
    // Scan for available WiFi networks
    void scanNetworks();
    
    // OTA firmware update
    bool updateFirmware(const String& firmwareUrl, const String& currentVersion = "");
    
    // Start web server for file uploads
    void beginUploadServer(int port = 80);
    
    // Handle server tasks (call in loop)
    void handleUploadServer();
    
    // Start telnet-style monitoring
    void beginRemoteMonitor(int port = 23);
    
    // Handle monitoring tasks (call in loop)
    void handleRemoteMonitor();
    
    // Write to remote monitor
    void remoteLog(const String& message);
    
    // Print WiFi connection status
    void printConnectionStatus(wl_status_t status);
    
private:
    // WiFi credentials for multiple networks
    struct WiFiNetwork {
        String ssid;
        String password;
        bool active;
    };
    
    WiFiNetwork _networks[MAX_WIFI_NETWORKS];
    int _networkCount;
    int _currentNetworkIndex;
    
    // Legacy support for single network
    String _ssid;
    String _password;
    
    int _statusLedPin;
    unsigned long _connectionTimeout;
    unsigned long _lastReconnectAttempt;
    bool _isConnected;
    bool _isLegacyMode;
    
    // Web server for OTA uploads
    WebServer* _uploadServer = nullptr;
    int _uploadServerPort = 80;
    bool _uploadServerActive = false;
    
    // Telnet-style monitoring
    WiFiServer* _monitorServer = nullptr;
    WiFiClient _monitorClient;
    int _monitorPort = 23;
    bool _monitorActive = false;
    
    // Try to connect to a specific network
    bool tryConnect(const String& ssid, const String& password);
    
    // Upload server pages
    void setupUploadServer();
    void handleUploadRoot();
    void handleFileUpload();
    void handleUploadComplete();
};

#endif // WIFI_MANAGER_H
