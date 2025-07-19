#include <iostream>
#include "include/testing_out_stuff/sophus_tings.hpp"
#include "include/utils/trajectory_utils.hpp"



int main() {
    motion::tests::test_initializing_sophus_objects();
    motion::utils::draw_trajectories_open_gl_context();
    return 0;
}