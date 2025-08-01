//
// Created by mbero on 22/07/2025.
//

#ifndef VISUAL_ODOMETRY_INTRO_HPP
#define VISUAL_ODOMETRY_INTRO_HPP
#include <bits/stdc++.h>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <utility>

#include "sophus/common.hpp"

namespace visual_odometry::feature_extraction {

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

        auto extract_features() -> cv::Mat {
            detector_->detect(image_1_, keypoints_1_);
            detector_->detect(image_2_, keypoints_2_);
            
            descriptor_extractor_->compute(image_1_, keypoints_1_, descriptor_1_);
            descriptor_extractor_->compute(image_2_, keypoints_2_, descriptor_2_);
            cv::Mat output_image;
            cv::drawKeypoints(image_1_, keypoints_1_, output_image, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
            return output_image;
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
    };


}
#endif //VISUAL_ODOMETRY_INTRO_HPP
