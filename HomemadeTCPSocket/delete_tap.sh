#!/bin/bash

TAP_NAME="tap0"

if ip link show "$TAP_NAME" >/dev/null 2>&1; then
    ip link set dev "$TAP_NAME" down
    ip tuntap del dev "$TAP_NAME" mode tap
    echo "Tap interface $TAP_NAME removed successfully."
else
    echo "Tap interface $TAP_NAME does not exist."
fi
