/*!

@authors Andrei Novikov (pyclustering@yandex.ru)
@date 2014-2020
@copyright BSD-3-Clause

*/


#include <pyclustering/cluster/fcm.hpp>

#include <pyclustering/utils/metric.hpp>

#include <pyclustering/parallel/parallel.hpp>

#include <chrono>//测量时间所需头文件

using namespace pyclustering::parallel;
using namespace pyclustering::utils::metric;


namespace pyclustering {

namespace clst {


const double             fcm::DEFAULT_TOLERANCE                       = 0.001;

const std::size_t        fcm::DEFAULT_ITERMAX                         = 100;

const double             fcm::DEFAULT_HYPER_PARAMETER                 = 2.0;


fcm::fcm(const dataset & p_initial_centers, const double p_m, const double p_tolerance, const std::size_t p_itermax) :
    m_tolerance(p_tolerance),
    m_itermax(p_itermax),
    m_initial_centers(p_initial_centers)
{
    if (p_m <= 1.0) {
        throw std::invalid_argument("Hyper parameter should be greater than 1.0.");
    }

    m_degree = 2.0 / (p_m - 1.0);
}


void fcm::process(const dataset & p_data, fcm_data & p_result) {
    m_ptr_data = &p_data;
    m_ptr_result = &p_result;
    
    m_ptr_result->centers().assign(m_initial_centers.begin(), m_initial_centers.end());

    if (m_itermax == 0) { return; }
    // 将隶属度矩阵的大小调整为 m_ptr_data->size() 行（样本数量）和 m_initial_centers.size() 列（聚类中心数量），并把矩阵中的所有元素初始化为 0.0
    m_ptr_result->membership().resize(m_ptr_data->size(), point(m_initial_centers.size(), 0.0));

    float current_change = std::numeric_limits< float>::max();
    // 循环迭代
    std::size_t iteration = 0;//记录迭代次数
    // 测量总循环迭代时间
    auto startTotalTime = std::chrono::steady_clock::now();//计时开始
    for(iteration = 0; iteration < m_itermax && current_change > m_tolerance; iteration++) {
        update_membership();
        current_change = update_centers();
    }
    auto  endTotalTime = std::chrono::steady_clock::now();//计时结束
    // 计算毫秒（ms）时间
    float  totalduration_second = std::chrono::duration<float,std::milli>( endTotalTime - startTotalTime).count();//计算时间差
    // 设置相关参数
    m_ptr_result->set_iteration(iteration);
    m_ptr_result->set_total_iteration_time( totalduration_second);
    m_ptr_result->set_average_iteration_time( totalduration_second/iteration);

    // 从模糊C均值（Fuzzy C-Means）算法的结果中提取出各个聚类的成员（即每个数据点属于哪个聚类）
    // 测量分类时间
    auto startClassifyTime = std::chrono::steady_clock::now();//计时开始
    extract_clusters(m_ptr_result->clusters());
    auto  endClassifyTime = std::chrono::steady_clock::now();//计时结束
    // 计算毫秒（ms）时间
    float  Classify_second = std::chrono::duration<float,std::milli>( endClassifyTime - startClassifyTime).count();//计算时间差
    m_ptr_result->set_classify_time(Classify_second);
}


void fcm::verify() const {
    if (m_ptr_data->at(0).size() != m_initial_centers[0].size()) {
        throw std::invalid_argument("Dimension of the input data and dimension of the initial cluster centers must be the same.");
    }
}

// 求出新旧聚类中心变化的最大值
double fcm::update_centers() {
    const std::size_t amount_centers = m_ptr_result->centers().size();

    std::vector<double> changes(amount_centers, 0.0);
    // 并行实现，（如何实现还不清楚）
    parallel_for(std::size_t(0), amount_centers, [this, &changes](const std::size_t p_index) {
        changes[p_index] = update_center(p_index);
    });

    return *(std::max_element(changes.cbegin(), changes.cend()));
}

// 更新聚类中心（单维度）
double fcm::update_center(const std::size_t p_index) {
    const std::size_t dimensions = m_ptr_data->at(0).size();
    const std::size_t data_length = m_ptr_data->size();

    std::vector<double> dividend(dimensions, 0.0);
    std::vector<double> divider(dimensions, 0.0);
    for (std::size_t j = 0; j < data_length; j++) {
        for (std::size_t dimension = 0; dimension < dimensions; dimension++) {
            dividend[dimension] += m_ptr_data->at(j).at(dimension) * m_ptr_result->membership()[j][p_index];
            divider[dimension] += m_ptr_result->membership()[j][p_index];
        }
    }

    point update_center(dimensions, 0.0);
    for (std::size_t dimension = 0; dimension < dimensions; dimension++) {
        update_center[dimension] = dividend[dimension] / divider[dimension];
    }

    double change = euclidean_distance(update_center, m_ptr_result->centers().at(p_index));
    m_ptr_result->centers().at(p_index) = std::move(update_center);

    return change;
}


void fcm::update_membership() {
    const std::size_t data_size = m_ptr_result->membership().size();

    parallel_for(std::size_t(0), data_size, [this](std::size_t p_index) {
        update_point_membership(p_index);
    });
}


void fcm::update_point_membership(const std::size_t p_index) {
    const std::size_t center_amount = m_ptr_result->centers().size();
    std::vector<double> differences(center_amount, 0.0);
    for (std::size_t j = 0; j < center_amount; j++) {
        differences[j] = euclidean_distance_square(m_ptr_data->at(p_index), m_ptr_result->centers().at(j));
    }

    for (std::size_t j = 0; j < center_amount; j++) {
        double divider = 0.0;
        for (std::size_t k = 0; k < center_amount; k++) {
            if (differences[k] != 0.0) {
                divider += std::pow(differences[j] / differences[k], m_degree);
            }
        }

        if (divider == 0.0) {
            m_ptr_result->membership()[p_index][j] = 1.0;
        }
        else {
            m_ptr_result->membership()[p_index][j] = 1.0 / divider;
        }
    }
}

// 从模糊C均值（Fuzzy C-Means）算法的结果中提取出各个聚类的成员（即每个数据点属于哪个聚类）。
// 串行实现
void fcm::extract_clusters(cluster_sequence & p_clusters) {
    m_ptr_result->clusters() = cluster_sequence(m_ptr_result->centers().size());
    for (std::size_t i = 0; i < m_ptr_data->size(); i++) {
        const auto & membership = m_ptr_result->membership().at(i);
        auto iter = std::max_element(membership.begin(), membership.end());
        std::size_t index_cluster = iter - membership.begin();

        m_ptr_result->clusters().at(index_cluster).push_back(i);
    }
}


}

}