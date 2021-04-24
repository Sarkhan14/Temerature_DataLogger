#/bin/bash

#set baudrate for MySmartUSB: 19200
stty ospeed 19200 -F /dev/ttyUSB_MySmartUSB

#send command to set to UART mode
echo -en '\xE6' > /dev/ttyUSB_MySmartUSB
echo -en '\xB5' > /dev/ttyUSB_MySmartUSB
echo -en '\xBA' > /dev/ttyUSB_MySmartUSB
echo -en '\xB9' > /dev/ttyUSB_MySmartUSB
echo -en '\xB2' > /dev/ttyUSB_MySmartUSB
echo -en '\xB3' > /dev/ttyUSB_MySmartUSB
echo -en '\xA9' > /dev/ttyUSB_MySmartUSB
echo -en '\x70' > /dev/ttyUSB_MySmartUSB
