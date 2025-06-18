#ifndef __SL_H__
#define __SL_H__
#include <linux/types.h>
#include <stdint.h>
#include "murmur3.h"

#define ROWS    8
#define LEVEL   4
#define COLUMNS 16384

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

static const __u32 seeds[] = {1,2,3,4,5,6,7,8};

struct pkt_5tuple {
    __be32 src_ip;
    __be32 dst_ip;
    __be16 src_port;
    __be16 dst_port;
    __u8 proto;
} __attribute__ ((packed));

struct sketchlearn {
    __u32 values[LEVEL+1][ROWS][COLUMNS];
};

static inline void update(struct sketchlearn *sl, struct pkt_5tuple *pkt){
    __u32 hashes[ROWS];
    for(int i=0; i < ROWS; i++){
       //  hashes[i] = xxh32(pkt, sizeof(struct pkt_5tuple), seeds[i]);
	hashes[i] = MurmurHash3_x86_32((const void *)pkt, sizeof(struct pkt_5tuple), seeds[i]);
    }
    __u32 flowkey = hashes[0];
    for(int i=0;i<ROWS;i++){
        __u32 index = hashes[i] % COLUMNS;
        sl->values[0][i][index]++;
    }
    for(int i=1;i<LEVEL+1;i++){
        if(CHECK_BIT(flowkey, i)){
            for(int j=1;j<ROWS;j++){
                __u32 index = hashes[j] % COLUMNS;
                sl->values[i][j][index]++;
            }
        }
    }
}

static inline int query(struct sketchlearn *sl, struct pkt_5tuple *pkt){
    __u32 hashes[ROWS];
    __u32 min = UINT32_MAX;
    for(int i=0; i < ROWS; i++){
        hashes[i] = MurmurHash3_x86_32((const void *)pkt, sizeof(struct pkt_5tuple), seeds[i]);
    }
    for(int i=0;i<ROWS;i++){
        __u32 index = hashes[i] % COLUMNS;
        __u32 value = sl->values[0][i][index];
        if(value < min){
            min = value;
        }
    }
    return min;
}
#endif
