#!/bin/zsh
# ESP32 ngrok tunnel manager script

# Configuration
PORT=80                   # Default HTTP port for ESP32
HOSTNAME="esp32-device"   # Your ESP32 device hostname
CONFIG_FILE="ngrok.yml"   # ngrok configuration file

# Check if ngrok is installed
if ! command -v ngrok &> /dev/null; then
    echo "Error: ngrok is not installed."
    echo "Please install it with: brew install ngrok"
    exit 1
fi

# Check for ngrok authentication
if ! ngrok config check &> /dev/null; then
    echo "ngrok does not appear to be authenticated."
    echo "Please run 'ngrok authtoken YOUR_AUTH_TOKEN' first."
    echo "Get your auth token from https://dashboard.ngrok.com/get-started/your-authtoken"
    exit 1
fi

# Create ngrok configuration file if it doesn't exist
if [ ! -f "$CONFIG_FILE" ]; then
    cat > "$CONFIG_FILE" << EOL
version: 2
authtoken: # Will be filled automatically from your ngrok config
tunnels:
  esp32:
    proto: http
    addr: ${PORT}
    subdomain: ${HOSTNAME}
EOL
    echo "Created ngrok configuration file: $CONFIG_FILE"
fi

# Function to detect ESP32 IP address
detect_esp32_ip() {
    # First try mDNS resolution
    echo "Trying to detect ESP32 using mDNS..."
    ip=$(ping -c1 "${HOSTNAME}.local" 2>/dev/null | head -n1 | grep -o -E '([0-9]{1,3}\.){3}[0-9]{1,3}')
    
    if [ -n "$ip" ]; then
        echo "Found ESP32 at $ip via mDNS"
        return 0
    fi
    
    # If mDNS fails, check serial output for IP address
    echo "mDNS detection failed. Checking serial output..."
    echo "Please connect your ESP32 and press Enter to monitor serial for IP address"
    read -r
    
    echo "Monitoring serial output for 10 seconds to detect IP address..."
    ip=$(timeout 10 pio device monitor | grep -o -E 'IP: ([0-9]{1,3}\.){3}[0-9]{1,3}' | head -n1 | cut -d' ' -f2)
    
    if [ -n "$ip" ]; then
        echo "Found ESP32 at $ip via serial monitor"
        return 0
    fi
    
    # If automatic detection fails, ask for manual entry
    echo "Could not automatically detect ESP32 IP address."
    echo "Please enter the ESP32 IP address manually:"
    read -r ip
    
    if [ -n "$ip" ]; then
        echo "Using manually entered IP: $ip"
        return 0
    fi
    
    return 1
}

# Main execution
case "$1" in
    start)
        # Detect ESP32 IP address
        if ! detect_esp32_ip; then
            echo "Failed to detect ESP32 IP address. Cannot continue."
            exit 1
        fi
        
        # Update ngrok config with IP address
        sed -i '' "s/addr: ${PORT}/addr: ${ip}:${PORT}/" "$CONFIG_FILE"
        
        # Start ngrok tunnel
        echo "Starting ngrok tunnel to ESP32 at $ip:$PORT..."
        ngrok start --config="$CONFIG_FILE" esp32
        ;;
    stop)
        # Find and kill ngrok processes
        pkill -f "ngrok" || echo "No ngrok processes found to stop"
        echo "ngrok tunnel stopped"
        ;;
    status)
        # Check if ngrok is running
        if pgrep -f "ngrok" > /dev/null; then
            echo "ngrok is running"
            # Get tunnel information
            curl -s http://localhost:4040/api/tunnels | grep -o '"public_url":"[^"]*"' | grep -o 'https://[^"]*'
        else
            echo "ngrok is not running"
        fi
        ;;
    *)
        echo "Usage: $0 {start|stop|status}"
        echo "  start  - Start ngrok tunnel to ESP32"
        echo "  stop   - Stop ngrok tunnel"
        echo "  status - Check ngrok tunnel status"
        exit 1
        ;;
esac

exit 0
