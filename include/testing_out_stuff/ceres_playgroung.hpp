//
// Created by mbero on 20/07/2025.
//

#ifndef CERES_PLAYGROUNG_HPP
#define CERES_PLAYGROUNG_HPP

#include <bits/stdc++.h>
#include "ceres/ceres.h"


namespace motion::tests {
    struct CostFunctor {
        template <typename T>
        bool operator()(const T* const x, T* residual) const {
            residual[0] = 10.0 - x[0];
            return true;
        }
    };
    inline auto hello_world_ceres() -> void {
        double x = 0.5;
        const double initial_x = x;
        ceres::Problem problem;

        // Set up the only cost function (also known as residual). This uses
        // auto-differentiation to obtain the derivative (jacobian).
        ceres::CostFunction* cost_function =
            new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>();
        problem.AddResidualBlock(cost_function, nullptr, &x);

        // Run the solver!
        ceres::Solver::Options options;
        options.minimizer_progress_to_stdout = true;
        ceres::Solver::Summary summary;
        ceres::Solve(options, &problem, &summary);

        std::cout << summary.BriefReport() << "\n";
        std::cout << "x : " << initial_x << " -> " << x << "\n";
        delete cost_function;
    }
}

#endif //CERES_PLAYGROUNG_HPP
