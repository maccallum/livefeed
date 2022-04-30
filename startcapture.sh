#!/bin/bash

hn=`hostname`

# dt=`date "+%Y%m%d.%H%M%S"`

if [[ "$hn" =~ jn[[:digit:]]+$ ]]
then
	# jetson nano
    for i in 1 2; do
        pipeline="v4l2src device=/dev/cam${i} io-mode=2 
			! image/jpeg,framerate=(fraction)30/1,width=1920,height=1080 
			! nvjpegdec 
			! video/x-raw 
			! nvvidconv 
			! video/x-raw 
			! tee name=t 
			! queue leaky=1 
			! nvoverlaysink display-id=$(($i-1)) sync=false -ev 
			t. 
			! queue 
			! omxh264enc bitrate=15000000 control-rate=2 
			! qtmux 
			! filesink location=${hn}.%s.${i}.mp4 sync=false"
        echo "$pipeline"
        sudo -u john /home/john/livefeed/cc_captured -d "$pipeline" &
    done
else
    echo "unknown host!"
fi

sleep 5
sudo -u john /home/john/livefeed/cc_config

