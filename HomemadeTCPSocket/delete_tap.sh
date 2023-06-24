#!/bin/bash

TAP_NAME="tap0"
BRIDGE_NAME="br0"


if ip link show "$TAP_NAME" >/dev/null 2>&1; then
    ip link set dev "$TAP_NAME" down
    ip tuntap del dev "$TAP_NAME" mode tap
else
    echo "Tap interface $TAP_NAME does not exist."
fi

if ip link show "$BRIDGE_NAME" >/dev/null 2>&1; then
    ip link set dev "$BRIDGE_NAME" down
    ip link delete "$BRIDGE_NAME"
else
    echo "Bridge does not exist."
fi