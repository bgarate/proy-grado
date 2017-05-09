cmake_minimum_required(VERSION 3.5)
project(proy_grado)

set(spdlog_DIR lib/cmake/spdlog)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED system program_options)
find_package(Protobuf REQUIRED)
find_package (Threads)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES src/main.cpp src/tracking/TrackingAlgorithm.h src/tracking/HogDetector.cpp src/tracking/HogDetector.h src/tracking/MultiTracker.cpp src/tracking/MultiTracker.h src/tracking/DetectAndTrack.cpp src/tracking/DetectAndTrack.h src/logging/Logger.cpp src/logging/Logger.h src/tracking/Track.cpp src/tracking/Track.h src/Brain.cpp src/Brain.h src/Body.cpp src/Body.h src/messages/SocketChannel.cpp src/messages/SocketChannel.h src/messages/PackedMessage.h src/messages/MessageBuilder.cpp src/messages/MessageBuilder.h src/MessageHandler.cpp src/MessageHandler.h src/hal/hal.hpp src/hal/vrep/extApi.c src/hal/vrep/extApi.h src/hal/vrep/extApiInternal.h src/hal/vrep/extApiPlatform.c src/hal/vrep/extApiPlatform.h src/hal/vrep/v_repConst.h src/hal/vrep/vrephal.cpp src/communication/Communication.cpp src/communication/Communication.h src/messages/Broadcaster.cpp src/messages/Broadcaster.h src/bodytests/BodyTest.h src/bodytests/BodyTest1.cpp src/bodytests/BodyTest2.cpp)

ADD_SUBDIRECTORY(src/proto)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(lib/spdlog/include)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(proy_grado ${SOURCE_FILES} ${PROTO_SRC} ${PROTO_HEADER})

target_compile_options(proy_grado PUBLIC -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux)

target_link_libraries(proy_grado ${OpenCV_LIBS})
target_link_libraries(proy_grado ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(proy_grado ${Boost_LIBRARIES})
target_link_libraries(proy_grado ${Protobuf_LIBRARIES})
target_link_libraries(proy_grado proto)
target_link_libraries(proy_grado ${CMAKE_THREAD_LIBS_INIT})


