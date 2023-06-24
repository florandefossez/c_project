#!/bin/bash

TAP_NAME="tap0"
BRIDGE_NAME="br0"
IP_ADDRESS="22.22.22.22"
NETMASK="32"

Check if the tap interface already exists
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

ip link add "$BRIDGE_NAME" type bridge
if [[ $? -ne 0 ]]; then
    echo "Failed to create bridge $BRIDGE_NAME."
    exit 1
fi

ip link set "$TAP_NAME" master "$BRIDGE_NAME"
if [[ $? -ne 0 ]]; then
    echo "Failed to add the tap to the bridge"
    exit 1
fi

ip link set "$BRIDGE_NAME" up
if [[ $? -ne 0 ]]; then
    echo "Failed to set bridge interface $BRIDGE_NAME up."
    exit 1
fi

ip route add "$IP_ADDRESS" dev "$BRIDGE_NAME"
if [[ $? -ne 0 ]]; then
    echo "Failed to add route $IP_ADDRESS via $BRIDGE_NAME."
    exit 1
fi

echo "Tap interface $TAP_NAME created successfully with IP address $IP_ADDRESS."
exit 0
