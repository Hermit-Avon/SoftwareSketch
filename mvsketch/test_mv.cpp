#include "mvsketch.h"

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
    auto mv = MVSketch(200, 4, 104);
    fivetuple major_flow;
    fivetuple small_flow;
    set_fivetuple(&major_flow, 1, 1, 1, 1, 1);
    set_fivetuple(&small_flow, 2, 2, 2, 2, 2);
    get_fivetuple(major_flow);
    get_fivetuple(small_flow);

    mv.Update((unsigned char *)&major_flow, 1);
    mv.Update((unsigned char *)&major_flow, 1);
    mv.Update((unsigned char *)&major_flow, 1);
    mv.Update((unsigned char *)&small_flow, 1);
    mv.Update((unsigned char *)&major_flow, 1);
    std::vector<std::pair<key_tp, val_tp> > rs1;
    auto rs = mv.Query_fast(&major_flow);
    std::cout << rs << std::endl;
    return 0;
}