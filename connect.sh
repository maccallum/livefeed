#!/bin/bash

if [ $# -eq 0 ]
then
    n=2
else
    n=$(($1 - 1))
fi

screen -d -m -fn -S cameracage -t rpib
screen -S cameracage -p 0 -X stuff "ssh rpib.local\n"

for nn in $(seq 0 "$n")
do
    screen -S cameracage -X screen -t "jn${nn}" $(($nn + 1))
    screen -S cameracage -p $(($nn + 1)) -X stuff "ssh -A -t rpib.local ssh -A -t jn${nn}.local\n"
done

screen -S cameracage -X screen -t "local" $(($n + 1))

screen -r cameracage
