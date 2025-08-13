//
// Created by mbero on 22/07/2025.
//

#ifndef VISUAL_ODOMETRY_INTRO_HPP
#define VISUAL_ODOMETRY_INTRO_HPP
#include <bits/stdc++.h>
#include <opencv2/core/core.hpp>
#include <opencv2/sfm/fundamental.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <utility>

#include "sophus/common.hpp"

namespace visual_odometry::feature_extraction {


    namespace temporary_data_access {
        const std::string IMAGE_PATH_1 {"../resources/image_data/epipolar_constraints_images/test_image_1.png"};
        const std::string IMAGE_PATH_2 {"../resources/image_data/epipolar_constraints_images/test_image_2.png"};

        struct TestImages {
            cv::Mat image_1;
            cv::Mat image_2;
        };

        inline auto load_images_from_disk() -> TestImages {
            auto path_1 = std::filesystem::path(IMAGE_PATH_1);
            auto path_2 = std::filesystem::path(IMAGE_PATH_2);
            std::cout << (std::filesystem::exists(path_1) ? "true": "false") << std::endl;
            std::cout << (std::filesystem::exists(path_2) ? "true": "false") << std::endl;
            return {
                .image_1 = cv::imread(IMAGE_PATH_1, cv::IMREAD_COLOR),
                .image_2 = cv::imread(IMAGE_PATH_2, cv::IMREAD_COLOR)
            };
        }

        inline auto display_size_by_side(const TestImages& test_images) -> void {
            cv::Mat combined_image;
            cv::vconcat(test_images.image_1, test_images.image_2, combined_image);
            cv::imshow("Loaded Images", combined_image);
            cv::waitKey(0);
            cv::destroyAllWindows();
        }

    }

    using OptPtrFeatureExtractor = std::optional<cv::Ptr<cv::FeatureDetector>>;

    struct OrbTag{};
    struct SiftTag{};
    struct BriefTag{};

    class ExtractorFactory {
        using OrbPtr = cv::Ptr<cv::ORB>;
        using SiftPtr = cv::Ptr<cv::SIFT>;
    public:
        template<typename Tag, typename ... Args>
        static auto of(Args&&... args) -> OptPtrFeatureExtractor {
            if constexpr (std::is_same_v<Tag, OrbTag>) {
                return std::make_unique<std::optional<OrbPtr>>(std::forward<Args>(args)...);
            } else if constexpr (std::is_same_v<Tag, SiftTag>) {
                return std::make_unique<std::optional<SiftPtr>>(std::forward<Args>(args)...);
            } else {
                static_assert(!std::is_same_v<Tag, OrbTag> or !std::is_same_v<Tag, SiftTag>, "Error unsupported tag used !");
                return nullptr;
            }
        }
    };


    class BinaryFeatureExtractor {
        cv::Mat image_1_;
        cv::Mat image_2_;
        std::vector<cv::KeyPoint> keypoints_1_;
        std::vector<cv::KeyPoint> keypoints_2_;
        cv::Mat descriptor_1_, descriptor_2_;
        cv::Ptr<cv::FeatureDetector> detector_ = cv::ORB::create();
        // cv::Ptr<cv::FeatureDetector> detector_ = ExtractorFactory::of<FeatureTag>().value();
        cv::Ptr<cv::DescriptorExtractor> descriptor_extractor_ = cv::ORB::create();
        // cv::Ptr<cv::DescriptorExtractor> descriptor_extractor_ = ExtractorFactory::of<FeatureTag>().value();
        cv::Ptr<cv::DescriptorMatcher> matcher_ = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);

    public:
        explicit BinaryFeatureExtractor(cv::Mat  image_1, cv::Mat  image_2): image_1_(std::move(image_1)), image_2_(std::move(image_2)) {};

        auto extract_features() -> BinaryFeatureExtractor& {
            detector_->detect(image_1_, keypoints_1_);
            detector_->detect(image_2_, keypoints_2_);
            
            descriptor_extractor_->compute(image_1_, keypoints_1_, descriptor_1_);
            descriptor_extractor_->compute(image_2_, keypoints_2_, descriptor_2_);
            return *this;
        }


        [[nodiscard]]
        auto match_features(const float& threshold = 30 ) const -> std::vector<cv::DMatch> {
            assert(!descriptor_1_.empty() or descriptor_2_.empty());
            std::vector<cv::DMatch> matches_;
            matcher_->match(descriptor_1_, descriptor_2_, matches_);
            const auto [min_elem, max_elem ] = std::ranges::minmax(matches_, [](const cv::DMatch& a, cv::DMatch& b) {return a.distance < b.distance;});
            return matches_
                |   std::views::filter([min_elem, threshold](const cv::DMatch& match)
                                {return match.distance <= std::max(2*min_elem.distance, threshold);})
                | std::ranges::to<std::vector<cv::DMatch>>();
        }

        auto display_extracted_features() const -> void {
            cv::Mat output_image_1, output_image_2;
            cv::drawKeypoints(image_1_, keypoints_1_, output_image_1, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
            cv::drawKeypoints(image_2_, keypoints_1_, output_image_2, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
            cv::Mat display_image;
            cv::hconcat(output_image_1, output_image_2, display_image);
            cv::imshow("Extracted_features", display_image);
            cv::waitKey(0);
            cv::destroyAllWindows();

        }

        auto display_match_results(const float& threshold = 30) const -> void {
            auto good_matches = match_features(threshold);
            cv::Mat display_image;
            cv::drawMatches(image_1_, keypoints_1_, image_2_, keypoints_2_, good_matches, display_image);
            cv::imshow("Matches", display_image);
            cv::waitKey(0);
            cv::destroyAllWindows();

        }

    };

    namespace TUM_DATASET_DEFAULTS {
        const cv::Mat CAMERA_INTRINSICS { (cv::Mat_<double>(3, 3) <<  520.9, 0, 325.1, 0, 521.0, 249.7, 0, 0, 1) };
        const cv::Mat PRINCIPLE_POINT { };
        constexpr double FOCAL_LENGTH { 521.0 };
    }

    class PoseEstimator {
        std::vector<cv::KeyPoint> keypoints_1_;
        std::vector<cv::KeyPoint> keypoints_2_;
        std::vector<cv::DMatch> matches_;
    public:
        struct PoseEstimations {
            cv::Mat R;
            cv::Mat t;
        };
        explicit PoseEstimator(const std::vector<cv::KeyPoint>& key_points_1, const std::vector<cv::KeyPoint>& key_points_2, const std::vector<cv::DMatch>& matches)
            : keypoints_1_(keypoints_1_), keypoints_2_(key_points_2), matches_(matches) {

        }

        auto perform_pose_estimation() -> PoseEstimations {
            auto point_pairs = matches_
            |   std::views::transform([&](const cv::DMatch& match) {
                return std::make_pair(
                        keypoints_1_[match.queryIdx].pt,
                         keypoints_2_[match.trainIdx].pt
                         );
            });
            std::vector<cv::Point2d> points_1;
            std::vector<cv::Point2d> points_2;
            points_1.reserve(matches_.size());
            points_2.reserve(matches_.size());

            for (const auto& [query, train]: point_pairs) {
                points_1.push_back(query);
                points_2.push_back(train);
            }

        }

    private:
        PoseEstimations poseEstimations_;
    };

    inline auto test_binary_feature_extractor() -> void {
        const auto [image_1, image_2] = temporary_data_access::load_images_from_disk();
        auto binary_extractor = BinaryFeatureExtractor(image_1, image_2);
        binary_extractor.extract_features().display_match_results();
    }


}
#endif //VISUAL_ODOMETRY_INTRO_HPP
