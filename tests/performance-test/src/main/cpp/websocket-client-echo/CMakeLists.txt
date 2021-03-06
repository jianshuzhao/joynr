cmake_minimum_required(VERSION 2.8)

project(websocket-client-EchoString)

option(
    USE_PLATFORM_WEBSOCKETPP
    "Resolve dependency to WebSocket++ from the system?"
    ON
)
message(STATUS "option USE_PLATFORM_WEBSOCKETPP=" ${USE_PLATFORM_WEBSOCKETPP})

### Add websocketpp ###########################################################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../cpp/CMake")
include(AddWebSocketPP)
include(SetCppStandard)
include(AddClangFormat)

find_package(Boost REQUIRED COMPONENTS system thread program_options)
find_package(OpenSSL REQUIRED)
find_package (Threads)

add_executable(websocket-client-echo
    WebSocketClientEcho.cpp
)
add_dependencies(websocket-client-echo websocketpp)

target_include_directories(
    websocket-client-echo
    PUBLIC
        "$<BUILD_INTERFACE:${WEBSOCKETPP_INCLUDE_DIR}>"
)

target_link_libraries(
    websocket-client-echo
    ${OPENSSL_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
)

AddClangFormat(websocket-client-echo)
