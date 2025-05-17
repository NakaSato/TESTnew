#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

class MQTTManager {
private:
    WiFiClient _wifiClient;
    PubSubClient _client;
    
    String _server;
    int _port;
    String _username;
    String _password;
    String _clientId;
    
    String _topicPrefix;
    String _deviceId;
    
    bool _isConnected;
    unsigned long _lastReconnectAttempt;
    
    // Callback function for incoming messages
    static void defaultCallback(char* topic, byte* payload, unsigned int length);
    
public:
    // Constructor
    MQTTManager(
        const String& server, 
        int port,
        const String& username,
        const String& password,
        const String& clientId,
        const String& topicPrefix,
        const String& deviceId
    );
    
    // Initialize MQTT connection
    bool begin();
    
    // Set callback for incoming messages
    void setCallback(MQTT_CALLBACK_SIGNATURE);
    
    // Check and maintain MQTT connection
    bool checkConnection();
    
    // Publish message to a topic
    bool publish(const String& topic, const String& payload, bool retain = false);
    
    // Publish JSON data to a topic
    bool publishJson(const String& topic, const JsonDocument& jsonDoc, bool retain = false);
    
    // Subscribe to a topic
    bool subscribe(const String& topic);
    
    // Unsubscribe from a topic
    bool unsubscribe(const String& topic);
    
    // Check for new messages
    void loop();
    
    // Get MQTT connection status
    bool isConnected() const;
    
    // Build topic with prefix and device ID
    String buildTopic(const String& topicSuffix) const;
};

#endif // MQTT_MANAGER_H
