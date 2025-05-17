#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
#define WIFI_SSID "W"         // Your actual WiFi SSID
#define WIFI_PASSWORD "123456asd"  // Your actual WiFi password

// WiFi connection parameters
#define WIFI_TIMEOUT 60000  // Connection timeout in milliseconds (60 seconds)
#define WIFI_STATUS_LED LED_BUILTIN  // LED to indicate WiFi status
#define WIFI_RETRY_COUNT 5  // Number of times to retry connection
#define WIFI_RETRY_DELAY 5000  // Delay between retry attempts (5 seconds)
#define WIFI_CHANNEL 1  // Fixed channel to try connecting on (optional, can help with some routers)

// MQTT Configuration (if needed)
#define MQTT_SERVER "broker.hivemq.com"  // Replace with your MQTT broker address
#define MQTT_PORT 1883                      // MQTT port (typically 1883)
#define MQTT_USERNAME ""           // Replace with your MQTT username
#define MQTT_PASSWORD ""       // Replace with your MQTT password
#define CLIENT_ID "ESP32Node"               // Client ID for MQTT connection

// Topic Structure
#define MQTT_TOPIC_PREFIX "home/sensors/"   // Prefix for all topics
#define DEVICE_ID "device001"               // Unique identifier for this device

// HTTP Server Configuration
#define HTTP_SERVER_PORT 80               // HTTP server port
#define DEVICE_HOSTNAME "esp32-device"    // mDNS hostname (access via http://esp32-device.local)
#define HTTP_USERNAME "admin"             // Optional: Username for web interface (uncomment to enable)
#define HTTP_PASSWORD "admin"             // Optional: Password for web interface (uncomment to enable)

// Other configurations
#define SERIAL_BAUD_RATE 115200   // Serial baud rate
#define LED_EXTERNAL_PIN 4        // External LED on GPIO4

#endif // CONFIG_H
