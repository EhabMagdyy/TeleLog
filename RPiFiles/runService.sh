#!/bin/bash

pkill -9 -x gpuService
export LD_LIBRARY_PATH=/usr/local/lib
export COMMONAPI_CONFIG="/home/pi/Documents/ITI_9Months/CPP_Project/commonapi.ini"
export VSOMEIP_CONFIGURATION="/home/pi/Documents/ITI_9Months/CPP_Project/vsomeip.json"
export VSOMEIP_APPLICATION_NAME="Service"
export COMMONAPI_DEFAULT_BINDING="someip"
rm -rf /tmp/vsomeip*
./gpuService
