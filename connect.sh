#!/bin/bash

screen -d -m -S cameracage -t rpib
screen -S cameracage -p 0 -X stuff "ssh rpib.local\n"

screen -S cameracage -X screen -t jn0 1
screen -S cameracage -p 1 -X stuff "ssh -A -t rpib.local ssh -A -t jn0.local\n"

screen -S cameracage -X screen -t jn1 2
screen -S cameracage -p 2 -X stuff "ssh -A -t rpib.local ssh -A -t jn1.local\n"

screen -S cameracage -X screen -t jn2 3
screen -S cameracage -p 3 -X stuff "ssh -A -t rpib.local ssh -A -t jn2.local\n"

screen -r cameracage
