#include <iostream>
#include <bits/stdc++.h>
#include "include/testing_out_stuff/sophus_tings.hpp"
#include "include/testing_out_stuff/ceres_playgroung.hpp"
#include "include/utils/trajectory_utils.hpp"
#include "include/utils/data_loader.hpp"
#include "include/visual_odometry/visual_odometry_intro.hpp"
#include "include/utils/sterio_vision_basics.hpp"

#include <pangolin/display/display.h>
#include <pangolin/plot/plotter.h>

namespace plotter {
    auto plot_example() -> int {
        // Create OpenGL window in single line
        pangolin::CreateWindowAndBind("Main",640,480);

        // Data logger object
        pangolin::DataLog log;

        // Optionally add named labels
        std::vector<std::string> labels;
        labels.emplace_back("sin(t)");
        labels.emplace_back("cos(t)");
        labels.emplace_back("sin(t)+cos(t)");
        log.SetLabels(labels);

        const float tinc = 0.01f;

        // OpenGL 'view' of data. We might have many views of the same data.
        pangolin::Plotter plotter(&log,0.0f,4.0f*(float)M_PI/tinc,-2.0f,2.0f,(float)M_PI/(4.0f*tinc),0.5f);
        plotter.SetBounds(0.0, 1.0, 0.0, 1.0);
        plotter.Track("$i");

        // Add some sample annotations to the plot
        plotter.AddMarker(pangolin::Marker::Vertical,   -1000, pangolin::Marker::LessThan, pangolin::Colour::Blue().WithAlpha(0.2f) );
        plotter.AddMarker(pangolin::Marker::Horizontal,   100, pangolin::Marker::GreaterThan, pangolin::Colour::Red().WithAlpha(0.2f) );
        plotter.AddMarker(pangolin::Marker::Horizontal,    10, pangolin::Marker::Equal, pangolin::Colour::Green().WithAlpha(0.2f) );

        pangolin::DisplayBase().AddDisplay(plotter);

        float t = 0;

        // Default hooks for exiting (Esc) and fullscreen (tab).
        while( !pangolin::ShouldQuit() )
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            log.Log(sin(t),cos(t),sin(t)+cos(t));
            t += tinc;

            // Render graph, Swap frames and Process Events
            pangolin::FinishFrame();
        }

        return 0;
    }
}

namespace functional {
    auto parse_urls(std::string url) -> std::optional<std::vector<std::string>>;
    auto get_valid_tokens(std::vector<std::string> parsed_url) -> std::optional<std::vector<std::string>>;
    auto get_result = [] (std::string url) {
        return parse_urls(url)
                .and_then(get_valid_tokens);
    };
}

namespace to_be_deleted {
    inline auto test_generation() -> void {

    }
}

int main() {
    // motion::tests::test_initializing_sophus_objects();
    // motion::utils::draw_trajectories_open_gl_context();
    // motion::tests::playground_ceres::solve_distance_residual(10);
    // motion::tests::playground_ceres::solve_distance_residual_with_loss_function(10);
    // motion::tests::playground_ceres::run_calibration_residual();
    // motion::tests::playground_ceres::run_numeric_diff();
    // motion::utils::test_dataloader::test_getting_batches(200);
    // visual_odometry::feature_extraction::test_binary_feature_extractor();
    // utils::tutorial::point_cloud_pipeline();
    const auto result = plotter::plot_example();
    return 0;
}