//
// Created by mbero-akoko on 9/2/26.
//

#ifndef CPP_STRUCTURE_FROM_MOTION_OPTICAL_FLOW_HPP
#define CPP_STRUCTURE_FROM_MOTION_OPTICAL_FLOW_HPP

#include <ranges>
#include <opencv2/core.hpp>
#include <pstl/glue_execution_defs.h>

namespace motion::optical_flow {

    /**
    * @brief Sub-pixel bilinear interpolation of gray values from an OpenCV Mat..
    */
    [[nodiscard]]
    inline auto get_pixel_value(const cv::Mat& img, float x, float y) noexcept -> float {

        const float clamped_x = std::clamp(x, 0.0f, static_cast<float>(img.cols - 2));
        const float clamped_y = std::clamp(y, 0.0f, static_cast<float>(img.rows - 2));


        const int ix = static_cast<int>(clamped_x);
        const int iy = static_cast<int>(clamped_y);

        const float xx = clamped_x - static_cast<float>(ix);
        const float yy = clamped_y - static_cast<float>(iy);

        const auto p00 = static_cast<float>(img.at<uint8_t>(iy, ix));
        const auto p10 = static_cast<float>(img.at<uint8_t>(iy, ix + 1));
        const auto p01 = static_cast<float>(img.at<uint8_t>(iy + 1, ix));
        const auto p11 = static_cast<float>(img.at<uint8_t>(iy + 1, ix + 1));

        return (1.0f - xx) * (1.0f - yy) * p00 +
           xx * (1.0f - yy) * p10 +
           (1.0f - xx) * yy * p01 +
           xx * yy * p11;

    }


    struct TrackedPoint {
        cv::Point2f point{};      // Estimated 2D position in image 2
        bool success = false;     // True if tracking converged
        float error = 0.0f;       // Final alignment residual (photometric error)
    };

    /**
     * @brief Algorithmic formulation mode for Lucas-Kanade optical flow.
     */
    enum class FlowMethod {
        Forward, // Recomputes Hessian/Jacobian every iteration (higher accuracy)
        Inverse  // Precomputes Hessian/Jacobian on template patch (2-3x faster)
    };

    /**
     * @brief Configuration parameters for optical flow calculation.
     */
    struct OpticalFlowOptions {
        int half_patch_size = 4;
        int max_iterations = 10;
        float convergence_threshold = 0.01f;
        FlowMethod method = FlowMethod::Inverse;
        int pyramid_levels = 3;
        float pyramid_scale = 0.5f;
    };



    class OpticalFlowTracker {
    public:
        explicit OpticalFlowTracker(const OpticalFlowOptions &options = {}): options_(options){}

        [[nodiscard]] auto track_single_level(
            const cv::Mat &img1,
            const cv::Mat &img2,
            std::span<const cv::KeyPoint> keypoints,
            std::span<const cv::Point2f> initial_guesses = {}
        ) const -> std::vector<TrackedPoint> {

            std::vector<TrackedPoint> results(keypoints.size());
            const bool has_initial = !initial_guesses.empty();

            // Pair input keypoints with output slots using C++23 ranges
            auto index_range = std::views::iota(size_t{0}, keypoints.size());


            std::for_each(std::execution::par_unseq, std::begin(index_range), std::end(index_range),
                [&](std::size_t i) {

                    const auto kp = keypoints[i];
                    cv::Point2f target_pt = has_initial ? initial_guesses[i] : kp.pt;

                    // ---- [ Perform Lucas - Kanade Optimization here ] ----

                    bool tracked_successfully = true; // Optimization verdict
                    float final_residual = 0.05f; // Calculated residual error

                    // Populate output struct
                    results[i] = TrackedPoint{
                        .point = target_pt,
                        .success = tracked_successfully,
                        .error = final_residual
                    };
                }
            );

            return results;

        }

    private:
        OpticalFlowOptions options_;
    };


}

#endif //CPP_STRUCTURE_FROM_MOTION_OPTICAL_FLOW_HPP
