cmake_minimum_required(VERSION 3.5)
project(proy_grado)

find_package( OpenCV REQUIRED )
find_package(Threads)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/tracking/TrackingAlgorithm.h src/tracking/HogDetector.cpp src/tracking/HogDetector.h src/tracking/MultiTracker.cpp src/tracking/MultiTracker.h src/tracking/DetectAndTrack.cpp src/tracking/DetectAndTrack.h src/logging/Logger.cpp src/logging/Logger.h src/tracking/Track.cpp src/tracking/Track.h)
add_executable(proy_grado ${SOURCE_FILES})
target_link_libraries( proy_grado ${OpenCV_LIBS} )
target_link_libraries (proy_grado ${CMAKE_THREAD_LIBS_INIT})
