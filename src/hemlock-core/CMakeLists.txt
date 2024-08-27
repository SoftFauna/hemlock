# HEMLOCK - a SlackBuilds like package manager.
# <https://github.com/SoftFauna/HEMLOCK.git>
# Copyright (c) 2024 The SoftFauna Team

configure_file(config.h.in config.h)

find_package(SQLite3 REQUIRED)

add_executable(hemlock
        "main.c")
target_compile_features(hemlock c_std_11)
target_include_directories(hemlock
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${SQLite3_INCLUDE_DIRS}")
target_link_libraries(hemlock
        "${SQLite3_LIBRARIES}")

# end of file