# ESP32 WiFi-Enabled Project

This project provides a modular structure for ESP32-based IoT applications with WiFi and MQTT connectivity. The code is organized for reusability, maintainability, and extensibility.

## Project Structure

- **Config.h**: Central configuration file for WiFi, MQTT, and other settings
- **WiFiManager**: Class for handling WiFi connections with multiple connection strategies
- **MQTTManager**: Class for MQTT communication
- **DeviceManager**: Base class for managing device functionality
- **HttpServer**: Class for implementing a web interface and API
- **Example implementations**: Showing how to use these components together

## Usage

### Basic Setup

1. Configure your settings in `include/Config.h`:
   - WiFi credentials
   - MQTT broker information
   - Device-specific parameters

2. Choose your implementation:
   - `main.cpp`: Simple WiFi-enabled application
   - `WiFiSensorExample.cpp`: Advanced example with WiFi, MQTT, and sensor functionality

### Building and Uploading

The project includes multiple build environments in `platformio.ini`:

```
# Build and upload main application
pio run -e node32s -t upload

# Build and upload sensor example
pio run -e sensor -t upload

# Monitor serial output
pio device monitor
```

## How to Extend

### Creating a Custom Device

1. Create a new class that extends `DeviceManager`
2. Override the `sendTelemetryData()` method to include your custom sensor data
3. Implement additional methods as needed

Example:

```cpp
class MyCustomDevice : public DeviceManager {
public:
    MyCustomDevice(WiFiManager* wifi, MQTTManager* mqtt) : 
        DeviceManager(wifi, mqtt, "my_device", "1.0.0") {}
    
    void sendTelemetryData() override {
        // Your custom implementation
    }
};
```

## LED Status Indicators

- **Built-in LED**: System status
- **External LED**: Data transmission indicator
- **WiFi LED**: WiFi connection status
- **MQTT LED**: MQTT connection status

## File Descriptions

- `include/Config.h`: Configuration parameters for the entire application
- `include/WiFiManager.h`: WiFi connection management
- `src/WiFiManager.cpp`: Implementation of WiFi functions
- `include/MQTTManager.h`: MQTT communication interface
- `src/MQTTManager.cpp`: Implementation of MQTT functions
- `include/DeviceManager.h`: Base device management class
- `src/DeviceManager.cpp`: Implementation of core device functions
- `include/HttpServer.h`: HTTP server interface
- `src/HttpServer.cpp`: Implementation of HTTP server with web UI
- `src/main.cpp`: Simple WiFi-enabled application
- `src/WiFiSensorExample.cpp`: Complete example with sensor functionality

## Troubleshooting WiFi Connection Issues

If experiencing WiFi connection problems:

1. **Signal Strength Issues**: The WiFi scan shows the target network is visible (-82 dBm) but with weak signal strength. Try to:
   - Move the ESP32 closer to the router
   - Use an external antenna if available
   - Consider a WiFi extender if the device must remain in a location with poor signal

2. **Connectivity Debugging**:
   - The implementation now includes a WiFi scanner that shows all available networks
   - Multiple connection methods are tried automatically in the following sequence:
     - Standard connection attempt
     - Lower TX power mode connection
     - Static IP connection (to bypass DHCP issues)
     - Fixed channel connections (tries channel 1, 6, and 11)
   
3. **Router Configuration**:
   - Check router settings to ensure 2.4GHz network is enabled
   - Verify MAC address filtering is not blocking the device (MAC: F8:B3:B7:51:E5:E0)

## HTTP Server Features

The project includes a comprehensive HTTP server implementation with the following features:

### Web Interface

1. **Dashboard**: Main page showing device status, uptime, and navigation links
2. **LED Control**: Interactive UI to control the external LED (on/off/toggle/blink)
3. **Settings Page**: View and (in future versions) modify device configuration
4. **System Info**: Detailed hardware and network information

### API Endpoints

1. **/status**: JSON endpoint providing device data in a machine-readable format
2. **/led?action=...**: Control LED through API requests (on/off/toggle/blink)

### Security

- Basic authentication can be enabled in Config.h
- Username and password-protected routes
- HTTPS support can be added (requires additional code)

### mDNS Support

- Access the web interface through a user-friendly hostname
- Default: http://esp32-device.local/ (configurable in Config.h)

### Responsive Design

- Mobile-friendly UI with automatic refresh
- Clean, minimal design for easy navigation

## Upload Troubleshooting

If experiencing issues uploading firmware to the ESP32 board:

1. **Connection Issues**:
   - Press and hold the BOOT button while connecting
   - Use a different USB cable (data cable, not just power)
   - Try different USB ports

2. **Driver Problems**:
   - Ensure you have the correct USB-Serial drivers installed
   - Check if the device shows up in the serial port list

3. **Upload Settings**:
   - Try different upload_flags in platformio.ini
   - Common combinations:
     ```ini
     upload_flags = 
       --before=default_reset
       --after=hard_reset
     ```
     or
     ```ini
     upload_flags = 
       --before=no_reset
       --after=hard_reset
     ```

4. **Hardware Issues**:
   - Check the board's Auto-Program circuit if available
   - Some ESP32 development boards have unreliable USB-UART bridges

## Remote Access with ngrok

This project includes support for remote access to your ESP32 device via ngrok tunneling. This allows you to:

- Access your ESP32's web interface from anywhere on the internet
- Test and demonstrate your project without port forwarding
- Share access with others temporarily
- Monitor your device remotely

### Quick Start

1. Install ngrok: `brew install ngrok`
2. Authenticate ngrok: `ngrok authtoken YOUR_AUTH_TOKEN`
3. Start the tunnel: `./ngrok_tunnel.sh start`
4. Access your device at the URL provided by ngrok

For detailed instructions, see [ngrok_guide.md](ngrok_guide.md).
   - Try setting router to use a fixed channel (1, 6, or 11) instead of auto
   - Some routers have compatibility issues with ESP32 - try updating router firmware

4. **Credentials and Security**:
   - Double-check SSID and password in Config.h
   - Try temporarily reducing WiFi security from WPA2 to a less secure option just for testing

## Advanced Features

The WiFiManager includes:
- Automatic reconnection with multiple strategies
- Signal strength reporting
- Connection status monitoring
- Network scanning capability for diagnostics

The MQTTManager includes:
- Automatic reconnection
- Topic management
- JSON payload support via ArduinoJson

## License

This project is open source and available under the MIT License.
