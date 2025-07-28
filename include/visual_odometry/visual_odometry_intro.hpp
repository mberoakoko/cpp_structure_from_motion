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

namespace visual_odometry::feature_extraction {
    class BinaryFeatureExtractor {
        cv::Mat image_1_;
        cv::Mat image_2_;
        std::vector<cv::KeyPoint> keypoints_1_;
        std::vector<cv::KeyPoint> keypoints_2_;
        cv::Mat descriptor_1_, descriptor_2_;
        cv::Ptr<cv::FeatureDetector> detector_ = cv::ORB::create();
        cv::Ptr<cv::DescriptorExtractor> descriptor_extractor_ = cv::ORB::create();
        cv::Ptr<cv::DescriptorMatcher> matcher_ = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);

    public:
        explicit BinaryFeatureExtractor(cv::Mat  image_1, cv::Mat  image_2): image_1_(std::move(image_1)), image_2_(std::move(image_2)) {};

        auto extract_features() -> cv::Mat {
            detector_->detect(image_1_, keypoints_1_);
            detector_->detect(image_2_, keypoints_2_);
            
            descriptor_extractor_->compute(image_1_, keypoints_1_, descriptor_1_);
            descriptor_extractor_->compute(image_2_, keypoints_2_, descriptor_2_);
            cv::Mat output_image:
            cv::drawKeypoints(image_1_, keypoints_1_, output_image, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
            return output_image;
        }
    };
}
#endif //VISUAL_ODOMETRY_INTRO_HPP
