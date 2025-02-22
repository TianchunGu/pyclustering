from pyclustering.cluster.center_initializer import kmeans_plusplus_initializer
from pyclustering.cluster.fcm import fcm
from pyclustering.utils import read_sample
from pyclustering.cluster import cluster_visualizer

# 加载数据集（可以替换为自己的数据）
from pyclustering.samples.definitions import FAMOUS_SAMPLES
sample = read_sample("../DataSet/S1.csv")

# 使用K-Means++算法初始化聚类中心
initial_centers = kmeans_plusplus_initializer(sample, 15, kmeans_plusplus_initializer.FARTHEST_CENTER_CANDIDATE).initialize()

# 创建Fuzzy C-Means实例，启用C++实现（默认启用）
fcm_instance = fcm(sample, initial_centers)

# 执行聚类
fcm_instance.process()

# 获取聚类结果
clusters = fcm_instance.get_clusters()
centers = fcm_instance.get_centers()
iteration = fcm_instance.get_iteration()
total_iteration_time = fcm_instance.get_total_iteration_time()
average_iteration_time = fcm_instance.get_average_iteration_time()
classify_time = fcm_instance.get_classify_time()

print("iteration:"+str(iteration)+"次")
print("total_iteration_time:"+str(total_iteration_time)+"ms")
print("average_iteration_time:"+str(average_iteration_time)+"ms")
print("classify_timen:"+str(classify_time)+"ms")

# 输出聚类标签
# 初始化一个列表，长度与数据点相同，初始值为-1（表示未分配）
labels = [-1] * len(sample)

# 遍历每个簇，将数据点索引分配到对应的簇标签
for cluster_id, cluster in enumerate(clusters):
    for index in cluster:
        labels[index] = cluster_id

# 输出聚类标签
# print("聚类标签：", labels)

# 如果需要，可以将标签保存到文件
import numpy as np
np.savetxt("cluster_labels_S1.csv", labels, fmt="%d", delimiter=",", header="Cluster_Labels", comments="")


print("success!")


# 可视化结果
# visualizer = cluster_visualizer()
# visualizer.append_clusters(clusters, sample)
# visualizer.append_cluster(centers, marker='*', markersize=10)
# visualizer.show()

