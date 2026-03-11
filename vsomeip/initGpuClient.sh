#!/bin/bash
export LD_LIBRARY_PATH=/usr/local/lib
export COMMONAPI_CONFIG="$PWD/commonapi.ini"
export VSOMEIP_APPLICATION_NAME="gpuClient"
export COMMONAPI_DEFAULT_BINDING="someip"
export VSOMEIP_CONFIGURATION="$PWD/vsomeip/gpuClient.json"

sudo ip route add 224.224.224.245 dev lo 2>/dev/null

echo "Starting Client..."