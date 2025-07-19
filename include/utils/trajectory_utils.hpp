//
// Created by mbero on 19/07/2025.
//

#ifndef TRAJECTORY_UTILS_HPP
#define TRAJECTORY_UTILS_HPP
#include <pangolin/pangolin.h>
#include <Eigen/Core>
#include <bits/stdc++.h>

#include "pprint_utils.hpp"
#include "sophus/geometry.hpp"

namespace motion::utils {
    namespace resources {
        const std::string STR_TRAJECTORY_LOCATION = "../resources/faux_trajectories/trajectories.txt";
        const auto TRAJECTORY_PATH = std::filesystem::path(STR_TRAJECTORY_LOCATION);
    }


    using PosesVector = std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d>>;
    using TrajectoryReadResult = std::variant<PosesVector, std::string>;

    inline auto read_trajectory_from_disk(const std::filesystem::path& path = resources::TRAJECTORY_PATH) -> TrajectoryReadResult {
        PosesVector poses{};
        std::ifstream input_file_stream(path);
        if (!input_file_stream.is_open()) {
            return "file stream is already opened";
        }
        std::cout << TURN_ON_UNDERLINE;
        std::cout << "Fetching Trajectory data from disk " << std::endl;
        std::cout << TURN_OFF_UNDERLINE << std::endl;
        while (not input_file_stream.eof()) {
            double time, tx, ty, tz, qx, qy, qz, qw;
            input_file_stream >> time >> tx >> ty >> tz >> qx >> qy >> qz >> qw;
            Eigen::Isometry3d temp_cache { Eigen::Quaterniond{qw, qx, qy, qz} };
            temp_cache.pretranslate(Eigen::Vector3d{qx, qy, qz});
            poses.push_back(temp_cache);
        }
        std::cout << TURN_ON_UNDERLINE;
        std::cout << "Completed Read  " << std::endl;
        std::cout << TURN_OFF_UNDERLINE << std::endl;

        std::cout << GREEN ;
        std::cout << "Total of  " <<poses.size() << " poses,  have been loaded from disk!" << std::endl;
        std::cout << RESET;
        return poses;
    };

    inline auto draw_trajectories_open_gl_context(
        PosesVector poses = std::get<PosesVector>(read_trajectory_from_disk(resources::STR_TRAJECTORY_LOCATION))
        ) -> void {

    }

}

#endif //TRAJECTORY_UTILS_HPP
