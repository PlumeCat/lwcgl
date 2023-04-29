# CMakeLists.txt

cmake_minimum_required (VERSION 3.23)

project ("tge")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include_directories(tge "..//jlib")
include_directories(tge ".//dep")
include_directories(tge "..//glfw//include")
include_directories(tge "..//stb")
include_directories(tge "..//irrKlang-64bit-1.6.0//include")
include_directories(tge "..//tack//src//")

file(GLOB_RECURSE source_files src/*.cpp src/**/*.cpp)
message("Source files: ${source_files}")
add_executable (tge ${source_files})
target_link_libraries(tge "${CMAKE_SOURCE_DIR}//..//glfw//out//build//x64-${CMAKE_BUILD_TYPE}//src//glfw3.lib")
target_link_libraries(tge "${CMAKE_SOURCE_DIR}//..//tack//build//${CMAKE_BUILD_TYPE}//tack.lib")
target_link_libraries(tge "${CMAKE_SOURCE_DIR}//..//irrKlang-64bit-1.6.0//lib//Winx64-visualStudio//irrKlang.lib")

# packfont executable
add_executable(packfont packfont/main.cpp)

# fonts
function(add_font font_name size)
	# usage: packfont <font-name> <size> <output-name> [ <font-index> [ <oversampling> ] ]
	set(output_name "${PROJECT_SOURCE_DIR}//resource//font//${font_name}-${size}")
	message("Custom font: ${output_name}")
	add_custom_command(TARGET tge COMMAND packfont ${font_name} ${size} ${output_name})
endfunction()

add_font("Arial" 40)
add_font("Sylfaen" 30)
add_font("Verdana" 40)
add_font("Verdana" 20)
