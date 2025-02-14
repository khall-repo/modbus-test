# modbus-test
ModBUS TCP CLI test program using libmodbus 3.1.11

Usage: $./bin/main -ip [ip] -s [slave id] -f [function] -a (-h [using hex]) [address] -n [number of registers] -v (-h [using hex]) [value]

Supported functions [2]: 0x03-Read Holding Registers, 0x10-Write Multiple Registers