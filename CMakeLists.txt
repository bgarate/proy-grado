cmake_minimum_required(VERSION 3.6)
project(proy_grado)

find_package( OpenCV REQUIRED )

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/Tracker.cpp src/Tracker.h)
add_executable(proy_grado ${SOURCE_FILES})
target_link_libraries( proy_grado ${OpenCV_LIBS} )
