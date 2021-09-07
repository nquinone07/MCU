#!/bin/bash
sudo avrdude -c arduino -p m168 -P /dev/ttyUSB0 -b 19200 -U flash:w:bin/target.hex
