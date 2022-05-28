# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME lwcgl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_BUILD_TYPE Debug)

project(${PROJECT_NAME})

include_directories("./dep")
include_directories("~/Downloads/glfw-3.3.7.bin.MACOS/include")
file(GLOB_RECURSE source_files src/*.cpp)


add_executable(${PROJECT_NAME} ${source_files})
target_link_libraries(${PROJECT_NAME} "/Users/james/Downloads/glfw-3.3.7.bin.MACOS/lib-arm64/libglfw3.a")
target_link_libraries(${PROJECT_NAME} "-framework cocoa")
target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
target_link_libraries(${PROJECT_NAME} "-framework IOKit")
target_link_libraries(${PROJECT_NAME} "-framework CoreVideo")