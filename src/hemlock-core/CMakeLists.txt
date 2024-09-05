# HEMLOCK - a SlackBuilds like package manager.
# <https://github.com/SoftFauna/HEMLOCK.git>
# Copyright (c) 2024 The SoftFauna Team

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(SQLite3 REQUIRED)

configure_file(config.h.in config.h)

add_executable(hemlock
        "main.c"
        "string_utils.c"
        "database_core.c"
        "database.c")
target_compile_features(hemlock PUBLIC c_std_11)
target_include_directories(hemlock PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${SQLITE3_INCLUDE_DIRS}")
target_link_libraries(hemlock
        "${SQLITE3_LIBRARIES}")

# end of file