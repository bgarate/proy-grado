cmake_minimum_required(VERSION 3.5)
project(proy_grado)

#set(CMAKE_BUILD_TYPE Debug)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" cmake )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Ubicación librería de logging spdlog

set(spdlog_DIR lib/cmake/spdlog)

# Paquetes

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED system program_options thread)
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)
find_package(FFmpeg REQUIRED)
find_package(Cairo REQUIRED)

find_package(Eigen3 REQUIRED)
find_package(Pangolin REQUIRED)

# Ubicación librerías adicionales

link_directories(src/hal/pb2/lib)
link_directories(lib/yaml-cpp/)

set(CMAKE_CXX_STANDARD 11)

# Src

set(SOURCE_FILES src/proto/message.pb.h src/proto/message.pb.cc
        src/tracking/TrackingAlgorithm.h
        src/config/ConfigDefaults.cpp src/config/ConfigDefaults.h
        src/config/Config.h src/config/ConfigKeys.h src/config/ConfigKeys.cpp
        src/tracking/Follower.cpp src/tracking/Follower.h
        src/logging/Logger.cpp src/logging/Logger.h
        src/tracking/HogDetector.cpp src/tracking/HogDetector.h
        src/tracking/CascadeDetector.cpp src/tracking/CascadeDetector.h
        src/tracking/MultiTracker.cpp src/tracking/MultiTracker.h
        src/tracking/DetectAndTrack.cpp src/tracking/DetectAndTrack.h
        src/tracking/Track.cpp src/tracking/Track.h
        src/messages/SocketChannel.cpp src/messages/SocketChannel.h
        src/Brain.cpp src/Brain.h
        src/Body.cpp src/Body.h
        src/bodytests/BodyTestRegistry.cpp src/bodytests/BodyTestRegistry.h
        src/bodytests/BodyTest.h src/bodytests/BodyTest1.cpp src/bodytests/BodyTest2.cpp
        src/communication/Communication.cpp src/communication/Communication.h
        src/messages/Connection.cpp src/messages/Connection.h
        src/messages/AsyncSocketChannel.cpp src/messages/AsyncSocketChannel.h
        src/messages/PackedMessage.h
        src/messages/MessageBuilder.cpp src/messages/MessageBuilder.h src/messages/MessageHandler.cpp src/messages/MessageHandler.h
        src/hal/HalType.hpp
        src/hal/hal.hpp
        src/hal/vrep/extApi.c src/hal/vrep/extApi.h src/hal/vrep/extApiInternal.h
        src/hal/vrep/extApiPlatform.c src/hal/vrep/extApiPlatform.h src/hal/vrep/v_repConst.h src/hal/vrep/vrephal.cpp
        src/hal/pb2/pb2hal.cpp
        src/messages/Broadcaster.cpp src/messages/Broadcaster.h
        src/bodytests/FlightManeuver.cpp src/bodytests/Follow.cpp src/bodytests/PatrolAndFollow.cpp
        src/messages/MessageQueue.cpp src/messages/MessageQueue.h src/hal/dummyHal/dummyHal.cpp src/hal/dummyHal/dummyHal.h
        src/messages/IpResolver.cpp src/messages/IpResolver.h src/hal/pb2/VideoDecoder.cpp src/hal/pb2/VideoDecoder.h
        src/hal/pb2/CommandHandler.cpp src/hal/pb2/CommandHandler.h src/tracking/AreaRect.h src/hal/ManualControl.cpp
        src/hal/ManualControl.h src/debugging/VisualDebugger.cpp src/debugging/VisualDebugger.h
        src/bodytests/OpticalFlowObstacleAvoidance.cpp
        src/landtracking/MarkerLand.h src/landtracking/MarkerLand.cpp
        src/bodytests/TrackMarkers.cpp src/navigation/MarkerTracker.cpp src/navigation/MarkerTracker.h
        src/navigation/World.cpp src/navigation/World.h src/navigation/NavigationDebugger.cpp src/navigation/NavigationDebugger.h
        src/landtracking/MarkerTrack.cpp src/landtracking/MarkerTrack.h
        src/navigation/MarkerFollower.cpp src/navigation/MarkerFollower.h
        src/main.cpp src/tracking/OpticalFlow.cpp src/tracking/OpticalFlow.h src/tracking/DbScan.cpp src/tracking/DbScan.h src/utils/Helpers.h
        src/landtracking/MarkerTrack.cpp src/landtracking/MarkerTrack.h
        src/landtracking/MarkerLand.cpp src/landtracking/MarkerLand.h src/navigation/Path.cpp src/navigation/Path.h)

set(LANDTRACKING_MAIN src/landtracking/landtrack.cpp src/landtracking/MarkerTrack.cpp src/landtracking/MarkerTrack.h
        src/landtracking/MarkerLand.cpp src/landtracking/MarkerLand.h)
set(CALIBRATION_MAIN src/calibration.cpp)
set(MARKER_MAIN src/navigation/MarkerTracker.cpp src/navigation/MarkerTracker.h src/marker.cpp)

# Otros proyectos a compilar

ADD_SUBDIRECTORY(src/proto)
ADD_SUBDIRECTORY(lib/yaml-cpp)

# Ubicacion includes

include_directories(src/hal/pb2/include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(lib/spdlog/include)
include_directories(lib/yaml-cpp/include)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${FFMPEG_INCLUDE_DIR})
include_directories(${FFMPEG_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(
        ${EIGEN3_INCLUDE_DIR}
        ${Pangolin_INCLUDE_DIRS}
)
include_directories(${CAIRO_INCLUDE_DIRS})

# Ejecutables generables

add_executable(proy_grado ${SOURCE_FILES})
add_executable(marker_generator ${MARKER_MAIN})
add_executable(calibration ${CALIBRATION_MAIN})
add_executable(landtracking ${LANDTRACKING_MAIN})

# Flags de compilación

target_compile_options(proy_grado PUBLIC -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux)
target_compile_options(calibration PUBLIC -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux)
target_compile_options(landtracking PUBLIC -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux)

# Librerias a linkear

target_link_libraries(proy_grado ${OpenCV_LIBS})
target_link_libraries(proy_grado ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(proy_grado ${Boost_LIBRARIES})
target_link_libraries(proy_grado ${Protobuf_LIBRARIES})
target_link_libraries(proy_grado proto)
target_link_libraries(proy_grado ${FFMPEG_LIBRARIES})
target_link_libraries(proy_grado ${CAIRO_LIBRARIES})
target_link_libraries(proy_grado yaml-cpp)
target_link_libraries(proy_grado ${EIGEN3_LIBS} ${Pangolin_LIBRARIES})
target_link_libraries(proy_grado libarcommands.so libarcontroller.so libardiscovery.so libarnetwork.so
        libarnetworkal.so libarsal.so libarstream.so libarstream2.so libarmavlink.so libjson.so
        libarmedia.so libarutils.so libcurl.so libardatatransfer.so libmux.so libpomp.so libcrypto.so
        libssl.so libz.so libncurses.so libtls.so libarupdater.so)

target_link_libraries(calibration ${OpenCV_LIBS})
target_link_libraries(calibration ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(calibration ${Boost_LIBRARIES})
target_link_libraries(calibration ${FFMPEG_LIBRARIES})

target_link_libraries(landtracking ${OpenCV_LIBS})

target_link_libraries(marker_generator ${OpenCV_LIBS})
target_link_libraries(marker_generator ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(marker_generator ${Boost_LIBRARIES})
