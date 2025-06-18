#pragma once
#ifndef MVKETCH_H
#define MVKETCH_H
#include <vector>
#include <unordered_set>
#include <utility>
#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>
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

typedef int val_tp;
typedef fivetuple key_tp;
typedef std::vector<std::pair<fivetuple, int> > myvector;
# define LGN 13




class MVSketch {

    typedef struct SBUCKET_type {
        //Total sum V(i, j)
        val_tp sum;
        // bool valid;
        val_tp count;

        unsigned char key[LGN];
        SBUCKET_type(): count(0){};

    } SBucket;

    struct MV_type {

        //Counter to count total degree
        val_tp sum;
        //Counter table
        SBucket **counts;

        //Outer sketch depth and width
        int depth;
        int width;

        //# key word bits
        int lgn;

        unsigned long *hash, *scale, *hardner;
    };


    public:
    MVSketch(int depth, int width, int lgn);

    ~MVSketch();

    void Update(unsigned char * key, val_tp val);

    // val_tp PointQuery(unsigned char* key);

    void Query_all(val_tp thresh, myvector& results);
    val_tp Query_fast(key_tp* key);
    val_tp Low_estimate(unsigned char* key);
    // val_tp Up_estimate(unsigned char* key);

    val_tp GetCount();

    void Reset();

    void MergeAll(MVSketch** mv_arr, int size);

    private:

    void SetBucket(int row, int column, val_tp sum, long count, unsigned char* key);

    MVSketch::SBucket** GetTable();

    MV_type mv_;
};

#endif
