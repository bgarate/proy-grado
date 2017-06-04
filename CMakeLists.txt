cmake_minimum_required(VERSION 3.5)
project(proy_grado)

set(CMAKE_MODULE_PATH cmake)

set(spdlog_DIR lib/cmake/spdlog)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED system program_options thread)
find_package(Protobuf REQUIRED)
find_package (Threads)
find_package(FFmpeg REQUIRED)

link_directories(src/hal/pb2/lib)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/tracking/TrackingAlgorithm.h)
set(SOURCE_FILES ${SOURCE_FILES} src/Config.cpp src/Config.h)
set(SOURCE_FILES ${SOURCE_FILES} src/logging/Logger.cpp src/logging/Logger.h)
set(SOURCE_FILES ${SOURCE_FILES} src/tracking/HogDetector.cpp src/tracking/HogDetector.h)
set(SOURCE_FILES ${SOURCE_FILES} src/tracking/MultiTracker.cpp src/tracking/MultiTracker.h)
set(SOURCE_FILES ${SOURCE_FILES} src/tracking/DetectAndTrack.cpp src/tracking/DetectAndTrack.h)
set(SOURCE_FILES ${SOURCE_FILES} src/tracking/Track.cpp src/tracking/Track.h)
set(SOURCE_FILES ${SOURCE_FILES} src/messages/SocketChannel.cpp src/messages/SocketChannel.h)
set(SOURCE_FILES ${SOURCE_FILES} src/Brain.cpp src/Brain.h)
set(SOURCE_FILES ${SOURCE_FILES} src/Body.cpp src/Body.h)
set(SOURCE_FILES ${SOURCE_FILES} src/bodytests/BodyTest.h src/bodytests/BodyTest1.cpp src/bodytests/BodyTest2.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/communication/Communication.cpp src/communication/Communication.h)
set(SOURCE_FILES ${SOURCE_FILES} src/messages/Connection.cpp src/messages/Connection.h)
set(SOURCE_FILES ${SOURCE_FILES} src/messages/AsyncSocketChannel.cpp src/messages/AsyncSocketChannel.h)
set(SOURCE_FILES ${SOURCE_FILES} src/messages/PackedMessage.h src/messages/MessageBuilder.cpp src/messages/MessageBuilder.h src/MessageHandler.cpp src/MessageHandler.h)
set(SOURCE_FILES ${SOURCE_FILES} src/hal/HalType.hpp)
set(SOURCE_FILES ${SOURCE_FILES} src/hal/hal.hpp)
set(SOURCE_FILES ${SOURCE_FILES} src/hal/vrep/extApi.c src/hal/vrep/extApi.h src/hal/vrep/extApiInternal.h src/hal/vrep/extApiPlatform.c src/hal/vrep/extApiPlatform.h src/hal/vrep/v_repConst.h src/hal/vrep/vrephal.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/hal/pb2/pb2hal.cpp)
set(SOURCE_FILES ${SOURCE_FILES} src/messages/Broadcaster.cpp src/messages/Broadcaster.h)
set(SOURCE_FILES ${SOURCE_FILES} src/messages/MessageQueue.cpp src/messages/MessageQueue.h src/main.cpp src/hal/dummyHal/dummyHal.cpp src/hal/dummyHal/dummyHal.h src/messages/IpResolver.cpp src/messages/IpResolver.h src/hal/pb2/VideoDecoder.cpp src/hal/pb2/VideoDecoder.h src/bodytests/FlightManeuver.cpp src/hal/pb2/CommandHandler.cpp src/hal/pb2/CommandHandler.h src/tracking/AreaRect.h src/hal/ManualControl.cpp src/hal/ManualControl.h src/VisualDebugger.cpp src/VisualDebugger.h src/tracking/Follower.cpp src/tracking/Follower.h)


ADD_SUBDIRECTORY(src/proto)
ADD_SUBDIRECTORY(lib/ORB_SLAM2)

include_directories(src/hal/pb2/include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(lib/spdlog/include)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${FFMPEG_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(proy_grado ${SOURCE_FILES} ${PROTO_SRC} ${PROTO_HEADER})

target_compile_options(proy_grado PUBLIC -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux)

target_link_libraries(proy_grado ${OpenCV_LIBS})
target_link_libraries(proy_grado ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(proy_grado ${Boost_LIBRARIES})
target_link_libraries(proy_grado ${Protobuf_LIBRARIES})
target_link_libraries(proy_grado proto)
target_link_libraries(proy_grado ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(proy_grado ${FFMPEG_LIBRARIES})
target_link_libraries(proy_grado libarcommands.so libarcontroller.so libardiscovery.so libarnetwork.so
        libarnetworkal.so libarsal.so libarstream.so libarstream2.so libarmavlink.so libjson.so
        libarmedia.so libarutils.so libcurl.so libardatatransfer.so libmux.so libpomp.so libcrypto.so
        libssl.so libz.so libncurses.so libtls.so libarupdater.so)

target_link_libraries(proy_grado ORB_SLAM2)

