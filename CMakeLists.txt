cmake_minimum_required(VERSION 3.22.1)
project(cpp_structure_from_motion)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)


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

find_package(Pangolin CONFIG REQUIRED)

# You can check if it was found and print its details (optional, for debugging)
if (Pangolin_FOUND)
    message(STATUS "Found Pangolin version: ${Pangolin_VERSION}")
    message(STATUS "Pangolin include directories: ${Pangolin_INCLUDE_DIRS}")
    message(STATUS "Pangolin libraries: ${Pangolin_LIBRARIES}")
else()
    message(FATAL_ERROR "Pangolin not found!")
endif()


add_executable(cpp_structure_from_motion main.cpp
        include/testing_out_stuff/sophus_tings.hpp
        include/utils/pprint_utils.hpp
        include/utils/trajectory_utils.hpp)

target_link_libraries(cpp_structure_from_motion PUBLIC sophus pango_core pango_display pango_geometry pango_glgeometry)