#include "HttpServer.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "Config.h"

// Constructor
HttpServer::HttpServer(WiFiManager* wifiManager, int port) : 
    _wifiManager(wifiManager),
    _port(port),
    _authEnabled(false) {
    _server = new WebServer(port);
}

// Destructor
HttpServer::~HttpServer() {
    stop();
    if (_server) {
        delete _server;
        _server = nullptr;
    }
}

// Initialize and start server
void HttpServer::begin() {
    if (!_wifiManager->isConnected()) {
        Serial.println("HTTP Server: Cannot start, WiFi not connected");
        return;
    }

    // Setup default routes
    setupDefaultRoutes();
    
    // Start mDNS responder if defined in config
    #ifdef DEVICE_HOSTNAME
    if (MDNS.begin(DEVICE_HOSTNAME)) {
        Serial.print("HTTP Server: mDNS responder started at http://");
        Serial.print(DEVICE_HOSTNAME);
        Serial.println(".local");
        
        // Advertise HTTP service
        MDNS.addService("http", "tcp", _port);
    }
    #endif
    
    // Start server
    _server->begin();
    Serial.print("HTTP Server: Started on port ");
    Serial.println(_port);
    Serial.print("HTTP Server: IP address: ");
    Serial.println(_wifiManager->getIPAddress());
}

// Process client requests (call in loop)
void HttpServer::handleClient() {
    if (_server) {
        _server->handleClient();
    }
}

// Stop server
void HttpServer::stop() {
    if (_server) {
        _server->close();
        Serial.println("HTTP Server: Stopped");
    }
}

// Add custom routes/handlers
void HttpServer::on(const String& uri, WebServer::THandlerFunction handler) {
    if (_server) {
        _server->on(uri, handler);
    }
}

// Add custom routes/handlers with HTTP method
void HttpServer::on(const String& uri, HTTPMethod method, WebServer::THandlerFunction handler) {
    if (_server) {
        _server->on(uri, method, handler);
    }
}

// Set basic authentication
void HttpServer::setAuthentication(const String& username, const String& password) {
    _username = username;
    _password = password;
    _authEnabled = true;
    Serial.println("HTTP Server: Basic authentication enabled");
}

// Check if server is running
bool HttpServer::isRunning() const {
    return (_server != nullptr && _wifiManager->isConnected());
}

// Authenticate request (basic auth)
bool HttpServer::authenticateRequest() {
    if (!_authEnabled) {
        return true;
    }
    
    if (!_server->authenticate(_username.c_str(), _password.c_str())) {
        _server->requestAuthentication();
        return false;
    }
    
    return true;
}

// Default route handlers
void HttpServer::setupDefaultRoutes() {
    // Root handler
    _server->on("/", HTTP_GET, [this]() {
        this->handleRoot();
    });
    
    // Status endpoint
    _server->on("/status", HTTP_GET, [this]() {
        this->handleStatus();
    });
    
    // Network info endpoint (useful for ngrok and remote access)
    _server->on("/network", HTTP_GET, [this]() {
        this->handleNetworkInfo();
    });
    
    // 404 handler
    _server->onNotFound([this]() {
        this->handleNotFound();
    });
}

void HttpServer::handleRoot() {
    if (!authenticateRequest()) return;
    
    String html = "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>" DEVICE_ID " Web Interface</title>"
        "<style>"
        "body {font-family: Arial, sans-serif; margin: 0; padding: 20px; color: #333;}"
        "h1 {color: #0066cc;}"
        ".card {background: #f9f9f9; border-radius: 5px; padding: 15px; margin-bottom: 20px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);}"
        "button {background: #0066cc; color: white; border: none; padding: 10px 15px; border-radius: 4px; cursor: pointer; margin-right: 10px; margin-bottom: 10px;}"
        "button:hover {background: #0055aa;}"
        ".status-ok {color: green;}"
        ".status-error {color: red;}"
        "</style>"
        "</head>"
        "<body>"
        "<h1>" DEVICE_ID " Web Interface</h1>"
        "<div class='card'>"
        "<h2>Device Status</h2>"
        "<p><strong>WiFi SSID:</strong> " + String(WIFI_SSID) + "</p>"
        "<p><strong>IP Address:</strong> " + _wifiManager->getIPAddress() + "</p>"
        "<p><strong>MAC Address:</strong> " + _wifiManager->getMACAddress() + "</p>"
        "<p><strong>Signal Strength:</strong> " + String(_wifiManager->getSignalStrength()) + " dBm</p>"
        "<p><strong>Uptime:</strong> " + String(millis() / 1000) + " seconds</p>"
        "</div>"
        "<div class='card'>"
        "<h2>Actions</h2>"
        "<p>"
        "<button onclick='window.location.href=\"/led\"'>LED Control</button>"
        "<button onclick='window.location.href=\"/status\"'>View JSON Status</button>"
        "<button onclick='window.location.href=\"/settings\"'>Settings</button>"
        "<button onclick='window.location.href=\"/system\"'>System Info</button>"
        "<button onclick='window.location.href=\"/network\"'>Network Info</button>"
        "</p>"
        "</div>"
        "<script>"
        "setTimeout(function() { location.reload(); }, 30000);"
        "</script>"
        "</body>"
        "</html>";
    
    _server->send(200, "text/html", html);
}

void HttpServer::handleStatus() {
    if (!authenticateRequest()) return;
    
    // Create JSON document using the newer recommended method
    JsonDocument doc;
    
    doc["device"] = DEVICE_ID;
    doc["uptime"] = millis() / 1000;
    
    // Create nested objects using the newer recommended method
    JsonObject wifi = doc["wifi"].to<JsonObject>();
    wifi["connected"] = _wifiManager->isConnected();
    wifi["ssid"] = WIFI_SSID;
    wifi["ip"] = _wifiManager->getIPAddress();
    wifi["mac"] = _wifiManager->getMACAddress();
    wifi["rssi"] = _wifiManager->getSignalStrength();
    
    #ifdef MQTT_SERVER
    JsonObject mqtt = doc["mqtt"].to<JsonObject>();
    mqtt["broker"] = MQTT_SERVER;
    mqtt["port"] = MQTT_PORT;
    mqtt["clientId"] = CLIENT_ID;
    #endif
    
    String response;
    serializeJsonPretty(doc, response);
    
    _server->send(200, "application/json", response);
}

void HttpServer::handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += _server->uri();
    message += "\nMethod: ";
    message += (_server->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += _server->args();
    message += "\n";
    
    for (uint8_t i = 0; i < _server->args(); i++) {
        message += " " + _server->argName(i) + ": " + _server->arg(i) + "\n";
    }
    
    _server->send(404, "text/plain", message);
}

// Network info handler (useful for ngrok setup)
void HttpServer::handleNetworkInfo() {
    if (!authenticateRequest()) return;
    
    // Create JSON document
    JsonDocument doc;
    
    // Add network information
    doc["device_id"] = DEVICE_ID;
    doc["hostname"] = DEVICE_HOSTNAME;
    doc["local_ip"] = _wifiManager->getIPAddress();
    doc["mac_address"] = _wifiManager->getMACAddress();
    doc["rssi"] = _wifiManager->getSignalStrength();
    doc["port"] = HTTP_SERVER_PORT;
    doc["mdns"] = String(DEVICE_HOSTNAME) + ".local";
    
    // Add HTTP headers the request came in with (useful for debugging ngrok)
    JsonObject headers = doc["request_headers"].to<JsonObject>();
    for (int i = 0; i < _server->headers(); i++) {
        headers[_server->headerName(i)] = _server->header(i);
    }
    
    // Serialize to JSON
    String response;
    serializeJsonPretty(doc, response);
    
    // Send the response
    _server->send(200, "application/json", response);
    
    // Also log to serial
    Serial.println("Network info requested. IP: " + _wifiManager->getIPAddress());
}
