#!/bin/bash

# Set capi vsomeip environment variables for both server and client
export LD_LIBRARY_PATH=/usr/local/lib
export COMMONAPI_CONFIG="$PWD/commonapi.ini"
export COMMONAPI_DEFAULT_BINDING="someip"
sudo ip route add 224.224.224.245 dev lo 2>/dev/null
export VSOMEIP_APPLICATION_NAME="gpuClient"
export VSOMEIP_CONFIGURATION="$PWD/vsomeip/gpuClient.json"

echo "Starting main app..."
./build/app/TeleLog
