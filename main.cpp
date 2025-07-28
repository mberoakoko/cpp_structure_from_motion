#include <iostream>
#include "include/testing_out_stuff/sophus_tings.hpp"
#include "include/testing_out_stuff/ceres_playgroung.hpp"
#include "include/utils/trajectory_utils.hpp"
#include "include/utils/data_loader.hpp"



int main() {
    // motion::tests::test_initializing_sophus_objects();
    motion::utils::draw_trajectories_open_gl_context();
    // motion::tests::hello_world_ceres();
    // motion::utils::test_dataloader::test_getting_batches(200);
    return 0;
}