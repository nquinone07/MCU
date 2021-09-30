#!/bin/bash
if [[ -z $1 ]]; then
    echo -e "No binary supplied!\n"
    exit -1
fi
sudo avrdude -c arduino -p m168 -P /dev/ttyUSB0 -b 19200 -U flash:w:$1
