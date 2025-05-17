# Accessing your ESP32 Remotely with ngrok

This guide explains how to use ngrok to create a secure tunnel to your ESP32 device, allowing you to access it from anywhere on the internet.

## What is ngrok?

ngrok is a service that creates secure tunnels to localhost or internal networks, making them accessible from the internet. In the context of your ESP32 project, ngrok allows you to:

- Access your ESP32's web interface from anywhere
- Test webhooks and integrations without deploying
- Share your project with others without port forwarding
- Bypass NAT and firewall restrictions

## Prerequisites

1. An ngrok account (sign up at [ngrok.com](https://ngrok.com))
2. ngrok installed on your computer (`brew install ngrok` on macOS)
3. Your ESP32 device connected to your WiFi network
4. Your auth token from the ngrok dashboard

## Setup Instructions

### One-Time Setup

1. **Authenticate ngrok** with your auth token:
   ```sh
   ngrok authtoken YOUR_AUTH_TOKEN
   ```
   (Replace YOUR_AUTH_TOKEN with your token from [dashboard.ngrok.com](https://dashboard.ngrok.com))

2. **Make the tunnel script executable**:
   ```sh
   chmod +x ngrok_tunnel.sh
   ```

### Using the Tunnel

1. **Start the tunnel**:
   ```sh
   ./ngrok_tunnel.sh start
   ```
   The script will try to automatically detect your ESP32's IP address or prompt you to enter it manually.

2. **Check the tunnel status**:
   ```sh
   ./ngrok_tunnel.sh status
   ```
   This will show the public URL you can use to access your ESP32.

3. **Stop the tunnel**:
   ```sh
   ./ngrok_tunnel.sh stop
   ```

## How It Works

1. When you start the tunnel, ngrok creates a connection from your computer to the ngrok servers
2. The ngrok server provides a public URL (e.g., https://esp32-device.ngrok.io)
3. All traffic to that URL is securely forwarded to your ESP32 device
4. Your ESP32 device doesn't need any modifications - it thinks it's receiving regular HTTP requests

## Special Endpoints

Your ESP32 device includes a special `/network` endpoint that shows detailed information about the current network connection and HTTP request headers. This is particularly useful for debugging issues with ngrok tunneling.

To access it, go to:
- Local: `http://esp32-device.local/network` or `http://<ESP32-IP>/network`
- Via ngrok: `https://esp32-device.ngrok.io/network`

## Troubleshooting

1. **Cannot connect via ngrok URL**:
   - Ensure your ESP32 is powered on and connected to WiFi
   - Check if you can access the device locally first
   - Run `./ngrok_tunnel.sh status` to verify the tunnel is active

2. **ngrok tunnel disconnects**:
   - Free ngrok accounts have session time limits
   - Restart the tunnel with `./ngrok_tunnel.sh start`

3. **Wrong IP address detected**:
   - Stop the tunnel with `./ngrok_tunnel.sh stop`
   - Start again and enter the IP address manually when prompted

4. **Authentication errors**:
   - Double-check that you've run `ngrok authtoken YOUR_AUTH_TOKEN`
   - Verify your ngrok account is active
