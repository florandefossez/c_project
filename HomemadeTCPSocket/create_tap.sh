#!/bin/bash

TAP_NAME="tap0"
IP_ADDRESS="22.22.22.22"
NETMASK="32"

# Check if the tap interface already exists
if ip link show "$TAP_NAME" >/dev/null 2>&1; then
    echo "Tap interface $TAP_NAME already exists."
    exit 1
fi

ip tuntap add dev "$TAP_NAME" mode tap
if [[ $? -ne 0 ]]; then
    echo "Failed to create tap interface $TAP_NAME."
    exit 1
fi

ip link set dev "$TAP_NAME" up
if [[ $? -ne 0 ]]; then
    echo "Failed to set tap interface $TAP_NAME up."
    exit 1
fi

ip route add "$IP_ADDRESS"/"$NETMASK" dev "$TAP_NAME"
if [[ $? -ne 0 ]]; then
    echo "Failed to add the route entry "$IP_ADDRESS"/"$NETMASK" to tap interface $TAP_NAME."
    exit 1
fi

echo "Tap interface $TAP_NAME created successfully with IP address $IP_ADDRESS."
exit 0
