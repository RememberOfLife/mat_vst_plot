cmake_minimum_required(VERSION 3.18)
set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 11)

project(vst_plot C CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(INCLUDES
    src
)

set(SOURCES
    src/mode/readin_draw.cpp
    src/mode/readin_test.cpp
    src/mode/writeout_test.cpp

    src/util.cpp
    src/visual_info.cpp

    src/main.cpp
)

add_executable(vst_plot "${SOURCES}")

target_compile_options(vst_plot PRIVATE
    # "-Wall"
    # "-Wextra"
    # "-Werror" # warnings as errors
    "-Wfatal-errors" # stop after first error
)

target_include_directories(vst_plot PRIVATE ${INCLUDES})

target_link_libraries(vst_plot Threads::Threads)

set_target_properties(vst_plot PROPERTIES EXPORT_COMPILE_COMMANDS true)
