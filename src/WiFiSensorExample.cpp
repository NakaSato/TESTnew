#include <Arduino.h>
#include "Config.h"
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "DeviceManager.h"
#include "HttpServer.h"
#include <ArduinoJson.h>

// LED definitions
#define LED_BUILTIN 2   // Built-in LED on GPIO2
#define LED_EXTERNAL LED_EXTERNAL_PIN  // External LED from Config.h
#define LED_WIFI 5      // WiFi status LED on GPIO5
#define LED_MQTT 18     // MQTT status LED on GPIO18

// Function declarations
void blinkLED(int pin, int times, int delayMs);
void testExternalLED();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void handleSensorData();
void setupHttpRoutes();
void handleSensorControl();

// Managers
WiFiManager wifiManager(WIFI_SSID, WIFI_PASSWORD, LED_WIFI, WIFI_TIMEOUT);
MQTTManager mqttManager(
    MQTT_SERVER,
    MQTT_PORT,
    MQTT_USERNAME,
    MQTT_PASSWORD,
    CLIENT_ID,
    MQTT_TOPIC_PREFIX,
    DEVICE_ID
);

// HTTP server
HttpServer httpServer(&wifiManager, HTTP_SERVER_PORT);

// Custom device manager derived class
class SensorDeviceManager : public DeviceManager {
private:
    int _sensorPin;
    
public:
    SensorDeviceManager(
        WiFiManager* wifiManager, 
        MQTTManager* mqttManager,
        int sensorPin
    ) : DeviceManager(wifiManager, mqttManager, DEVICE_ID, "1.0.0", 30000),
        _sensorPin(sensorPin) {
        
        // Initialize sensor pin
        pinMode(_sensorPin, INPUT);
    }
    
    // Override telemetry method to send sensor data
    void sendTelemetryData() override {
        if (!mqttManager.isConnected()) {
            return;
        }
        
        // Read sensor data
        int sensorValue = analogRead(_sensorPin);
        
        // Create JSON document
        JsonDocument sensorDoc;
        sensorDoc["value"] = sensorValue;
        sensorDoc["timestamp"] = millis() / 1000;
        
        // Publish sensor data
        mqttManager.publishJson("telemetry/sensor", sensorDoc, false);
        
        Serial.print("Sensor data sent: ");
        Serial.println(sensorValue);
        
        // Blink the data LED
        digitalWrite(LED_EXTERNAL, HIGH);
        delay(50);
        digitalWrite(LED_EXTERNAL, LOW);
    }
};

// Create device manager instance
SensorDeviceManager deviceManager(&wifiManager, &mqttManager, A0);

void setup() {
    // Initialize serial communication
    Serial.begin(SERIAL_BAUD_RATE);
    delay(1000);  // Give time for the serial monitor to connect
    
    Serial.println("\n\n===== ESP32 Sensor Node =====\n");
    
    // Initialize LEDs
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_EXTERNAL, OUTPUT);
    pinMode(LED_WIFI, OUTPUT);
    pinMode(LED_MQTT, OUTPUT);
    
    // Turn off all LEDs to start
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_EXTERNAL, LOW);
    digitalWrite(LED_WIFI, LOW);
    digitalWrite(LED_MQTT, LOW);
    
    Serial.println("LEDs initialized");
    
    // Run the LED test
    testExternalLED();
    
    // Set MQTT callback
    mqttManager.setCallback(mqttCallback);
    
    // Set authentication for HTTP server if defined in config
    #if defined(HTTP_USERNAME) && defined(HTTP_PASSWORD)
    httpServer.setAuthentication(HTTP_USERNAME, HTTP_PASSWORD);
    #endif
    
    // Setup custom HTTP routes
    setupHttpRoutes();
    
    // Start HTTP server if WiFi is connected
    if (wifiManager.isConnected()) {
        httpServer.begin();
        Serial.println("HTTP server started on port " + String(HTTP_SERVER_PORT));
        Serial.println("Access the web interface at http://" + wifiManager.getIPAddress());
        Serial.println("Or via mDNS at http://" DEVICE_HOSTNAME ".local");
    }
    
    // Initialize device manager
    if (deviceManager.begin()) {
        Serial.println("Device manager initialized successfully");
    } else {
        Serial.println("Device manager initialization failed");
    }
}

void loop() {
    // Main device manager loop
    deviceManager.loop();
    
    // Process HTTP client requests if WiFi is connected
    if (wifiManager.isConnected()) {
        httpServer.handleClient();
    }
}

/**
 * MQTT message callback
 */
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // Convert topic and payload to String for easier handling
    String topicStr = String(topic);
    String payloadStr = "";
    
    for (unsigned int i = 0; i < length; i++) {
        payloadStr += (char)payload[i];
    }
    
    // Process the command in device manager
    deviceManager.processCommand(topicStr, payloadStr);
    
    // Blink LED to indicate message received
    blinkLED(LED_EXTERNAL, 2, 100);
}

/**
 * Blink an LED
 * @param pin - Pin connected to the LED
 * @param times - Number of times to blink
 * @param delayMs - Delay between blinks in milliseconds
 */
void blinkLED(int pin, int times, int delayMs) {
    for (int i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);  // Turn LED on
        delay(delayMs);
        digitalWrite(pin, LOW);   // Turn LED off
        delay(delayMs);
    }
}

/**
 * Test the external LED with various patterns
 */
void testExternalLED() {
    Serial.println("\nRunning LED test sequence...");

    // First, test the built-in LED to verify the system
    Serial.println("Testing built-in LED...");
    blinkLED(LED_BUILTIN, 3, 200);
    
    // Now test the external LED
    Serial.println("Testing external LED...");
    blinkLED(LED_EXTERNAL, 3, 200);
    
    // Test WiFi status LED
    Serial.println("Testing WiFi status LED...");
    blinkLED(LED_WIFI, 3, 200);
    
    // Test MQTT status LED
    Serial.println("Testing MQTT status LED...");
    blinkLED(LED_MQTT, 3, 200);
    
    Serial.println("LED test sequence complete");
    delay(1000);
}

/**
 * Setup custom HTTP routes for the sensor example
 */
void setupHttpRoutes() {
    // Sensor data route
    httpServer.on("/sensor", HTTP_GET, handleSensorControl);
    
    // API endpoint for real-time sensor data in JSON format
    httpServer.on("/api/sensor", HTTP_GET, []() {
        if (!httpServer.getServer()->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
            return httpServer.getServer()->requestAuthentication();
        }
        
        // Read current sensor value
        int sensorValue = analogRead(A0);
        
        // Create JSON document
        StaticJsonDocument<256> doc;
        doc["device"] = DEVICE_ID;
        doc["sensor"] = "analog";
        doc["value"] = sensorValue;
        doc["raw"] = sensorValue;
        doc["percentage"] = map(sensorValue, 0, 4095, 0, 100);  // ESP32 has 12-bit ADC (0-4095)
        doc["timestamp"] = millis() / 1000;
        
        String response;
        serializeJson(doc, response);
        
        httpServer.getServer()->send(200, "application/json", response);
    });
    
    // System information page
    httpServer.on("/system", HTTP_GET, []() {
        if (!httpServer.getServer()->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
            return httpServer.getServer()->requestAuthentication();
        }
        
        String html = "<!DOCTYPE html>"
            "<html>"
            "<head>"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
            "<title>" DEVICE_ID " System Info</title>"
            "<style>"
            "body {font-family: Arial, sans-serif; margin: 0; padding: 20px; color: #333;}"
            "h1 {color: #0066cc;}"
            ".card {background: #f9f9f9; border-radius: 5px; padding: 15px; margin-bottom: 20px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);}"
            "table {width: 100%; border-collapse: collapse;}"
            "table, th, td {border: 1px solid #ddd;}"
            "th, td {padding: 10px; text-align: left;}"
            "th {background-color: #f2f2f2;}"
            ".back-link {margin-top: 20px; display: block;}"
            "</style>"
            "</head>"
            "<body>"
            "<h1>" DEVICE_ID " System Information</h1>"
            "<div class='card'>"
            "<h2>Hardware</h2>"
            "<table>"
            "<tr><th>Chip Model</th><td>ESP32</td></tr>"
            "<tr><th>CPU Frequency</th><td>" + String(ESP.getCpuFreqMHz()) + " MHz</td></tr>"
            "<tr><th>Flash Size</th><td>" + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB</td></tr>"
            "<tr><th>Free Heap</th><td>" + String(ESP.getFreeHeap() / 1024) + " KB</td></tr>"
            "</table>"
            "</div>"
            "<div class='card'>"
            "<h2>Network</h2>"
            "<table>"
            "<tr><th>WiFi SSID</th><td>" + String(WIFI_SSID) + "</td></tr>"
            "<tr><th>IP Address</th><td>" + wifiManager.getIPAddress() + "</td></tr>"
            "<tr><th>MAC Address</th><td>" + wifiManager.getMACAddress() + "</td></tr>"
            "<tr><th>Signal Strength</th><td>" + String(wifiManager.getSignalStrength()) + " dBm</td></tr>"
            "<tr><th>MQTT Broker</th><td>" + String(MQTT_SERVER) + ":" + String(MQTT_PORT) + "</td></tr>"
            "</table>"
            "</div>"
            "<div class='card'>"
            "<h2>System</h2>"
            "<table>"
            "<tr><th>Uptime</th><td>" + String(millis() / 1000) + " seconds</td></tr>"
            "<tr><th>HTTP Server Port</th><td>" + String(HTTP_SERVER_PORT) + "</td></tr>"
            "<tr><th>Authentication</th><td>" + String(HTTP_USERNAME != "" ? "Enabled" : "Disabled") + "</td></tr>"
            "</table>"
            "</div>"
            "<a href='/' class='back-link'>Back to Dashboard</a>"
            "<script>"
            "setTimeout(function() { location.reload(); }, 10000);"
            "</script>"
            "</body>"
            "</html>";
            
        httpServer.getServer()->send(200, "text/html", html);
    });
}

/**
 * Handle sensor control and visualization from web interface
 */
void handleSensorControl() {
    if (!httpServer.getServer()->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
        return httpServer.getServer()->requestAuthentication();
    }
    
    // Read current sensor value
    int sensorValue = analogRead(A0);
    int percentage = map(sensorValue, 0, 4095, 0, 100);  // ESP32 has 12-bit ADC (0-4095)
    
    String html = "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>" DEVICE_ID " Sensor Monitor</title>"
        "<style>"
        "body {font-family: Arial, sans-serif; margin: 0; padding: 20px; color: #333;}"
        "h1 {color: #0066cc;}"
        ".card {background: #f9f9f9; border-radius: 5px; padding: 15px; margin-bottom: 20px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);}"
        ".meter {height: 20px; position: relative; background: #ddd; border-radius: 25px; padding: 5px; margin: 20px 0;}"
        ".meter > span {display: block; height: 100%; border-radius: 20px; background-color: #0066cc; position: relative; overflow: hidden;}"
        ".value {font-size: 24px; font-weight: bold; margin: 10px 0;}"
        ".back-link {margin-top: 20px; display: block;}"
        "table {width: 100%; border-collapse: collapse; margin-top: 20px;}"
        "table, th, td {border: 1px solid #ddd;}"
        "th, td {padding: 10px; text-align: left;}"
        "th {background-color: #f2f2f2;}"
        ".chart-container {width: 100%; height: 300px; margin-top: 20px;}"
        "</style>"
        "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"
        "</head>"
        "<body>"
        "<h1>" DEVICE_ID " Sensor Monitor</h1>"
        "<div class='card'>"
        "<h2>Analog Sensor Reading</h2>"
        "<div class='value'>" + String(sensorValue) + " (" + String(percentage) + "%)</div>"
        "<div class='meter'>"
        "  <span style='width: " + String(percentage) + "%;'></span>"
        "</div>"
        "<table>"
        "<tr><th>Raw Value</th><td>" + String(sensorValue) + "</td></tr>"
        "<tr><th>Percentage</th><td>" + String(percentage) + "%</td></tr>"
        "<tr><th>Voltage</th><td>" + String(sensorValue * 3.3 / 4095.0, 2) + " V</td></tr>"
        "</table>"
        "</div>"
        
        "<div class='card'>"
        "<h2>Real-time Monitoring</h2>"
        "<p>The chart below updates automatically every second.</p>"
        "<div class='chart-container'>"
        "  <canvas id='sensorChart'></canvas>"
        "</div>"
        "<script>"
        "var ctx = document.getElementById('sensorChart').getContext('2d');"
        "var sensorChart = new Chart(ctx, {"
        "  type: 'line',"
        "  data: {"
        "    labels: [],"
        "    datasets: [{"
        "      label: 'Sensor Value',"
        "      backgroundColor: 'rgba(0, 102, 204, 0.2)',"
        "      borderColor: 'rgba(0, 102, 204, 1)',"
        "      data: [],"
        "      fill: true"
        "    }]"
        "  },"
        "  options: {"
        "    responsive: true,"
        "    scales: {"
        "      x: {title: {display: true, text: 'Time'}},"
        "      y: {min: 0, max: 4095, title: {display: true, text: 'Value'}}"
        "    }"
        "  }"
        "});"
        
        "// Function to update chart with new data"
        "function updateChart() {"
        "  fetch('/api/sensor')"
        "    .then(response => response.json())"
        "    .then(data => {"
        "      // Add new data to chart"
        "      var now = new Date().toLocaleTimeString();"
        "      sensorChart.data.labels.push(now);"
        "      sensorChart.data.datasets[0].data.push(data.value);"
        "      "
        "      // Keep only the last 20 points for clarity"
        "      if (sensorChart.data.labels.length > 20) {"
        "        sensorChart.data.labels.shift();"
        "        sensorChart.data.datasets[0].data.shift();"
        "      }"
        "      "
        "      sensorChart.update();"
        "    });"
        "}"
        
        "// Update chart initially and every second"
        "updateChart();"
        "setInterval(updateChart, 1000);"
        
        "// Reload page every minute to update static content"
        "setTimeout(function() { location.reload(); }, 60000);"
        "</script>"
        "</div>"
        
        "<a href='/' class='back-link'>Back to Dashboard</a>"
        "</body>"
        "</html>";
        
    httpServer.getServer()->send(200, "text/html", html);
}
