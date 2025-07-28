# We gonna yoink cmake from git
function(fetch_ceres_solver)
    include(FetchContent)
    message(STATUS "Fetching Ceres-Solver and its dependencies...")
    FetchContent_Declare(
            glog
            GIT_REPOSITORY https://github.com/google/glog.git
            GIT_TAG        v0.7.0
            CMAKE_ARGS
            -DBUILD_SHARED_LIBS=OFF
            -DBUILD_TESTING=OFF
    )
    FetchContent_MakeAvailable(glog)

    FetchContent_Declare(
            gflags
            GIT_REPOSITORY https://github.com/gflags/gflags.git
            GIT_TAG        v2.2.2
            CMAKE_ARGS
            -DBUILD_SHARED_LIBS=OFF
            -DBUILD_TESTING=OFF
            -DBUILD_GFLAGS_LIBS_AS_SHARED=OFF
    )
    message(STATUS "Glog dir found \n ")
    message(STATUS ${glog_SOURCE_DIR})

    FetchContent_Declare(
            ceres-solver
            GIT_REPOSITORY https://ceres-solver.googlesource.com/ceres-solver
            GIT_TAG        2.2.0 # Pin to a specific stable version
            CMAKE_ARGS
            -DBUILD_SHARED_LIBS=OFF
            -DMINIGLOG=ON # Use Ceres's internal glog unless you need full glog features
            # Pass explicit paths to fetched dependencies
            -DGLOG_INCLUDE_DIR=${glog_SOURCE_DIR}/src/glog
            -DGFLAGS_INCLUDE_DIR=${gflags_SOURCE_DIR}/src
            -DEIGEN_INCLUDE_DIR=${eigen_SOURCE_DIR}
            -DBUILD_EXAMPLES=OFF
            -DBUILD_TESTING=OFF
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            # Ensure Ceres finds the built static libraries
            -DGLOG_LIBRARY_DIRS=${glog_BINARY_DIR}
            -DGFLAGS_LIBRARY_DIRS=${gflags_BINARY_DIR}
            # Add other Ceres options as needed (e.g., -DSUITESPARSE=OFF)
    )
    FetchContent_MakeAvailable(ceres-solver)
    message(STATUS "Ceres-Solver and dependencies configured via FetchContent.")

endfunction()