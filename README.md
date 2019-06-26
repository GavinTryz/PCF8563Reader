# PCF8563Reader
Arduino sketch built to read the date and time on a PCF8563 without changing any data

Code by Gavin Tryzbiak

###Important:
* The Serial Monitor MUST match "#define BAUD_RATE 115200" from line 33 in the dropdown menu of the Serial Monitor! By default it is 115200, but it can be set lower if your cable has a poor connection, or higher for quicker text output.

### Note:
* You may need to change your register! Do this by changing "#define I2C_ADDRESS 0x51" from line 34 to whatever register your chip uses. This sketch does not support the serparate r/w address version of the chip.
* You may think your chip has separate addresses, but use an "I2CScanner" sketch to check, as it may actually be a single-address chip.
