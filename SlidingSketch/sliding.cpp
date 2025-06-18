#include "sliding.h"

#define min(a, b) (a > b ? b : a)

Recent_Counter::Recent_Counter(int c, int l, int _row_length, int _hash_numberber, int _field_num):
    Recent_Sketch(c,l,_row_length,_hash_numberber,_field_num){
    counter = new Unit [l];
    field_num = _field_num;
    for(int i = 0; i < l; i++){
        counter[i].count = new int[_field_num];
        counter[i].field_num = _field_num;
        memset(counter[i].count, 0, _field_num * sizeof(int));
    }
}

Recent_Counter::~Recent_Counter(){
    delete [] counter;
}

void Recent_Counter::Update(fivetuple* key, unsigned long long int num){
    unsigned int position;
    Clock_Go(num * step);
    for(int i = 0;i < hash_number;++i){
        position =  MurmurHash3_x86_32(key, sizeof(fivetuple), i * i) % row_length + i * row_length;
        counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;
    }
}

unsigned int Recent_Counter::Query(fivetuple* key){
    unsigned int min_num = 0x7fffffff;

    for(int i = 0;i < hash_number;++i)
        min_num = min(counter[MurmurHash3_x86_32(key, sizeof(fivetuple), i * i) % row_length + i * row_length].Total(), min_num);
    return min_num;
}

void Recent_Counter::Clock_Go(unsigned long long int num){
    for(;last_time < num;++last_time){
        counter[clock_pos].count[(cycle_num + 1) % field_num] = 0;
        clock_pos = (clock_pos + 1) % len;
        if(clock_pos == 0){
            cycle_num = (cycle_num + 1) % field_num;
        }
    }
}