cmake_minimum_required(VERSION 3.14)
project(dining-philosophers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Curses REQUIRED)
find_package(Threads REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})

add_executable(${PROJECT_NAME}.exe main.cpp Fork.cpp Philosopher.cpp)
target_link_libraries(${PROJECT_NAME}.exe ${CURSES_LIBRARY})
target_link_libraries(${PROJECT_NAME}.exe Threads::Threads)