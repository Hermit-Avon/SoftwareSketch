import glob

# 初始化字典来存储每个测试对象的总周期数、总吞吐量和数据条数
data_dict = {}
CPU_F = 2.4
# 遍历所有以res开头的txt文件
for file_path in glob.glob('res*.txt'):
    print(file_path)
    pkt_size = int(file_path[file_path.find("_") + 1: file_path.find(".")])
   #  print(data_dict, pkt_size)
    with open(file_path, 'r') as file:
        for line in file:
            line = line.strip()
            if not line:
                continue
                       # 分割行数据
            parts = line.split()
            test_object = parts[0]
            cycles = float(parts[2].split(':')[1])
            throughput = float(parts[3].split(':')[1]) 
            # 更新字典数据
            if test_object not in data_dict:
                data_dict[test_object] = {
                    'total_cycles': 0.0,
                    'total_throughput': 0.0,
                    'count': 0
                }
            data_dict[test_object]['total_cycles'] += cycles
            data_dict[test_object]['total_throughput'] += throughput
            data_dict[test_object]['count'] += 1


# 计算并打印平均值（按测试对象名称排序）
        for obj in sorted(data_dict.keys()):
            avg_cycles = data_dict[obj]['total_cycles'] / data_dict[obj]['count']
            avg_latency = avg_cycles / CPU_F
            avg_throughput = data_dict[obj]['total_throughput'] / data_dict[obj]['count'] * pkt_size * 8 / 1000000000
            print(f"{obj}: 平均测试周期数 = {avg_cycles:.2f}, 平均测试吞吐量 = {avg_throughput:.2f}, 平均测试吞吐量 = {avg_latency:.2f}")
        data_dict = dict()
