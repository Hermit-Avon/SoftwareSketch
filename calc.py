import glob
import os
import re
import csv

# 获取当前系统的CPU主频（GHz）
def get_cpu_freq():
    try:
        with open('/proc/cpuinfo') as f:
            for line in f:
                if "cpu MHz" in line:
                    # 取第一个CPU的频率，单位MHz，转为GHz
                    mhz = float(line.strip().split(":")[1])
                    return mhz / 1000
    except Exception as e:
        print(f"无法获取CPU主频，使用默认值2.4GHz: {e}")
        return 2.4

CPU_F = get_cpu_freq()

# 初始化字典来存储每个测试对象的总周期数、总吞吐量和数据条数
data_dict = {}
# 遍历results目录下的所有txt文件
for file_path in glob.glob('results/*.txt'):
    print(file_path)
    # 用正则表达式提取文件名中的数字
    match = re.search(r'(\d+)', os.path.basename(file_path))
    if match:
        pkt_size = int(match.group(1))
    else:
        print(f"无法从文件名中提取pkt_size: {file_path}")
        continue

    data_dict = {}  # 每个文件单独统计
    with open(file_path, 'r') as file:
        for line in file:
            line = line.strip()
            if not line:
                continue
            parts = line.split()
            test_object = parts[0]
            cycles = float(parts[2].split(':')[1])
            throughput = float(parts[3].split(':')[1]) 
            if test_object not in data_dict:
                data_dict[test_object] = {
                    'total_cycles': 0.0,
                    'total_throughput': 0.0,
                    'count': 0
                }
            data_dict[test_object]['total_cycles'] += cycles
            data_dict[test_object]['total_throughput'] += throughput
            data_dict[test_object]['count'] += 1

    # 生成csv文件
    csv_path = file_path.replace('.txt', '.csv')
    with open(csv_path, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['sketch name', 'cpu cycles', 'throughput (Gbps)', 'latency (ns)'])
        for obj in sorted(data_dict.keys()):
            avg_cycles = data_dict[obj]['total_cycles'] / data_dict[obj]['count']
            avg_latency = avg_cycles / CPU_F  # 单位: ns
            avg_throughput = data_dict[obj]['total_throughput'] / data_dict[obj]['count'] * pkt_size * 8 / 1e9  # 单位: Gbps
            writer.writerow([obj, f"{avg_cycles:.2f}", f"{avg_throughput:.2f}", f"{avg_latency:.2f}"])
            print(f"{obj}: 平均测试周期数 = {avg_cycles:.2f} cycles, 平均测试吞吐量 = {avg_throughput:.2f} Gbps, 平均测试延迟 = {avg_latency:.2f} ns")
data_dict
