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



add_executable(cpp_structure_from_motion main.cpp
        include/testing_out_stuff/sophus_tings.hpp
        include/utils/pprint_utils.hpp)

target_link_libraries(cpp_structure_from_motion PUBLIC sophus)