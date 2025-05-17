#include "WiFiManager.h"

// Constructor
WiFiManager::WiFiManager(const String& ssid, const String& password, int statusLedPin, unsigned long connectionTimeout) : 
    _ssid(ssid), 
    _password(password), 
    _statusLedPin(statusLedPin),
    _connectionTimeout(connectionTimeout),
    _lastReconnectAttempt(0),
    _isConnected(false) {
    
    // Initialize status LED if specified
    if (_statusLedPin >= 0) {
        pinMode(_statusLedPin, OUTPUT);
        digitalWrite(_statusLedPin, LOW);
    }
}

// Initialize WiFi connection
bool WiFiManager::begin() {
    Serial.print("Connecting to WiFi network: ");
    Serial.println(_ssid);
    
    // Set WiFi mode to station (client) and reset WiFi
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true, true);  // Disconnect and clear credentials
    delay(1000);  // Increased delay to ensure WiFi hardware reset
    
    // Start connection process
    Serial.println("Starting WiFi connection attempt...");
    WiFi.begin(_ssid.c_str(), _password.c_str());
    
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    
    // Wait for connection with timeout
    unsigned long startTime = millis();
    int attemptCount = 0;
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < _connectionTimeout)) {
        // Blink status LED if available
        if (_statusLedPin >= 0) {
            digitalWrite(_statusLedPin, !digitalRead(_statusLedPin));
        }
        
        delay(500);
        Serial.print(".");
        
        // Every 5 seconds, print WiFi status and attempt reconnection
        if ((millis() - startTime) % 5000 < 500) {
            wl_status_t currentStatus = WiFi.status();
            printConnectionStatus(currentStatus);
            
            // If stuck in IDLE or DISCONNECTED for too long, retry connection
            if ((currentStatus == WL_IDLE_STATUS || currentStatus == WL_DISCONNECTED) && ++attemptCount >= 3) {
                Serial.println("\nRetrying WiFi connection...");
                WiFi.disconnect();
                delay(500);
                WiFi.begin(_ssid.c_str(), _password.c_str());
                attemptCount = 0;
            }
        }
    }
    
    // Check if connection was successful
    if (WiFi.status() == WL_CONNECTED) {
        _isConnected = true;
        
        // Turn on status LED if available
        if (_statusLedPin >= 0) {
            digitalWrite(_statusLedPin, HIGH);
        }
        
        Serial.println("\nWiFi connected successfully!");
        printStatus();
        return true;
    } else {
        _isConnected = false;
        
        // Turn off status LED if available
        if (_statusLedPin >= 0) {
            digitalWrite(_statusLedPin, LOW);
        }
        
        Serial.println("\nStandard WiFi connection failed! Trying advanced methods...");
        // Try the advanced connection methods
        return tryAdvancedConnection();
    }
}

// Check and maintain WiFi connection
bool WiFiManager::checkConnection() {
    wl_status_t currentStatus = WiFi.status();
    
    if (currentStatus != WL_CONNECTED) {
        _isConnected = false;
        
        // Turn off status LED if available
        if (_statusLedPin >= 0) {
            digitalWrite(_statusLedPin, LOW);
        }
        
        // Try to reconnect if enough time has passed since last attempt
        unsigned long currentTime = millis();
        if (currentTime - _lastReconnectAttempt > 10000) { // Reduced from 30000 to 10000
            _lastReconnectAttempt = currentTime;
            Serial.print("WiFi disconnected (status: ");
            printConnectionStatus(currentStatus);
            Serial.println("). Attempting to reconnect...");
            return reconnect();
        }
        return false;
    }
    
    _isConnected = true;
    return true;
}

// Reconnect to WiFi if disconnected
bool WiFiManager::reconnect() {
    Serial.println("Attempting WiFi reconnection with power cycle...");
    
    // Power cycle the WiFi hardware
    WiFi.disconnect(true, true);  // Disconnect and clear stored credentials
    delay(1000);
    
    // Turn WiFi back on
    WiFi.mode(WIFI_STA);
    delay(500);
    
    // Try the first connection method
    Serial.println("Reconnection attempt 1: Standard method");
    WiFi.begin(_ssid.c_str(), _password.c_str());
    
    // Wait for connection with a short timeout
    unsigned long startTime = millis();
    unsigned short retryTimeout = 10000; // 10 seconds
    
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < retryTimeout)) {
        if (_statusLedPin >= 0) {
            digitalWrite(_statusLedPin, !digitalRead(_statusLedPin));
        }
        delay(500);
        Serial.print(".");
    }
    
    // If connected, return success
    if (WiFi.status() == WL_CONNECTED) {
        _isConnected = true;
        if (_statusLedPin >= 0) {
            digitalWrite(_statusLedPin, HIGH);
        }
        Serial.println("\nWiFi reconnected successfully!");
        printStatus();
        return true;
    }
    
    // Try alternative connection method
    Serial.println("\nStandard reconnection failed. Trying alternative method...");
    WiFi.disconnect();
    delay(1000);
    
    // Force a specific channel (1) and try again
    WiFi.begin(_ssid.c_str(), _password.c_str(), 1);
    
    startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < retryTimeout)) {
        if (_statusLedPin >= 0) {
            digitalWrite(_statusLedPin, !digitalRead(_statusLedPin));
        }
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        _isConnected = true;
        if (_statusLedPin >= 0) {
            digitalWrite(_statusLedPin, HIGH);
        }
        Serial.println("\nWiFi reconnected with alternative method!");
        printStatus();
        return true;
    }
    
    Serial.println("\nAll reconnection attempts failed!");
    return false;
}

// Try advanced connection methods when standard approach fails
bool WiFiManager::tryAdvancedConnection() {
    Serial.println("\nTrying advanced WiFi connection methods...");
    
    // Method 1: Try with a fixed channel (auto-select)
    WiFi.disconnect(true);
    delay(1000);
    Serial.println("Method 1: Using automatic channel selection with lower TX power");
    WiFi.setTxPower(WIFI_POWER_17dBm); // Lower TX power for more stable connection
    WiFi.begin(_ssid.c_str(), _password.c_str());
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < 10000)) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        _isConnected = true;
        Serial.println("\nMethod 1 successful!");
        printStatus();
        return true;
    }
    
    // Method 2: Try connecting with a static IP (often helps bypass DHCP issues)
    Serial.println("\nMethod 2: Using static IP address");
    WiFi.disconnect();
    delay(1000);
    
    // Set static IP configuration (use safe default values)
    IPAddress staticIP(192, 168, 1, 200);  // Static IP
    IPAddress gateway(192, 168, 1, 1);     // Gateway
    IPAddress subnet(255, 255, 255, 0);    // Subnet mask
    IPAddress dns(8, 8, 8, 8);             // DNS (Google)
    
    if (WiFi.config(staticIP, gateway, subnet, dns)) {
        Serial.println("Static IP configuration set");
    } else {
        Serial.println("Failed to set static IP configuration");
    }
    
    WiFi.begin(_ssid.c_str(), _password.c_str());
    
    startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < 10000)) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        _isConnected = true;
        Serial.println("\nMethod 2 successful!");
        printStatus();
        return true;
    }
    
    // Method 3: Try specific channels - start with channel from Config.h
    #ifdef WIFI_CHANNEL
    Serial.println("\nMethod 3: Using specific WiFi channel " + String(WIFI_CHANNEL));
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(_ssid.c_str(), _password.c_str(), WIFI_CHANNEL);
    
    startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < 10000)) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        _isConnected = true;
        Serial.println("\nMethod 3 successful with channel " + String(WIFI_CHANNEL) + "!");
        printStatus();
        return true;
    }
    #endif
    
    // Try channels 1, 6, and 11 (standard non-overlapping channels)
    int standardChannels[] = {1, 6, 11};
    for (int i = 0; i < 3; i++) {
        Serial.println("\nMethod 4: Trying standard channel " + String(standardChannels[i]));
        WiFi.disconnect();
        delay(1000);
        WiFi.begin(_ssid.c_str(), _password.c_str(), standardChannels[i]);
        
        startTime = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - startTime < 8000)) {
            delay(500);
            Serial.print(".");
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            _isConnected = true;
            Serial.println("\nMethod 4 successful with channel " + String(standardChannels[i]) + "!");
            printStatus();
            return true;
        }
    }
    
    // If all else fails, try one last attempt with default settings
    Serial.println("\nMethod 5: Final attempt with default settings");
    WiFi.disconnect(true);
    delay(1000);
    WiFi.mode(WIFI_STA);
    delay(1000);
    WiFi.begin(_ssid.c_str(), _password.c_str());
    
    startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < 15000)) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        _isConnected = true;
        Serial.println("\nMethod 5 successful!");
        printStatus();
        return true;
    }
    
    Serial.println("\nAll connection methods failed!");
    return false;
}

// Get connection status
bool WiFiManager::isConnected() const {
    return _isConnected;
}

// Get WiFi signal strength
int WiFiManager::getSignalStrength() const {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.RSSI();
    }
    return 0; // Return 0 if not connected
}

// Get IP address as string
String WiFiManager::getIPAddress() const {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.localIP().toString();
    }
    return "Not connected";
}

// Get MAC address as string
String WiFiManager::getMACAddress() const {
    return WiFi.macAddress();
}

// Print WiFi connection status code
void WiFiManager::printConnectionStatus(wl_status_t status) {
    Serial.print("\nWiFi status: ");
    switch (status) {
        case WL_CONNECTED:
            Serial.println("Connected");
            break;
        case WL_NO_SHIELD:
            Serial.println("No shield");
            break;
        case WL_IDLE_STATUS:
            Serial.println("Idle");
            break;
        case WL_NO_SSID_AVAIL:
            Serial.println("No SSID available");
            break;
        case WL_SCAN_COMPLETED:
            Serial.println("Scan completed");
            break;
        case WL_CONNECT_FAILED:
            Serial.println("Connection failed");
            break;
        case WL_CONNECTION_LOST:
            Serial.println("Connection lost");
            break;
        case WL_DISCONNECTED:
            Serial.println("Disconnected");
            break;
        default:
            Serial.println("Unknown");
    }
}

// Print WiFi status information to Serial
void WiFiManager::printStatus() const {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("=== WiFi Status ===");
        Serial.print("SSID: ");
        Serial.println(_ssid);
        Serial.print("IP Address: ");
        Serial.println(getIPAddress());
        Serial.print("MAC Address: ");
        Serial.println(getMACAddress());
        Serial.print("Signal Strength (RSSI): ");
        Serial.print(getSignalStrength());
        Serial.println(" dBm");
        Serial.println("==================");
    } else {
        Serial.println("WiFi not connected");
    }
}

// Scan for available WiFi networks and print results
void WiFiManager::scanNetworks() {
    Serial.println("Scanning for WiFi networks...");
    
    // Set WiFi to station mode and disconnect from any AP
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    int numNetworks = WiFi.scanNetworks();
    
    if (numNetworks == 0) {
        Serial.println("No WiFi networks found!");
    } else {
        Serial.print("Found ");
        Serial.print(numNetworks);
        Serial.println(" networks:");
        
        // Print network details
        for (int i = 0; i < numNetworks; i++) {
            // Print SSID, RSSI and encryption type
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(" dBm) ");
            
            // Print encryption type
            switch (WiFi.encryptionType(i)) {
                case WIFI_AUTH_OPEN:
                    Serial.println("[Open]");
                    break;
                case WIFI_AUTH_WEP:
                    Serial.println("[WEP]");
                    break;
                case WIFI_AUTH_WPA_PSK:
                    Serial.println("[WPA-PSK]");
                    break;
                case WIFI_AUTH_WPA2_PSK:
                    Serial.println("[WPA2-PSK]");
                    break;
                case WIFI_AUTH_WPA_WPA2_PSK:
                    Serial.println("[WPA/WPA2-PSK]");
                    break;
                default:
                    Serial.println("[Unknown]");
            }
            
            delay(10);
        }
        
        // Check if our target network is visible
        bool targetNetworkFound = false;
        for (int i = 0; i < numNetworks; i++) {
            if (WiFi.SSID(i) == _ssid) {
                targetNetworkFound = true;
                Serial.print("Target network '");
                Serial.print(_ssid);
                Serial.print("' found with signal strength: ");
                Serial.print(WiFi.RSSI(i));
                Serial.println(" dBm");
                break;
            }
        }
        
        if (!targetNetworkFound) {
            Serial.print("TARGET NETWORK '");
            Serial.print(_ssid);
            Serial.println("' NOT FOUND! Please check SSID spelling or if network is in range.");
        }
    }
    
    // Delete scan result to free memory
    WiFi.scanDelete();
    Serial.println("Network scan complete.");
}
