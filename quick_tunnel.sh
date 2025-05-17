#!/bin/zsh
# Simple script to start an ngrok tunnel to 172.20.10.2:80

# Check if ngrok is installed
if ! command -v ngrok &> /dev/null; then
    echo "Error: ngrok is not installed."
    echo "Please install it with: brew install ngrok"
    exit 1
fi

# Check if authtoken is provided as argument
if [ -n "$1" ]; then
    echo "Setting ngrok authtoken..."
    ngrok config add-authtoken "$1"
    echo "Authtoken configured."
fi

# Start the tunnel
echo "Starting ngrok tunnel to 172.20.10.2:80..."
ngrok http 172.20.10.2:80

# Note: This script will keep running until you press Ctrl+C
# The tunnel URL will be displayed in the ngrok interface
