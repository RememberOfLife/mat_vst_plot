cmake_minimum_required(VERSION 3.18)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 11)

project(vst_plot C CXX)

set(INCLUDES
    src
)

set(SOURCES
src/mode/result_readin_draw.cpp
    src/mode/result_writeout_database.cpp
    src/mode/sample_readin_draw.cpp
    src/mode/sample_readin_test.cpp
    src/mode/sample_writeout_test.cpp

    src/util.cpp
    src/visual_info.cpp

    src/main.cpp
)

add_executable(vst_plot "${SOURCES}")

find_package(MYSQL REQUIRED)
target_include_directories(vst_plot PRIVATE ${MYSQL_INCLUDE_DIRS})
target_link_libraries(vst_plot PRIVATE ${MYSQL_LIBRARIES})

find_package(Cairo)
target_include_directories(vst_plot PRIVATE ${CAIRO_INCLUDE_DIRS})
target_link_libraries(vst_plot PRIVATE ${CAIRO_LIBRARIES})

target_compile_options(vst_plot PRIVATE
    # "-Wall"
    # "-Wextra"
    # "-Werror" # warnings as errors
    "-Wfatal-errors" # stop after first error
)

target_link_libraries(vst_plot PRIVATE m)

target_include_directories(vst_plot PRIVATE ${INCLUDES})

set_target_properties(vst_plot PROPERTIES EXPORT_COMPILE_COMMANDS true)
