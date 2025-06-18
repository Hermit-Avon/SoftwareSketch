#ifndef __TWOTUPLE_H__
#define __TWOTUPLE_H__
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cinttypes>
#include <climits>
#include <cmath>
#include <fstream>
#include <sstream>
using namespace std;

struct twoTuple_t {
    uint32_t srcIP; 
    uint32_t dstIP; 
};

struct pkt_5tuple {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t proto;
} __attribute__ ((packed));

int extract_twotpl_from_file(string fname, std::vector<twoTuple_t>& arr);

// Tools
uint32_t get_uint32_IP(uint8_t* arr);
twoTuple_t* convert_fivetpl_to_twotpl(struct pkt_5tuple* fivetpl);
uint64_t combine_IPs(uint32_t srcIP, uint32_t dstIP);
twoTuple_t* from_uint64_to_twotpl(uint64_t key);
void printbits(unsigned long n);
std::string get_bits_from_key(unsigned long n);
void get_key_from_bits(std::string key_str, uint32_t* srcIP_p, uint32_t* dstIP_p);
#endif