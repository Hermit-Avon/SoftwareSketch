# include <iostream>
# include "sliding.h"

void set_fivetuple(struct fivetuple* ft, uint32_t _saddr, uint32_t _daddr, uint16_t _sport, uint16_t _dport, uint8_t _proto){
    ft -> saddr = _saddr;
    ft -> daddr = _daddr;
    ft -> sport = _sport;
    ft -> dport = _dport;
    ft -> proto = _proto;
}

void get_fivetuple(const fivetuple& ft){
    std::cout << "The fivetuple is " << ft.saddr << ", " << ft.daddr << ", " << ft.sport << ", " << ft.dport << ", " << ft.proto << std::endl;
}

int main(){
    int cycle_num = 200000;
    int field_num = 3;
    int hash_number = 4;
    int memory = 4;
    int row_length = memory * 1024 * 1024 / hash_number / 4*(field_num); 
    Recent_Counter sl(cycle_num, hash_number * row_length , row_length, hash_number, field_num);
    fivetuple major_flow;
    fivetuple small_flow;
    set_fivetuple(&major_flow, 1, 1, 1, 1, 1);
    set_fivetuple(&small_flow, 2, 2, 2, 2, 2);
    get_fivetuple(major_flow);
    get_fivetuple(small_flow);

    sl.Update((fivetuple *)&major_flow, 1);
    sl.Update((fivetuple *)&major_flow, 1);
    sl.Update((fivetuple *)&major_flow, 1);
    sl.Update((fivetuple *)&small_flow, 1);
    sl.Update((fivetuple *)&major_flow, 1);
    auto rs = sl.Query(&major_flow);
    std::cout << rs << std::endl;
    return 0;
}