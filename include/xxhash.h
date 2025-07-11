/*
 * xxHash - Extremely Fast Hash algorithm
 * Copyright (C) 2012-2016, Yann Collet.
 *
 * BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following disclaimer
 *     in the documentation and/or other materials provided with the
 *     distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation. This program is dual-licensed; you may select
 * either version 2 of the GNU General Public License ("GPL") or BSD license
 * ("BSD").
 *
 * You can contact the author at:
 * - xxHash homepage: https://cyan4973.github.io/xxHash/
 * - xxHash source repository: https://github.com/Cyan4973/xxHash
 *
 * Adapted from lib/xxhash.c in the Linux kernel source tree.
 */

/* vmlinux.h should be included first */
#ifndef __XXHASH_H
#define __XXHASH_H
#include <asm-generic/int-ll64.h>
#include <stdint.h>
/*-*************************************
* Macros
**************************************/
#define xxh_rotl32(x, r) ((x << r) | (x >> (32 - r)))
#define xxh_rotl64(x, r) ((x << r) | (x >> (64 - r)))

/*-*************************************
* Constants
**************************************/
static const __u32 PRIME32_1 = 2654435761U;
static const __u32 PRIME32_2 = 2246822519U;
static const __u32 PRIME32_3 = 3266489917U;
static const __u32 PRIME32_4 = 668265263U;
static const __u32 PRIME32_5 = 374761393U;

static const __u64 PRIME64_1 = 11400714785074694791ULL;
static const __u64 PRIME64_2 = 14029467366897019727ULL;
static const __u64 PRIME64_3 = 1609587929392839161ULL;
static const __u64 PRIME64_4 = 9650029242287828579ULL;
static const __u64 PRIME64_5 = 2870177450012600261ULL;

/*-***************************
* Simple Hash Functions
****************************/
static inline __u32 xxh32_round(__u32 seed, const __u32 input)
{
    seed += input * PRIME32_2;
    seed = xxh_rotl32(seed, 13);
    seed *= PRIME32_1;
    return seed;
}

static inline __u32 xxh32(const void *input, const __u64 len,
                const __u32 seed)
{
    const __u8 *p = (const __u8 *)input;
    const __u8 *b_end = p + len;
    __u32 h32;

    if (len >= 16) {
        const __u8 *const limit = b_end - 16;
        __u32 v1 = seed + PRIME32_1 + PRIME32_2;
        __u32 v2 = seed + PRIME32_2;
        __u32 v3 = seed + 0;
        __u32 v4 = seed - PRIME32_1;

        do {
            v1 = xxh32_round(v1, *(__u32 *)p);
            p += 4;
            v2 = xxh32_round(v2, *(__u32 *)p);
            p += 4;
            v3 = xxh32_round(v3, *(__u32 *)p);
            p += 4;
            v4 = xxh32_round(v4, *(__u32 *)p);
            p += 4;
        } while (p <= limit);

        h32 = xxh_rotl32(v1, 1) + xxh_rotl32(v2, 7) +
            xxh_rotl32(v3, 12) + xxh_rotl32(v4, 18);
    } else {
        h32 = seed + PRIME32_5;
    }

    h32 += (__u32)len;

    while (p + 4 <= b_end) {
        h32 += *(__u32 *)p * PRIME32_3;
        h32 = xxh_rotl32(h32, 17) * PRIME32_4;
        p += 4;
    }

    while (p < b_end) {
        h32 += (*p) * PRIME32_5;
        h32 = xxh_rotl32(h32, 11) * PRIME32_1;
        p++;
    }

    h32 ^= h32 >> 15;
    h32 *= PRIME32_2;
    h32 ^= h32 >> 13;
    h32 *= PRIME32_3;
    h32 ^= h32 >> 16;

    return h32;
}

static inline __u64 xxh64_round(__u64 acc, const __u64 input)
{
    acc += input * PRIME64_2;
    acc = xxh_rotl64(acc, 31);
    acc *= PRIME64_1;
    return acc;
}

static inline __u64 xxh64_merge_round(__u64 acc, __u64 val)
{
    val = xxh64_round(0, val);
    acc ^= val;
    acc = acc * PRIME64_1 + PRIME64_4;
    return acc;
}

static inline __u64 xxh64(const void *input, const __u64 len,
                const __u64 seed)
{
    const __u8 *p = (const __u8 *)input;
    const __u8 *const b_end = p + len;
    __u64 h64;

    if (len >= 32) {
        const __u8 *const limit = b_end - 32;
        __u64 v1 = seed + PRIME64_1 + PRIME64_2;
        __u64 v2 = seed + PRIME64_2;
        __u64 v3 = seed + 0;
        __u64 v4 = seed - PRIME64_1;

        do {
            v1 = xxh64_round(v1, *(__u64 *)p);
            p += 8;
            v2 = xxh64_round(v2, *(__u64 *)p);
            p += 8;
            v3 = xxh64_round(v3, *(__u64 *)p);
            p += 8;
            v4 = xxh64_round(v4, *(__u64 *)p);
            p += 8;
        } while (p <= limit);

        h64 = xxh_rotl64(v1, 1) + xxh_rotl64(v2, 7) +
            xxh_rotl64(v3, 12) + xxh_rotl64(v4, 18);
        h64 = xxh64_merge_round(h64, v1);
        h64 = xxh64_merge_round(h64, v2);
        h64 = xxh64_merge_round(h64, v3);
        h64 = xxh64_merge_round(h64, v4);

    } else {
        h64 = seed + PRIME64_5;
    }

    h64 += (__u64)len;

    while (p + 8 <= b_end) {
        const __u64 k1 = xxh64_round(0, *(__u64 *)p);

        h64 ^= k1;
        h64 = xxh_rotl64(h64, 27) * PRIME64_1 + PRIME64_4;
        p += 8;
    }

    if (p + 4 <= b_end) {
        h64 ^= (__u64)(p)*PRIME64_1;
        h64 = xxh_rotl64(h64, 23) * PRIME64_2 + PRIME64_3;
        p += 4;
    }

    while (p < b_end) {
        h64 ^= (*p) * PRIME64_5;
        h64 = xxh_rotl64(h64, 11) * PRIME64_1;
        p++;
    }

    h64 ^= h64 >> 33;
    h64 *= PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= PRIME64_3;
    h64 ^= h64 >> 32;

    return h64;
}

static inline __u64 two_tuple_xxh(__u32 src_ip, __u32 dst_ip, __u64 index){
    __u64 k1, k2;
    k1 = (__u64)src_ip;
    k2 = (__u64)dst_ip;
    //printf("%" PRIu64 "\n", k1);
    //printf("%" PRIu64 "\n", k2);
    //printbits(k1);
    //printbits(k2);
    __u64 combined_k;
    combined_k = (k1<<32)+k2;
    //printf("%" PRIu64 "\n", combined_k);
    //printbits(combined_k);

    // convert uint64 to an array of uint8 variables
    __u8 *p = (__u8 *)&combined_k;
    __u8 key_bytes[8];
    for (int i = 0; i < 8; i++) {
        key_bytes[i] = p[i];
        //printf("%hhu ", key_bytes[i]);
    } //printf("\n");
    // hash
    __u64 key_len = 8;
    __u64 seed = index;
    return xxh64(key_bytes, key_len, seed);
 }

static inline __u64 two_tuple_sketch_hash(__u32 src_ip, __u32 dst_ip, __u64 index, __u64 mod) {
    __u64 mmh3_ret_uint64 = two_tuple_xxh(src_ip, dst_ip, index);
    int64_t res = (int64_t)mmh3_ret_uint64 % mod;
    if (res < 0) res = res+mod;
    return (__u64)res;
}
 
 #endif