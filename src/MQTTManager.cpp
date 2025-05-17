#include "MQTTManager.h"

// Default callback function for incoming messages
void MQTTManager::defaultCallback(char* topic, byte* payload, unsigned int length) {
    Serial.println("Message received on topic: " + String(topic));
    Serial.print("Payload: ");
    
    // Print the message content
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

// Constructor
MQTTManager::MQTTManager(
    const String& server, 
    int port,
    const String& username,
    const String& password,
    const String& clientId,
    const String& topicPrefix,
    const String& deviceId
) : 
    _server(server),
    _port(port),
    _username(username),
    _password(password),
    _clientId(clientId),
    _topicPrefix(topicPrefix),
    _deviceId(deviceId),
    _isConnected(false),
    _lastReconnectAttempt(0),
    _client(_wifiClient)
{
    // Set default callback
    _client.setCallback(defaultCallback);
}

// Initialize MQTT connection
bool MQTTManager::begin() {
    Serial.print("Connecting to MQTT broker at ");
    Serial.print(_server);
    Serial.print(":");
    Serial.println(_port);
    
    // Set server and port
    _client.setServer(_server.c_str(), _port);
    
    // Connect with credentials
    bool success = _client.connect(
        _clientId.c_str(),
        _username.c_str(),
        _password.c_str()
    );
    
    if (success) {
        _isConnected = true;
        Serial.println("MQTT connection successful");
        
        // Subscribe to device-specific control topic
        String controlTopic = buildTopic("control/#");
        _client.subscribe(controlTopic.c_str());
        Serial.println("Subscribed to: " + controlTopic);
        
        // Publish connection status
        String statusTopic = buildTopic("status");
        _client.publish(statusTopic.c_str(), "online", true);
        Serial.println("Published online status to: " + statusTopic);
        
        return true;
    } else {
        _isConnected = false;
        Serial.print("MQTT connection failed with error code: ");
        Serial.println(_client.state());
        return false;
    }
}

// Set callback for incoming messages
void MQTTManager::setCallback(MQTT_CALLBACK_SIGNATURE) {
    _client.setCallback(callback);
}

// Check and maintain MQTT connection
bool MQTTManager::checkConnection() {
    if (!_client.connected()) {
        _isConnected = false;
        
        // Try to reconnect if enough time has passed since last attempt
        unsigned long currentTime = millis();
        if (currentTime - _lastReconnectAttempt > 5000) {
            _lastReconnectAttempt = currentTime;
            Serial.println("MQTT disconnected. Attempting to reconnect...");
            
            if (begin()) {
                _isConnected = true;
                return true;
            }
        }
        return false;
    }
    
    _isConnected = true;
    return true;
}

// Publish message to a topic
bool MQTTManager::publish(const String& topic, const String& payload, bool retain) {
    if (!_isConnected || !checkConnection()) {
        return false;
    }
    
    String fullTopic = buildTopic(topic);
    return _client.publish(fullTopic.c_str(), payload.c_str(), retain);
}

// Publish JSON data to a topic
bool MQTTManager::publishJson(const String& topic, const JsonDocument& jsonDoc, bool retain) {
    if (!_isConnected || !checkConnection()) {
        return false;
    }
    
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    
    String fullTopic = buildTopic(topic);
    return _client.publish(fullTopic.c_str(), jsonString.c_str(), retain);
}

// Subscribe to a topic
bool MQTTManager::subscribe(const String& topic) {
    if (!_isConnected || !checkConnection()) {
        return false;
    }
    
    String fullTopic = buildTopic(topic);
    return _client.subscribe(fullTopic.c_str());
}

// Unsubscribe from a topic
bool MQTTManager::unsubscribe(const String& topic) {
    if (!_isConnected || !checkConnection()) {
        return false;
    }
    
    String fullTopic = buildTopic(topic);
    return _client.unsubscribe(fullTopic.c_str());
}

// Check for new messages
void MQTTManager::loop() {
    if (_isConnected) {
        _client.loop();
    }
}

// Get MQTT connection status
bool MQTTManager::isConnected() const {
    return _isConnected;
}

// Build topic with prefix and device ID
String MQTTManager::buildTopic(const String& topicSuffix) const {
    return _topicPrefix + _deviceId + "/" + topicSuffix;
}
