//TODO: sketchlearn 软件实现
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <cstring>
#include <time.h>

#include "sketchlearn.h"
#include "../../include/protocol_parsing.h"

int pkt_cnt = 10000;
int pkt_total = 10000;
long long time_total_ns = 0;
long long cycle_total = 0;
struct pkt_5tuple *ft = (struct pkt_5tuple *)malloc(sizeof(struct pkt_5tuple));
SketchLearn<8, 327680, 16> sl = SketchLearn<8, 327680, 16>();

static inline uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t get_time()
{
    struct timespec time1 = {0, 0};
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
    uint64_t ns = time1.tv_sec * 1000000000 + time1.tv_nsec;
    return ns;
}

void got_packet(u_char *argv, const struct pcap_pkthdr *header, const u_char *packet)
{

    int start_cycle = rdtsc();
    uint64_t time_start = get_time();
    IPV4_HEADER *ip = (IPV4_HEADER *)(packet + sizeof(ETH_HEADER));
    UDP_HEADER *udp = (UDP_HEADER *)(packet + sizeof(IPV4_HEADER));
    // 需要malloc,否则会有segment fault

    memset(ft, 0, sizeof(struct pkt_5tuple));
    ft->src_ip = ip->saddr;
    ft->dst_ip = ip->daddr;
    ft->src_port = udp->sport;
    ft->dst_port = udp->dport;
    ft->proto = ip->proto;
    sl.insert(ft->src_ip, ft->dst_ip, 1);
    //cm.update(ft, 1);

    uint64_t time_end = get_time();
    int end_cycle = rdtsc();

    // printf("%d cycles/packet\n", end_cycle - start_cycle);
    time_total_ns += (time_end - time_start);
    cycle_total += (end_cycle - start_cycle);
    if (pkt_cnt)
        pkt_cnt--;
    else
    {
        printf("%lld\n", time_total_ns);
        long long res = 10000000000000 / time_total_ns;
        long long cycle_res = cycle_total / pkt_total;
        printf("throughoutput:%lld \n", res);
        printf("avg cycles/packet:%lld \n", cycle_res);
        exit(0);
    }

    // printf("%d ns/packet\n", time_end - time_start);
}

int main()
{

    pthread_t tid;
    pthread_attr_t attr;

    /*get our device name*/
    const char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    // dev = pcap_lookupdev(errbuf);
    dev = "lo";
    printf("Device: %s\n", dev); /*print our device name*/

    /*open device and sniff
     *pcap_t *pcap_open_dev(char*device,int snaplen,int promisc,int to_ms,char*errbuf);
     *
     */
    pcap_t *open_dev = pcap_open_live(dev, 65535, 1, 0, errbuf);
    if (!open_dev)
    {
        printf("open device failed: %s", errbuf);
    }

    /*filter communication
     *pcap_compile(pcap_t *p,struct bpf_program *fp,char *str,int optimize,bpf_u_int32 netmask)
     *int pcap_setfilter(pcap_t *p, struct bpf_program *fp)
     */

    struct bpf_program filter;
    // char filter_exp[] = "port 80"; /*filter expressiong*/
    char filter_exp[] = ""; /*filter expressiong*/
    bpf_u_int32 mask;       /*net mask*/
    pcap_compile(open_dev, &filter, filter_exp, 0, mask);
    pcap_setfilter(open_dev, &filter);

    /*sniff
     *u_char *pcap_next(pcap_t *p,pcap_pkthdr *h)
     *int pcap_loop(pcap_t *p,int cnt,pcap_hander callback,u_char *user)
     */

    // pthread_attr_init(&attr);
    // pthread_create(&tid, &attr, runner, NULL);

    int id = 0;

    pcap_loop(open_dev, -1, got_packet, (u_char *)&id);

    pcap_close(open_dev);
    // pthread_join(tid, NULL);

    return 0;
}
