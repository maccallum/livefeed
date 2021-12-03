#!/bin/bash

echo '#!/bin/bash'

for d in /dev/video0 /dev/video1
do
    v4l2-ctl -d /dev/video0 --list-ctrls | awk -v d="$d" '
    	BEGIN {FS = "[ \t]+|="} 
    	{
        	if($NF != "inactive") 
            	print "v4l2-ctl -d " d " -c " $2 "=" $NF
    	}
	'
done
