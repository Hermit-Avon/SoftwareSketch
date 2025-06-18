#ifndef __PROTOCOL_PARSING_H__
#define __PROTOCOL_PARSING_H__
#include <cstdint>
/*
 *以太网帧的首部
 */
typedef struct eth_header
{
    uint8_t host1[6];
    uint8_t host2[6];
    uint16_t type;
} ETH_HEADER;

/* IPv4 首部 */
typedef struct ipv4_header
{
    uint8_t ver_ihl;         // 版本 (4 bits) + 首部长度 (4 bits)
    uint8_t tos;             // 服务类型(Type of service)
    uint16_t tlen;           // 总长(Total length)
    uint16_t identification; // 标识(Identification)
    uint16_t flags_fo;       // 标志位(Flags) (3 bits) + 段偏移量(Fragment offset) (13 bits)
    uint8_t ttl;             // 存活时间(Time to live)
    uint8_t proto;           // 协议(Protocol)
    uint16_t crc;            // 首部校验和(Header checksum)
    uint32_t saddr;            // 源地址(Source address)
    uint32_t daddr;            // 目的地址(Destination address)
} IPV4_HEADER;

/* UDP 首部*/
typedef struct udp_header
{
    uint16_t sport; // 源端口(Source port)
    uint16_t dport; // 目的端口(Destination port)
    uint16_t len;   // UDP数据包长度(Datagram length),the minimum is 8
    uint16_t crc;   // 校验和(Checksum)
} UDP_HEADER;

/*TCP首部*/
typedef struct tcp_header
{

    uint16_t sport;
    uint16_t dPort;
    uint32_t uiSequNum;
    uint32_t uiAcknowledgeNum;
    uint16_t sHeaderLenAndFlag;
    uint16_t sWindowSize;
    uint16_t sCheckSum;
    uint16_t surgentPointer;
} TCP_HEADER;

#endif