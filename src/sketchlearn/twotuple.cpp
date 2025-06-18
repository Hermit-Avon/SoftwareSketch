#include "twotuple.h"

int extract_twotpl_from_file(string fname, std::vector<twoTuple_t>& arr) {
    ifstream infile(fname);
    string s;
    int cnt = 0;
    while (getline(infile, s)) {
        vector<string> v;
        istringstream in(s);
        string subs;
        while (in >> subs) {
            v.push_back(subs);
        }
        uint32_t src_ip = static_cast<uint32_t>(std::stoul(v[0]));
        uint32_t dst_ip = static_cast<uint32_t>(std::stoul(v[1]));
        //cout << src_ip << " " << dst_ip << " " << size << endl;
        twoTuple_t* tpl = (twoTuple_t*)malloc(sizeof(twoTuple_t));
        tpl->srcIP = src_ip;
        tpl->dstIP = dst_ip;
        arr.push_back(*tpl);
        cnt++;
    }
    return cnt;
}

uint32_t get_uint32_IP(uint8_t* arr) {
    uint32_t a1=arr[0], b1=arr[1], c1=arr[2], d1=arr[3];
    a1 *= 16777216;
    b1 *= 65536;
    c1 *= 256;
    return a1+b1+c1+d1;
}

twoTuple_t* convert_fivetpl_to_twotpl(struct pkt_5tuple* fivetpl) {
    twoTuple_t* twotpl = (twoTuple_t*)malloc(sizeof(twoTuple_t));
    twotpl->srcIP = get_uint32_IP((uint8_t*)&fivetpl->src_ip);
    twotpl->dstIP = get_uint32_IP((uint8_t*)&fivetpl->dst_ip);
    return twotpl;
}

twoTuple_t* from_uint64_to_twotpl(uint64_t key) {
    twoTuple_t* ret = (twoTuple_t*)malloc(sizeof(twoTuple_t));
    ret->srcIP = key>>32;
    ret->dstIP = key&0xFFFFFFFF;
    return ret;
}

uint64_t combine_IPs(uint32_t srcIP, uint32_t dstIP) {
	uint64_t k1 = (uint64_t)srcIP;
	uint64_t k2 = (uint64_t)dstIP;
	uint64_t key = (k1<<32)+k2;
	return key;
}

// debug:
// print a uint64 variable as a binary sequence
void printbits(unsigned long n) {
    unsigned long i;
    int cnt = 0;
    i = 1UL<<(sizeof(n)*CHAR_BIT-1);
    while(i>0){
         if(n&i)
              printf("1"); 
         else 
              printf("0"); 
         i >>= 1;
         cnt++;
         if (cnt%8 == 0) printf(" ");
    }
    printf("\n");
}

std::string get_bits_from_key(unsigned long n) {
    unsigned long i;
    std::string ret = "";
    i = 1UL<<(sizeof(n)*CHAR_BIT-1);
    while(i>0){
         if(n&i)
              ret += "1";
         else 
              ret += "0";
         i >>= 1;
    }
    return ret;
}

void get_key_from_bits(std::string key_str, uint32_t* srcIP_p, uint32_t* dstIP_p) {
    std::string srcIP_str = "";
    std::string dstIP_str = "";
    for (int i = 0; i < key_str.size(); i++) {
        if (i < 32) srcIP_str += key_str[i];
        else dstIP_str += key_str[i];
    }

    //std::cout << srcIP_str << " " << dstIP_str << std::endl;

    *(srcIP_p) = std::stoul(srcIP_str, nullptr, 2);
    *(dstIP_p) = std::stoul(dstIP_str, nullptr, 2);

    //std::cout << *(srcIP_p) << " " << *(dstIP_p) << std::endl;
}

