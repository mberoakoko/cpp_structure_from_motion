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
        auto cost_functior = CostFunctor();
        ceres::CostFunction* cost_function =
            new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(&cost_functior);
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

    namespace playground_ceres {
        struct DistanceResidual {
            explicit DistanceResidual(const double measurement): measurement_(measurement) {}

            template<typename T>
            auto operator()(const T* const x, T* residual ) const -> bool {
                // here is where the magic happens
                residual[0] = x[0] - T(measurement_);
                return true;
            }
        private:
            const double measurement_;
        };

        auto solve_distance_residual(const double& observed_distance) -> void {
            double x_initail = 5;
            ceres::Problem problem;

            auto distance_residual = DistanceResidual(x_initail);

            ceres::CostFunction* cost_function = new ceres::AutoDiffCostFunction<DistanceResidual, 1, 1>(&distance_residual);

            problem.AddResidualBlock(cost_function, nullptr, &x_initail);

            ceres::Solver::Options options;
            options.minimizer_progress_to_stdout = true;
            options.linear_solver_type = ceres::DENSE_QR;

            ceres::Solver::Summary summary;
            ceres::Solve(options, &problem, &summary);
            std::cout << summary.BriefReport() << "\n";
            std::cout << "Final Value of X " << x_initail << "\n";
            delete cost_function;

        }

        inline auto solve_distance_residual_with_loss_function(const double& measurement) ->  void {
            double x_initail = 5;

            auto distance_residual = DistanceResidual(measurement);

            ceres::CostFunction* cost_function = new ceres::AutoDiffCostFunction<DistanceResidual, 1, 1>(&distance_residual);
            ceres::LossFunction* loss_function = new ceres::HuberLoss(1.0);

            ceres::Problem problem;
            problem.AddResidualBlock(cost_function, loss_function, &x_initail);

            ceres::Solver::Options options;
            options.minimizer_progress_to_stdout = true;
            options.linear_solver_type = ceres::DENSE_QR;

            ceres::Solver::Summary summary;
            ceres::Solve(options, &problem, &summary);
            std::cout << summary.BriefReport() << "\n";
            std::cout << "Final Value of X " << x_initail << "\n";
            // delete loss_function;
            // delete cost_function;

        }

    }
}

#endif //CERES_PLAYGROUNG_HPP
