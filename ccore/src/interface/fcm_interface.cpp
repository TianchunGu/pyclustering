/*!

@authors Andrei Novikov (pyclustering@yandex.ru)
@date 2014-2020
@copyright BSD-3-Clause

*/


#include <pyclustering/interface/fcm_interface.h>

#include <pyclustering/cluster/fcm.hpp>


pyclustering_package * fcm_algorithm(const pyclustering_package * const p_sample, 
                                     const pyclustering_package * const p_centers, 
                                     const double p_m,
                                     const double p_tolerance,
                                     const std::size_t p_itermax)
{
    pyclustering::dataset data, centers;
    // 提取数据
    p_sample->extract(data);
    p_centers->extract(centers);
    // 类声明，并初始化
    pyclustering::clst::fcm algorithm(centers, p_m, p_tolerance, p_itermax);
    // 声明存储聚类结果的函数
    pyclustering::clst::fcm_data output_result;
    // 执行聚类分析
    algorithm.process(data, output_result);
    // 设置跟python对接的参数
    pyclustering_package * package = create_package_container(FCM_PACKAGE_SIZE);
    ((pyclustering_package **) package->data)[FCM_PACKAGE_INDEX_CLUSTERS] = create_package(&output_result.clusters());
    ((pyclustering_package **) package->data)[FCM_PACKAGE_INDEX_CENTERS] = create_package(&output_result.centers());
    ((pyclustering_package **) package->data)[FCM_PACKAGE_INDEX_MEMBERSHIP] = create_package(&output_result.membership());
    ((pyclustering_package **) package->data)[FCM_PACKAGE_INDEX_ITERATION] = create_package(1,output_result.iteration());
    ((pyclustering_package **) package->data)[FCM_PACKAGE_INDEX_TOTALLTERATIONTIME] = create_package(1,output_result.total_iteration_time());
    ((pyclustering_package **) package->data)[FCM_PACKAGE_INDEX_AVERAGELTERATIONTIME] = create_package(1,output_result.average_iteration_time());
    ((pyclustering_package **) package->data)[FCM_PACKAGE_INDEX_CLASSIFYTIME] = create_package(1,output_result.classify_time());

    return package;
}