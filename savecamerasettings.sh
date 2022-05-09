#!/bin/bash

echo '#!/bin/bash'

for d in /dev/cam1 /dev/cam2
do
    v4l2-ctl -d "$d" --list-ctrls | awk -v d="$d" '
    	BEGIN {FS = "[ \t]+|="} 
    	{
        	if($NF != "inactive") 
            	print "v4l2-ctl -d " d " -c " $2 "=" $NF
    	}
	'
done
