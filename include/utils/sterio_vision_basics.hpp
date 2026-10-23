//
// Created by mbero-akoko on 8/30/26.
//

#ifndef CPP_STRUCTURE_FROM_MOTION_STERIO_VISION_BASICS_HPP
#define CPP_STRUCTURE_FROM_MOTION_STERIO_VISION_BASICS_HPP
#include <eigen3/Eigen/Core>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <ranges>
#include <string>

namespace utils::tutorial {
// 1. Pure Data Structures (No hidden I/O)
struct CameraIntrinsics {
  double fx = 718.13, fy = 1.2, cx = 12.2, cy = 123.2, baseline = 0.9;
};

struct TestImages {
  cv::Mat left;
  cv::Mat right;
};

struct Disparity {
  cv::Mat disparity_sgbm;
  cv::Mat disparity_32f;
};

using PointCloud_t =
    std::vector<Eigen::Vector4d, Eigen::aligned_allocator<Eigen::Vector4d>>;
using SGBM_Ptr_t = cv::Ptr<cv::StereoSGBM>;

// The Pipe Operator
template <typename T, typename Func>
constexpr decltype(auto) operator|(T &&val, Func &&fn) {
  return std::forward<Func>(fn)(std::forward<T>(val));
}

// 2. The Source (Arity 0 -> Arity 1 output)
[[nodiscard]] inline auto read_images(const std::string &left_path,
                                      const std::string &right_path)
    -> TestImages {
  return TestImages{.left = cv::imread(left_path, cv::IMREAD_GRAYSCALE),
                    .right = cv::imread(right_path, cv::IMREAD_GRAYSCALE)};
}

// 3. Curried Combinator: Pre-binds SGBM, returns closure taking TestImages
// Notice how we propagate TestImages downstream alongside Disparity using
// std::pair
[[nodiscard]] inline auto compute_disparity(SGBM_Ptr_t sgbm) {
  return [sgbm](TestImages images) -> std::pair<TestImages, Disparity> {
    cv::Mat disp_sgbm, disp_32f;
    sgbm->compute(images.left, images.right, disp_sgbm);
    disp_sgbm.convertTo(disp_32f, CV_32F, 1.0 / 16.0f);

    return std::make_pair(
        std::move(images),
        Disparity{.disparity_sgbm = disp_sgbm, .disparity_32f = disp_32f});
  };
}

// 4. Curried Combinator: Pre-binds intrinsics/coarseness, accepts the
// propagated tuple
[[nodiscard]] inline auto
compute_point_cloud(int coarseness = 1, CameraIntrinsics intrinsics = {}) {
  // Accepts the Arity 1 tuple from the previous step
  return [coarseness, intrinsics](const std::pair<TestImages, Disparity> &input)
             -> std::pair<Disparity, PointCloud_t> {
    const auto &[images, disparity] = input; // Fan-in / Unpack
    PointCloud_t point_cloud;
    // Pre-allocate to avoid reallocations
    point_cloud.reserve((images.left.rows * images.left.cols) /
                        (coarseness * coarseness));

    for (int v = 0; v < images.left.rows; v += coarseness) {
      for (int u = 0; u < images.right.cols; u += coarseness) {
        float disp_val = disparity.disparity_32f.at<float>(v, u);
        if (disp_val <= 10.0f || disp_val >= 96.0f)
          continue;

        double depth = intrinsics.fx * intrinsics.baseline / disp_val;
        double x = (u - intrinsics.cx) / intrinsics.fx;
        double y = (v - intrinsics.cy) / intrinsics.fy;

        point_cloud.emplace_back(x * depth, y * depth, depth,
                                 images.left.at<uchar>(v, u) / 255.0);
      }
    }
    // Propagate Disparity to the display sink
    return std::make_pair(disparity, std::move(point_cloud));
  };
}

// 5. The Sink: Consumes the final tuple
inline auto display_sink() {
  return [](const std::pair<Disparity, PointCloud_t> &input) -> void {
    cv::imshow("Disparity", input.first.disparity_32f);
    cv::waitKey(0);
  };
}

inline auto point_cloud_pipeline() -> void {
  auto sgbm = cv::StereoSGBM::create(0, 96, 9, 648, 2592, 1, 63, 10, 100, 32);

  // The functional pipeline in action:
  read_images("left.png", "right.png") | compute_disparity(sgbm) |
      compute_point_cloud(2, CameraIntrinsics{}) | display_sink();
}
} // namespace utils::tutorial

#endif // CPP_STRUCTURE_FROM_MOTION_STERIO_VISION_BASICS_HPP
