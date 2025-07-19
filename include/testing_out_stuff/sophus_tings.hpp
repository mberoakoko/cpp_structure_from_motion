//
// Created by mbero on 19/07/2025.
//

#ifndef SOPHUS_TINGS_HPP
#define SOPHUS_TINGS_HPP

#include <iostream>
#include <cmath>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "sophus/se3.hpp"
#include "sophus/so3.hpp"

#include "../../include/utils/pprint_utils.hpp"

namespace motion::tests {
    inline auto test_initializing_sophus_objects() -> void {
        const Eigen::Matrix3d rot_mat = Eigen::AngleAxisd(M_PI / 2, Eigen::Vector3d(0, 0, 1))
                                    .toRotationMatrix();
        std::cout << rot_mat << std::endl;
        const Eigen::Quaterniond quat(rot_mat);
        std::cout << utils::BLUE ;
        std::cout << quat << std::endl;
        std::cout << utils::RESET << std::endl;

        Sophus::SO3<double> so3_from_rot = Sophus::SO3<double>(rot_mat);
        std::cout << utils::BLUE ;
        std::cout << so3_from_rot.matrix() << std::endl;
        std::cout << utils::RESET << std::endl;

        Sophus::SO3<double> so3_from_quat = Sophus::SO3<double>(quat);
        std::cout << so3_from_quat.matrix() << std::endl;

    }
}

#endif //SOPHUS_TINGS_HPP
