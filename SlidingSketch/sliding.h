#ifndef CLOCK_H
#define CLOCK_H

#include <iostream>
#include <string.h>
#include <algorithm>
# include "murmur3.h"


struct fivetuple
{
    unsigned int saddr; // 源地址(Source address)
    unsigned int daddr; // 目的地址(Destination address)
    unsigned short sport;
    unsigned short dport;
    unsigned char proto; // 协议(Protocol)
} __attribute__((packed, aligned(1)));



class Recent_Sketch{
public :
    unsigned int clock_pos;
    unsigned int len;
    double step;
    unsigned int cycle_num;
    int row_length;
    int hash_number;
    int field_num;
    unsigned long long int last_time;

    Recent_Sketch(unsigned int c, unsigned int l, int _row_length, int _hash_number, int _field_num):
        len(l),step((double)l*(_field_num-1)/c),row_length(_row_length),hash_number(_hash_number),field_num(_field_num){
        clock_pos = 0;
        last_time = 0;
        cycle_num = 0;
    }
    int Mid(int *num);
};

class Recent_Counter: public Recent_Sketch{
public:
    int field_num;
    struct Unit{
        int* count;
        int field_num;
        int Total(){
            int ret = 0;
            for(int i = 0;i < field_num;++i){
                ret += count[i];
            }
            return ret;
        }
    };
    Unit* counter;
    

    Recent_Counter(int c, int l, int _row_length, int _hash_number, int _field_num);
    ~Recent_Counter();
    void Clock_Go(unsigned long long int num);
    void Update(fivetuple* str, unsigned long long int num);//CM Sketch update an item
    unsigned int Query(fivetuple* str);//CM(CU) Sketch update an item
};

#endif // CLOCK_H