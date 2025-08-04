#include <iostream>
#include <bits/stdc++.h>
#include "include/testing_out_stuff/sophus_tings.hpp"
#include "include/testing_out_stuff/ceres_playgroung.hpp"
#include "include/utils/trajectory_utils.hpp"
#include "include/utils/data_loader.hpp"
#include "include/visual_odometry/visual_odometry_intro.hpp"

namespace functional {
    auto parse_urls(std::string url) -> std::optional<std::vector<std::string>>;
    auto get_valid_tokens(std::vector<std::string> parsed_url) -> std::optional<std::vector<std::string>>;
    auto get_result = [] (std::string url) {
        return parse_urls(url)
                .and_then(get_valid_tokens);
    };
}

int main() {
    // motion::tests::test_initializing_sophus_objects();
    // motion::utils::draw_trajectories_open_gl_context();
    // motion::tests::hello_world_ceres();
    // motion::utils::test_dataloader::test_getting_batches(200);
    visual_odometry::feature_extraction::test_binary_feature_extractor();
    return 0;
}