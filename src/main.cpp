/**
 * @file main.cpp
 * @brief Main program file for modbus-test
 * @date 02-13-2025
 */
#include <iostream>
#include <string>
#include <modbus.h>
#include "config.h"

#define MAX_REGS 64
#define REG_BUF_SZ MAX_REGS*2
#define DEFAULT_PORT_INT 502   // accepted by modbus_t *modbus_new_tcp(const char *ip, int port)
#define DEFAULT_PORT_STR "502" // accepted by modbus_t *modbus_new_tcp_pi(const char *node, const char *service);

// read regs
// ./bin/main -ip 192.168.122.200 -s 1 -f 3 -a 20000 -n 4 (works ok with modbus_new_tcp())
// ./bin/main -ip fe80:0000:0000:0000:6252:d0ff:fe07:40f1 -s 1 -f 3 -a 20000 -n 4 (results in bad arg msg from modbus driver)
// ./bin/main -ip fe80::6252:d0ff:fe07:40f1 -s 1 -f 3 -a 20000 -n 4 (results in bad arg msg from modbus driver)

// ./bin/main -ip 6252:d0ff:fe07:40f1 -s 1 -f 3 -a 20000 -n 4 (results in connection refused msg from modbus driver)
//                                                            (nothing is even coming out of the port according to)
//                                                            (wireshark capture..)


// write regs
// ./bin/main -ip 192.168.122.200 -s 1 -f 16 -a 20000 -n 4 -v 1:2:3:4cl
// write regs with hex address and hex values
// ./bin/main -ip 192.168.122.200 -s 1 -f 16 -a -h 4e20 -n 4 -v -h a:b:c:d

std::string target_ip = "";

typedef struct _Modbus_PDU {
  uint8_t slave_id;
  uint8_t function;
  uint16_t address;
  uint16_t num_regs;
  uint16_t values[MAX_REGS];
} Modbus_PDU;

Modbus_PDU modbus_pdu = {0, 0, 0, 0, {0}};

void print_target_ip()
{
  std::cout << "\nTarget IP: " << target_ip << "\n";
}

void print_modbus_pdu(void)
{
  // print all values in modbus_pdu
  std::cout << "\nModbus PDU:\n";
  std::cout << "Slave ID: " << static_cast<int>(modbus_pdu.slave_id) << "\n";
  std::cout << "Function: " << static_cast<int>(modbus_pdu.function) << "\n";
  std::cout << "Address: " << static_cast<int>(modbus_pdu.address) << "\n";
  std::cout << "Number of registers: " << modbus_pdu.num_regs << "\n";

  if(MODBUS_FC_WRITE_MULTIPLE_REGISTERS == modbus_pdu.function) {
    for(int i = 0; i < modbus_pdu.num_regs; i++) {
      std::cout << "Value " << i << ": " << modbus_pdu.values[i] << "\n";
    }
  }

  std::cout << "\n";
}

/**
 * @brief Init from command line arguments
 * @return 0 if success, -1 if error
 */
int init(int argc, char *argv[])
{
  int argi = 1;
  bool use_hex;

  while(--argc) {

    if(std::string(argv[argi]).compare("-ip") == 0) {
      target_ip = argv[argi + 1];
      argi += 2;
      --argc;
    } else if(std::string(argv[argi]).compare("-s") == 0) {
      modbus_pdu.slave_id = static_cast<uint8_t>(std::stoul(argv[argi + 1]));
      argi += 2;
      --argc;
    } else if(std::string(argv[argi]).compare("-f") == 0) {
      modbus_pdu.function = static_cast<uint8_t>(std::stoul(argv[argi + 1]));
      argi += 2;
      --argc;
    } else if(std::string(argv[argi]).compare("-a") == 0) {
      if (std::string(argv[argi + 1]).compare("-h") == 0) {
        use_hex = true;
        ++argi;
        --argc;
      } else {
        use_hex = false;
      }
      modbus_pdu.address = use_hex ? static_cast<uint16_t>(std::stoul(argv[argi + 1], nullptr, 16)) : static_cast<uint16_t>(std::stoul(argv[argi + 1]));
      argi += 2;
      --argc;
      
    } else if(std::string(argv[argi]).compare("-n") == 0) {
      modbus_pdu.num_regs = static_cast<uint16_t>(std::stoul(argv[argi + 1]));
      argi += 2;
      --argc;
    } else if(std::string(argv[argi]).compare("-v") == 0) {
      // store following values, that are delimited by ':', in the modbus_pdu.values array
      if(modbus_pdu.num_regs == 0) { // check if number of registers is set
        return -1;
      } else if (modbus_pdu.num_regs > MAX_REGS) { // check if number of registers exceeds maximum
        return -1;
      }
      if (std::string(argv[argi + 1]).compare("-h") == 0) { // check if hex values are used
        use_hex = true;
        ++argi;
        --argc;
      } else {
        use_hex = false;
      }
      // Parse input values using ':' as delimeter and store them
      std::string values = argv[argi + 1];
      size_t pos = 0;
      std::string token;
      int i = 0;
      while ((pos = values.find(":")) != std::string::npos) {
        token = values.substr(0, pos);
        modbus_pdu.values[i] = use_hex ? static_cast<uint16_t>(std::stoul(token, nullptr, 16)) : static_cast<uint16_t>(std::stoul(token));
        values.erase(0, pos + 1);
        i++;
      }
      // last value
      modbus_pdu.values[i] = use_hex ? static_cast<uint16_t>(std::stoul(values, nullptr, 16)) : static_cast<uint16_t>(std::stoul(values));
      argi += 2;
      --argc;
    } else {
      std::cout << "Error: bad arg\n";
      return -1;
    }
  }
  return 0;
}

/**
 * @brief Main program entry point
 * @return 0 if success, -1 if error
 */
int main(int argc, char *argv[])
{
  modbus_t *ctx;
  uint16_t reg_buf[64];
  int rc;

  std::cout << "ModBUS TCP CLI test program using libmodbus 3.1.11\n";
  std::cout << "--------------------------------------------------\n";

  if (argc > 2) {
    if(0!= init(argc, argv)) {
      return -1;
    }
  } else {
    std::cout << "Usage: -ip [ip] -s [slave id] -f [function] -a (-h [using hex]) [address] -n [number of registers] -v (-h [using hex]) [value]\n";
    std::cout << "Supported functions [2]: 0x03-Read Holding Registers, 0x10-Write Multiple Registers \n";
    return 0;
  }

  print_target_ip();
  print_modbus_pdu();

  //ctx = modbus_new_tcp(target_ip.c_str(), DEFAULT_PORT_INT);
  ctx = modbus_new_tcp_pi(target_ip.c_str(), DEFAULT_PORT_STR);
  if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
  }

  switch(modbus_pdu.function) {
    case MODBUS_FC_READ_HOLDING_REGISTERS:
      rc = modbus_read_registers(ctx, modbus_pdu.address, modbus_pdu.num_regs, reg_buf);
      if (rc == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
      } else {
        std::cout << "Registers read:\n";
        for (int i = 0; i < modbus_pdu.num_regs; i++) {
          printf("reg[%d]=%d (0x%X)\n", i, reg_buf[i], reg_buf[i]);
        }
      }
      break;
    case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
      rc = modbus_write_registers(ctx, modbus_pdu.address, modbus_pdu.num_regs, modbus_pdu.values);
      if (rc == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
      } else {
        std::cout << "Register write confirmation:\n";
        rc = modbus_read_registers(ctx, modbus_pdu.address, modbus_pdu.num_regs, reg_buf);
        if (rc == -1) {
          fprintf(stderr, "%s\n", modbus_strerror(errno));
        } else {
          for (int i = 0; i < modbus_pdu.num_regs; i++) {
            printf("reg[%d]=%d (0x%X)\n", i, reg_buf[i], reg_buf[i]);
          }
        }
      }
      break;
    default:
      std::cout << "Error: function not supported\n";
      rc = -1;
  }

  modbus_close(ctx);
  modbus_free(ctx);

  return 0;
}