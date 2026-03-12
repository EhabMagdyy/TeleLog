#!/bin/bash

# Set capi vsomeip environment variables for server
export LD_LIBRARY_PATH=/usr/local/lib
export COMMONAPI_CONFIG="$PWD/commonapi.ini"
export COMMONAPI_DEFAULT_BINDING="someip"
export VSOMEIP_APPLICATION_NAME="gpuService"
export VSOMEIP_CONFIGURATION="$PWD/Handlers/vsomeip_server/gpuService.json"

sudo ip route add 224.224.224.245 dev lo 2>/dev/null
rm -f /tmp/vsomeip*

echo "Starting gpuService..."
./build/Handlers/vsomeip_server/gpuService

SERVICE_PID=$!

echo "Stopping gpuService..."
kill $SERVICE_PID
wait $SERVICE_PID 2>/dev/null