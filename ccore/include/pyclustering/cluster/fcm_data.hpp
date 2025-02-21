/*!

@authors Andrei Novikov (pyclustering@yandex.ru)
@date 2014-2020
@copyright BSD-3-Clause

*/

#pragma once


#include <memory>
#include <vector>

#include <pyclustering/cluster/cluster_data.hpp>

#include <pyclustering/definitions.hpp>


namespace pyclustering {

namespace clst {


/*!

@brief  Container for membership (probability) of each point from data.

*/
using membership_sequence = dataset;


/*!

@class    fcm_data fcm_data.hpp pyclustering/cluster/fcm_data.hpp

@brief    Clustering results of Fuzzy C-Means algorithm that consists of information about allocated
           clusters and centers of each cluster.

*/
class fcm_data : public cluster_data {
private:
    dataset       m_centers     = { };
    dataset       m_membership  = { };
    int m_iteration;//记录迭代次数
    float m_TotalIterationTime;//总迭代时间
    float m_AverageIterationTime;//平均迭代时间（更新隶属度矩阵+更新聚类中心+计算聚类中心变化量）
    float m_ClassifyTime;//聚类分类时间

public:

    /*! 
    @brief  带初始化的构造函数
    @param[in] p_centers: 聚类中心数据集
    @param[in] p_membership: 聚类隶属度数据集
    @param[in] p_iteration: 迭代次数
    @param[in] p_total_time: 总迭代时间
    @param[in] p_avg_time: 平均迭代时间
    //使用示例
    //dataset centers = {  填充数据  };
    //dataset membership = {  填充数据  };
    //int iteration = 10;
    //float total_time = 120.5f;
    //float avg_time = 12.05f;
    //fcm_data data(centers, membership, iteration, total_time, avg_time);
    如果构造函数调用时没有传递这些参数，默认值会被使用
    p_iteration = 0 表示默认迭代次数为 0。
    p_total_time = 0.0f 表示默认总迭代时间为 0.0。
    p_avg_time = 0.0f 表示默认平均迭代时间为 0.0。
    */
    fcm_data(const dataset & p_centers = {}, const dataset & p_membership = {}, 
             int p_iteration = 0
            , float p_total_time = 0.0f
            , float p_avg_time = 0.0f
            ,float p_Class_time = 0.0f
            )
        : m_centers(p_centers), 
          m_membership(p_membership),
          m_iteration(p_iteration), 
          m_TotalIterationTime(p_total_time), 
          m_AverageIterationTime(p_avg_time),
          m_ClassifyTime(p_Class_time)
        {}

    /*!
    
    @brief  Returns reference to centers of allocated clusters.

    @return Reference to centers of allocated clusters.
    
    */
    dataset & centers() { return m_centers; }

    /*!

    @brief  Returns const reference to centers of allocated clusters.

    @return Const reference to centers of allocated clusters.

    */
    const dataset & centers() const { return m_centers; };

    /*!

    @brief  Returns reference to cluster membership (probability) for each point in data.

    @return Reference to cluster membership (probability) for each point in data.

    */
    membership_sequence & membership() { return m_membership; }

    /*!

    @brief  Returns constant reference to cluster membership (probability) for each point in data.

    @return Constant reference to cluster membership (probability) for each point in data.

    */
    const membership_sequence & membership() const { return m_membership; };
    
    // 获取 m_iteration 的方法
    const int & iteration() const { return m_iteration; }  // 返回 m_iteration 的常量
    // 设置 m_iteration 的方法
    void set_iteration(int iteration) { m_iteration = iteration; }
    // 获取 m_TotalIterationTime 的方法
    const float & total_iteration_time() const { return m_TotalIterationTime; }  // 返回 m_TotalIterationTime 的常量
    // 设置 m_TotalIterationTime 的方法
    void set_total_iteration_time(float total_time) { m_TotalIterationTime = total_time; }
    // 获取 m_AverageIterationTime 的方法
    const float & average_iteration_time() const { return m_AverageIterationTime; }  // 返回 m_AverageIterationTime 的常量
    // 设置 m_AverageIterationTime 的方法
    void set_average_iteration_time(float avg_time) { m_AverageIterationTime = avg_time; }
    // 获取 m_ClassifyTime 的方法
    const float & classify_time() const { return m_ClassifyTime; }  // 返回 m_ClassifyTime 的常量
    // 设置 m_ClassifyTime 的方法
    void set_classify_time(float classify_time) { m_ClassifyTime = classify_time; }

};


}

}