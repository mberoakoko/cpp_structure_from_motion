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
        const std::string STR_ESTIMATED_TRAJECTORY_LOCATION = "../resources/faux_trajectories/estimated_trajectories.txt";
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

    constexpr std::int32_t SLEEP_TIME = 5000;

    inline auto draw_trajectories_open_gl_context(
        PosesVector poses = std::get<PosesVector>(read_trajectory_from_disk(resources::STR_TRAJECTORY_LOCATION))
        ) -> void {

        const std::string WINDOW_NAME = "Trajectories";
        pangolin::CreateWindowAndBind(WINDOW_NAME, 1024, 768);

        // if (GLenum err = glewInit(); GLEW_OK != err) {
        //     /* Problem: glewInit failed, something is seriously wrong. */
        //     std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        //     throw std::runtime_error("Error initializing GLEW");
        // }
        // std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
        pangolin::ModelViewLookAt(0, -0.1, -1.8, 0, 0, 0, 0.0, -1.0, 0.0)
        );

        pangolin::View &d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -1024.0f / 768.0f)
            .SetHandler(new pangolin::Handler3D(s_cam));

        while (pangolin::ShouldQuit() == false) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            d_cam.Activate(s_cam);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glLineWidth(2);
            for (size_t i = 0; i < poses.size(); i++) {
                Eigen::Vector3d Ow = poses[i].translation();
                Eigen::Vector3d Xw = poses[i] * (0.01 * Eigen::Vector3d(1, 0, 0));
                Eigen::Vector3d Yw = poses[i] * (0.01 * Eigen::Vector3d(0, 1, 0));
                Eigen::Vector3d Zw = poses[i] * (0.01 * Eigen::Vector3d(0, 0, 1));
                glBegin(GL_LINES);
                glColor3f(1.0, 0.0, 0.0);
                glVertex3d(Ow[0], Ow[1], Ow[2]);
                glVertex3d(Xw[0], Xw[1], Xw[2]);
                glColor3f(0.0, 1.0, 0.0);
                glVertex3d(Ow[0], Ow[1], Ow[2]);
                glVertex3d(Yw[0], Yw[1], Yw[2]);
                glColor3f(0.0, 0.0, 1.0);
                glVertex3d(Ow[0], Ow[1], Ow[2]);
                glVertex3d(Zw[0], Zw[1], Zw[2]);
                glEnd();
            }

            for (size_t i = 0; i < poses.size(); i++) {
                glColor3f(0.0, 0.0, 0.0);
                glBegin(GL_LINES);
                auto p1 = poses[i], p2 = poses[i+1];
                glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
                glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
                glEnd();
            }


            pangolin::FinishFrame();
            usleep(SLEEP_TIME); //
        }

    }

    inline auto compare_and_draw_trajectories_open_gl_context(const PosesVector& trajectory_1, const PosesVector& trajectory_2, bool with_poses = false) -> void {
        const std::string WINDOW_NAME = "Trajectory Comparision";
        pangolin::CreateWindowAndBind(WINDOW_NAME, 1024, 768);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
        pangolin::ModelViewLookAt(0, -0.1, -1.8, 0, 0, 0, 0.0, -1.0, 0.0)
        );

        pangolin::View &d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -1024.0f / 768.0f)
            .SetHandler(new pangolin::Handler3D(s_cam));

        auto draw_poses = [&](const PosesVector& poses) {
            for (size_t i = 0; i < poses.size(); i++) {
                Eigen::Vector3d Ow = poses[i].translation();
                Eigen::Vector3d Xw = poses[i] * (0.01 * Eigen::Vector3d(1, 0, 0));
                Eigen::Vector3d Yw = poses[i] * (0.01 * Eigen::Vector3d(0, 1, 0));
                Eigen::Vector3d Zw = poses[i] * (0.01 * Eigen::Vector3d(0, 0, 1));
                glBegin(GL_LINES);
                glColor3f(1.0, 0.0, 0.0);
                glVertex3d(Ow[0], Ow[1], Ow[2]);
                glVertex3d(Xw[0], Xw[1], Xw[2]);
                glColor3f(0.0, 1.0, 0.0);
                glVertex3d(Ow[0], Ow[1], Ow[2]);
                glVertex3d(Yw[0], Yw[1], Yw[2]);
                glColor3f(0.0, 0.0, 1.0);
                glVertex3d(Ow[0], Ow[1], Ow[2]);
                glVertex3d(Zw[0], Zw[1], Zw[2]);
                glEnd();
            }
        };
        auto draw_lines = [&](const PosesVector& poses) {
            for (size_t i = 0; i < poses.size(); i++) {
                glColor3f(0.0, 0.0, 0.0);
                glBegin(GL_LINES);
                auto p1 = poses[i], p2 = poses[i+1];
                glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
                glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
                glEnd();
            }
        };
        while (pangolin::ShouldQuit() == false) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            d_cam.Activate(s_cam);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glLineWidth(2);

            if (with_poses) {
                draw_poses(trajectory_1);
                draw_poses(trajectory_2);
            }


            draw_lines(trajectory_1);
            draw_lines(trajectory_2);

            pangolin::FinishFrame();
            usleep(SLEEP_TIME);
        }
    }

    struct TrajectoryPair {
        PosesVector trajectory_1;
        PosesVector trajectory_2;

        static auto assert_is_same_size(const PosesVector& trajectory_1, const PosesVector& trajectory_2) -> void {
            assert(trajectory_1.size() == trajectory_2.size());
        }

        static auto assert_is_not_empty(const PosesVector& trajectory_1, const PosesVector& trajectory_2) -> void {
            assert(not trajectory_1.empty() == trajectory_2.empty());
        }
        auto assert_is_same_size() const -> const TrajectoryPair& {
            TrajectoryPair::assert_is_same_size(trajectory_1, trajectory_2);
            return *this;
        }

        auto assert_is_not_empty() const -> const  TrajectoryPair& {
            TrajectoryPair::assert_is_not_empty(trajectory_1, trajectory_2);
            return *this;
        }
    };

    auto root_mean_square_trajectory(const TrajectoryPair& trajectory_pair) -> double {
        auto _ = trajectory_pair.assert_is_same_size()
                                                .assert_is_not_empty();

        return  0.0;

    }

}

#endif //TRAJECTORY_UTILS_HPP
