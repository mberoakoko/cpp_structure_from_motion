cmake_minimum_required(VERSION 3.22.1)
project(cpp_structure_from_motion)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_fetch_funcs")

include(FetchContent)

#include(FetchCeres)

## This often helps trick Ceres into linking correctly.
#if(NOT TARGET glog::glog)
#    MESSAGE(STATUS "Glog Target not found")
#    find_package(Glog REQUIRED) # Try to find the old-style Glog package
#    if(TARGET glog)
#        message(STATUS "Create the modern namespaced alias that Ceres expects")
#        add_library(glog::glog INTERFACE IMPORTED)
#        target_link_libraries(glog::glog INTERFACE glog)
#        message(STATUS "You may also need to set properties to include directories etc.")
#        message(STATUS "This can get complicated, so try the next simpler approach first.")
#    endif()
#endif()

find_package(Glog REQUIRED)

if(NOT TARGET glog::glog)
    # This assumes that 'find_package(Glog)' or simply having the header/library
    # installed is enough to define an existing simple target 'glog'
    add_library(glog::glog ALIAS glog)
endif()


find_package(Ceres CONFIG REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui sfm)
find_package(Pangolin CONFIG REQUIRED)

if (Sophus_FOUND)
    message(STATUS "Sophus is installed in system, chill a little bruh")
else ()
    message(STATUS "Yoh, we didn't find sophus in your system so we fetching it from github")
    FetchContent_Declare(
            Sophus
            GIT_REPOSITORY https://github.com/strasdat/Sophus
            GIT_TAG 1.22.10
    )

    FetchContent_MakeAvailable(sophus)
endif ()



# You can check if it was found and print its details (optional, for debugging)
if (Pangolin_FOUND)
    message(STATUS "Found Pangolin version: ${Pangolin_VERSION}")
    message(STATUS "Pangolin include directories: ${Pangolin_INCLUDE_DIRS}")
    message(STATUS "Pangolin libraries: ${Pangolin_LIBRARIES}")
else()
    message(FATAL_ERROR "Pangolin not found!")
endif()


if(OpenCV_FOUND)
    message(STATUS "\\033[31;1;4mFound OpenCV version ${OpenCV_VERSION}\\033[0m")
    message(STATUS "    \\033[31;1;4mInclude Path ${OpenCV_INCLUDE_DIRS}\\033[0m")
else ()
    message(STATUS "-------------OPENCV WAS NOT FOUND PANIC !!----------------")
endif ()


add_executable(cpp_structure_from_motion main.cpp
        include/testing_out_stuff/sophus_tings.hpp
        include/utils/pprint_utils.hpp
        include/utils/trajectory_utils.hpp
        include/testing_out_stuff/ceres_playgroung.hpp
        include/visual_odometry/visual_odometry_intro.hpp
        include/utils/data_loader.hpp)


target_include_directories(cpp_structure_from_motion PUBLIC
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(cpp_structure_from_motion
        PUBLIC sophus pango_core pango_display pango_geometry pango_glgeometry
        PRIVATE Ceres::ceres {OpenCV_LIBS}
)

#target_include_directories(cpp_structure_from_motion PUBLIC ${CERES_INCLUDE_DIRS})