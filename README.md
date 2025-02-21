# modbus-test
ModBUS TCP CLI test program using libmodbus 3.1.11

Usage: $./bin/main -ip [ip] -s [slave id] -f [function] -a (-h [using hex]) [address] -n [number of registers] -v (-h [using hex]) [value]

Supported functions [2]: 0x03-Read Holding Registers, 0x10-Write Multiple Registers

Note for using IPv6:
You have to indicate which network adapter to use when using IPv6 address and
the modbus_new_tcp_pi(addr, port).
Example bin usage:
./bin/main -ip fe80::6252:d0ff:fe07:40f1%enp27s0 -s 1 -f 3 -a 20000
This passes the string "fe80::6252:d0ff:fe07:40f1%enp27s0" to the
modbus_new_tcp_pi() address argument.

The takeaway is that you have to append "%(adaptername) to the end of the IPv6
address.
Your network adapter name can be found the usual ways, i.e. using the "ip addr"
or "ifconfig" command.

Note for note for using IPv6: I've only ever tested this on Linux, so this
may not apply word-for-word using Windows.