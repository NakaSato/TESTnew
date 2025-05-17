#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <functional>
#include "WiFiManager.h"

class HttpServer {
private:
    WebServer* _server;
    WiFiManager* _wifiManager;
    int _port;
    
    // Default handlers
    void handleRoot();
    void handleNotFound();
    void handleStatus();
    void handleNetworkInfo();
    void setupDefaultRoutes();
    
    // Security (optional for basic auth)
    String _username;
    String _password;
    bool _authEnabled;
    bool authenticateRequest();

public:
    // Constructor
    HttpServer(WiFiManager* wifiManager, int port = 80);
    
    // Destructor
    ~HttpServer();
    
    // Initialize and start server
    void begin();
    
    // Process client requests (call in loop)
    void handleClient();
    
    // Stop server
    void stop();
    
    // Add custom routes/handlers
    void on(const String& uri, WebServer::THandlerFunction handler);
    void on(const String& uri, HTTPMethod method, WebServer::THandlerFunction handler);
    
    // Set basic authentication
    void setAuthentication(const String& username, const String& password);
    
    // Get server instance if needed for advanced use
    WebServer* getServer() { return _server; }
    
    // Check if server is running
    bool isRunning() const;
};

#endif // HTTP_SERVER_H
