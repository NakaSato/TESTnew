#include <Arduino.h>
#include "Config.h"
#include "WiFiManager.h"
#include "HttpServer.h"

// LED definitions
#define LED_BUILTIN 2   // Built-in LED on GPIO2
#define LED_EXTERNAL LED_EXTERNAL_PIN  // External LED defined in Config.h

// Function declarations
void blinkLED(int pin, int times, int delayMs);
void testExternalLED();
void handleWiFiStatus();
void setupHttpRoutes();
void handleLedControl();  // New function for LED control through HTTP

// Create WiFi manager instance
WiFiManager wifiManager(WIFI_SSID, WIFI_PASSWORD, LED_BUILTIN, WIFI_TIMEOUT);

// Create HTTP server instance
HttpServer httpServer(&wifiManager, HTTP_SERVER_PORT);

// WiFi connection status
bool wifiConnected = false;
unsigned long lastWiFiCheck = 0;
const unsigned long wifiCheckInterval = 10000; // Check WiFi every 10 seconds

// HTTP server process interval
unsigned long lastHttpProcess = 0;
const unsigned long httpProcessInterval = 100; // Process HTTP every 100ms

// Other variables
unsigned long lastLedToggle = 0;
const unsigned long ledToggleInterval = 1000;
bool ledState = false;

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);  // Give time for the serial monitor to connect
  
  Serial.println("\n\n===== ESP32 Node32S =====\n");
  Serial.println("Device MAC Address: " + WiFi.macAddress());
  
  // Initialize LEDs
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_EXTERNAL, OUTPUT);
  
  // Turn off both LEDs to start
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_EXTERNAL, LOW);
  
  Serial.println("LEDs initialized");
  Serial.println("Built-in LED on GPIO" + String(LED_BUILTIN));
  Serial.println("External LED on GPIO" + String(LED_EXTERNAL));
  
  // Run the LED test
  testExternalLED();
  
  // Connect to WiFi
  Serial.println("\nStarting WiFi connection process...");
  Serial.println("WiFi SSID: " + String(WIFI_SSID));
  Serial.println("Connection timeout: " + String(WIFI_TIMEOUT) + "ms");
  
  // Scan for available networks to diagnose connection issues
  wifiManager.scanNetworks();
  
  // Try to connect to WiFi with all available methods
  Serial.println("Starting WiFi connection with all available methods...");
  wifiConnected = wifiManager.begin();
  
  if (wifiConnected) {
    Serial.println("WiFi connection successful!");
    
    // Start HTTP server
    Serial.println("Starting HTTP server...");
    
    // Set authentication if defined in config
    #if defined(HTTP_USERNAME) && defined(HTTP_PASSWORD)
    httpServer.setAuthentication(HTTP_USERNAME, HTTP_PASSWORD);
    #endif
    
    // Setup custom routes before starting the server
    setupHttpRoutes();
    
    // Start the server
    httpServer.begin();
    
  } else {
    Serial.println("\nAll WiFi connection attempts failed.");
    Serial.println("Please check:");
    Serial.println("1. WiFi credentials are correct");
    Serial.println("2. The router is within range and powered on");
    Serial.println("3. The router is not blocking this device (MAC address: " + WiFi.macAddress() + ")");
    Serial.println("\nContinuing without WiFi connection...");
  }
}

void loop() {
  // Get current time
  unsigned long currentMillis = millis();
  
  // Check WiFi status periodically
  if (currentMillis - lastWiFiCheck >= wifiCheckInterval) {
    lastWiFiCheck = currentMillis;
    
    // Check and handle WiFi connection
    bool prevConnected = wifiConnected;
    wifiConnected = wifiManager.checkConnection();
    
    // Print connection status on change or periodically
    if (prevConnected != wifiConnected || (currentMillis / 60000) % 2 == 0) {
      if (wifiConnected) {
        Serial.print("WiFi connected. IP: ");
        Serial.print(wifiManager.getIPAddress());
        Serial.print(", Signal: ");
        Serial.print(wifiManager.getSignalStrength());
        Serial.println(" dBm");
        
        // If WiFi was previously disconnected and now connected,
        // try to start HTTP server if it's not already running
        if (!prevConnected && !httpServer.isRunning()) {
          Serial.println("Restarting HTTP server after WiFi reconnection...");
          httpServer.begin();
        }
      } else {
        Serial.println("WiFi disconnected. Attempting to reconnect...");
        wifiManager.printConnectionStatus(WiFi.status());
      }
    }
  }
  
  // Process HTTP client requests
  if (wifiConnected && currentMillis - lastHttpProcess >= httpProcessInterval) {
    lastHttpProcess = currentMillis;
    httpServer.handleClient();
  }
  
  // Blink external LED to indicate operation
  if (currentMillis - lastLedToggle >= ledToggleInterval) {
    lastLedToggle = currentMillis;
    ledState = !ledState;
    
    digitalWrite(LED_EXTERNAL, ledState);
    
    // Only print LED status every 5 seconds to reduce serial output
    if ((currentMillis / 5000) % 2 == 0) {
      Serial.println(ledState ? "External LED ON" : "External LED OFF");
    }
  }
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
  delay(1000);

  // Now test the external LED
  Serial.println("Testing external LED...");
  
  // Test 1: Simple on-off
  Serial.println("Test 1: Simple on-off");
  digitalWrite(LED_EXTERNAL, HIGH);
  Serial.println("External LED should be ON");
  delay(2000);
  
  digitalWrite(LED_EXTERNAL, LOW);
  Serial.println("External LED should be OFF");
  delay(1000);
  
  // Test 2: Blink pattern
  Serial.println("Test 2: Blink pattern");
  blinkLED(LED_EXTERNAL, 5, 200);
  delay(1000);
  
  // Test 3: Alternate with built-in LED
  Serial.println("Test 3: Alternating with built-in LED");
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_EXTERNAL, LOW);
    Serial.println("Built-in ON, External OFF");
    delay(500);
    
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_EXTERNAL, HIGH);
    Serial.println("Built-in OFF, External ON");
    delay(500);
  }
  
  // Turn off both LEDs at end of test
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_EXTERNAL, LOW);
  
  Serial.println("LED test sequence complete");
  Serial.println("Now starting continuous alternating pattern...");
  delay(1000);
}

/**
 * Setup custom HTTP routes
 */
void setupHttpRoutes() {
  // LED control route
  httpServer.on("/led", HTTP_GET, handleLedControl);
  
  // Settings page
  httpServer.on("/settings", HTTP_GET, []() {
    if (!httpServer.getServer()->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
      return httpServer.getServer()->requestAuthentication();
    }
    
    String html = "<!DOCTYPE html>"
      "<html>"
      "<head>"
      "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
      "<title>" DEVICE_ID " Settings</title>"
      "<style>"
      "body {font-family: Arial, sans-serif; margin: 0; padding: 20px; color: #333;}"
      "h1 {color: #0066cc;}"
      ".card {background: #f9f9f9; border-radius: 5px; padding: 15px; margin-bottom: 20px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);}"
      "label {display: block; margin-bottom: 5px; font-weight: bold;}"
      "input[type=text], input[type=password] {width: 100%; padding: 8px; margin-bottom: 15px; border: 1px solid #ddd; border-radius: 4px;}"
      "button {background: #0066cc; color: white; border: none; padding: 10px 15px; border-radius: 4px; cursor: pointer;}"
      "button:hover {background: #0055aa;}"
      ".back-link {margin-top: 20px; display: block;}"
      "</style>"
      "</head>"
      "<body>"
      "<h1>" DEVICE_ID " Settings</h1>"
      "<div class='card'>"
      "<form action='/save-settings' method='post'>"
      "<h2>Network Settings</h2>"
      "<label for='wifi_ssid'>WiFi SSID:</label>"
      "<input type='text' id='wifi_ssid' name='wifi_ssid' value='" WIFI_SSID "' readonly>"
      
      "<label for='wifi_password'>WiFi Password:</label>"
      "<input type='password' id='wifi_password' name='wifi_password' value='********' readonly>"
      
      "<h2>Device Settings</h2>"
      "<label for='device_id'>Device ID:</label>"
      "<input type='text' id='device_id' name='device_id' value='" DEVICE_ID "' readonly>"
      
      "<label for='hostname'>Device Hostname:</label>"
      "<input type='text' id='hostname' name='hostname' value='" DEVICE_HOSTNAME "' readonly>"
      
      "<p>Note: Settings are read-only in this version. Future versions will allow changing settings.</p>"
      "</form>"
      "</div>"
      "<a href='/' class='back-link'>Back to Dashboard</a>"
      "</body>"
      "</html>";
      
    httpServer.getServer()->send(200, "text/html", html);
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
      "<tr><th>Hostname</th><td>" DEVICE_HOSTNAME ".local</td></tr>"
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
 * Handle LED control from web interface
 */
void handleLedControl() {
  if (!httpServer.getServer()->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
    return httpServer.getServer()->requestAuthentication();
  }
  
  bool hasAction = false;
  String action = "";
  
  if (httpServer.getServer()->hasArg("action")) {
    action = httpServer.getServer()->arg("action");
    hasAction = true;
  }
  
  String state = ledState ? "ON" : "OFF";
  
  // Process actions
  if (hasAction) {
    if (action == "on") {
      ledState = true;
      digitalWrite(LED_EXTERNAL, HIGH);
      state = "ON";
      Serial.println("LED turned ON via web interface");
    } 
    else if (action == "off") {
      ledState = false;
      digitalWrite(LED_EXTERNAL, LOW);
      state = "OFF";
      Serial.println("LED turned OFF via web interface");
    }
    else if (action == "toggle") {
      ledState = !ledState;
      digitalWrite(LED_EXTERNAL, ledState);
      state = ledState ? "ON" : "OFF";
      Serial.println("LED toggled via web interface: " + state);
    }
    else if (action == "blink") {
      // Temporarily blink without changing the main state
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_EXTERNAL, HIGH);
        delay(200);
        digitalWrite(LED_EXTERNAL, LOW);
        delay(200);
      }
      // Restore previous state
      digitalWrite(LED_EXTERNAL, ledState);
      Serial.println("LED blink pattern executed via web interface");
    }
  }
  
  String html = "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>" DEVICE_ID " LED Control</title>"
    "<style>"
    "body {font-family: Arial, sans-serif; margin: 0; padding: 20px; color: #333;}"
    "h1 {color: #0066cc;}"
    ".card {background: #f9f9f9; border-radius: 5px; padding: 15px; margin-bottom: 20px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);}"
    "button {background: #0066cc; color: white; border: none; padding: 10px 15px; border-radius: 4px; cursor: pointer; margin-right: 10px; margin-bottom: 10px;}"
    "button:hover {background: #0055aa;}"
    ".status {font-size: 24px; font-weight: bold; margin: 20px 0;}"
    ".on {color: green;}"
    ".off {color: red;}"
    ".back-link {margin-top: 20px; display: block;}"
    ".button-row {margin: 20px 0;}"
    "</style>"
    "</head>"
    "<body>"
    "<h1>" DEVICE_ID " LED Control</h1>"
    "<div class='card'>"
    "<h2>External LED Status</h2>"
    "<div class='status " + String(state == "ON" ? "on" : "off") + "'>" + state + "</div>"
    "<div class='button-row'>"
    "<button onclick='window.location.href=\"/led?action=on\"'>Turn ON</button>"
    "<button onclick='window.location.href=\"/led?action=off\"'>Turn OFF</button>"
    "<button onclick='window.location.href=\"/led?action=toggle\"'>Toggle</button>"
    "<button onclick='window.location.href=\"/led?action=blink\"'>Blink Pattern</button>"
    "</div>"
    "</div>"
    "<a href='/' class='back-link'>Back to Dashboard</a>"
    "</body>"
    "</html>";
    
  httpServer.getServer()->send(200, "text/html", html);
}
