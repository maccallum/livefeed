#!/bin/bash

dev="/dev/video0"

while getopts "d:" options; do
    case "${options}" in
        d)
            dev=${OPTARG}
            ;;
        \?)
            #usage
            exit 1
            ;;
    esac
done
shift $((OPTIND - 1))

echo >&1 "Resetting $dev"

set -x

v4l2-ctl -d "$dev" -c brightness=0
v4l2-ctl -d "$dev" -c contrast=32
v4l2-ctl -d "$dev" -c saturation=60
v4l2-ctl -d "$dev" -c hue=0
v4l2-ctl -d "$dev" -c gamma=100
v4l2-ctl -d "$dev" -c gain=0
v4l2-ctl -d "$dev" -c power_line_frequency=1
v4l2-ctl -d "$dev" -c white_balance_temperature_auto=0
v4l2-ctl -d "$dev" -c white_balance_temperature=4600
v4l2-ctl -d "$dev" -c white_balance_temperature_auto=1
v4l2-ctl -d "$dev" -c sharpness=2
v4l2-ctl -d "$dev" -c backlight_compensation=1
v4l2-ctl -d "$dev" -c exposure_auto=1
v4l2-ctl -d "$dev" -c exposure_absolute=157
v4l2-ctl -d "$dev" -c exposure_auto_priority=1
v4l2-ctl -d "$dev" -c exposure_auto=3

set +x
v4l2-ctl --list-ctrls

