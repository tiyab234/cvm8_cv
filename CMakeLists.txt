cmake_minimum_required(VERSION 3.28)

project(cvm8_cv)

option(USE_NATIVE_INSTRUCTIONS "Optimize for host CPU at the cost of portability !" OFF)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED YES)

file(GLOB_RECURSE SOURCES source/**.c)
file(GLOB_RECURSE HEADERS include/**.h)

find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_mixer REQUIRED CONFIG)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE include)

if(APPLE)
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE /opt/homebrew/include)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_mixer::SDL2_mixer)

if(USE_NATIVE_INSTRUCTIONS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
endif()
