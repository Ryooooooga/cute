cmake_minimum_required(VERSION 3.21.0)

set(CMAKE_C_STANDARD "99")
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -pedantic")
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g3 -Og")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")

if (COVERAGE)
    add_compile_options(-fprofile-arcs)
    add_compile_options(-ftest-coverage)
endif (COVERAGE)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

project(cute LANGUAGES C)

find_library(LIBMATH m)

add_library(cute INTERFACE)
target_include_directories(cute INTERFACE .)

if(EXAMPLE)
    add_executable(cute_example example.c)
    target_link_libraries(cute_example cute ${LIBMATH})

    enable_testing()
    add_test(NAME cute::example COMMAND $<TARGET_FILE:cute_example>)
endif(EXAMPLE)
