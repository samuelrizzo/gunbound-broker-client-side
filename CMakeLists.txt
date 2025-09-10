cmake_minimum_required(VERSION 3.20)

project(GunBoundBrokerClient LANGUAGES CXX)

# Windows-only
if (NOT WIN32)
  message(FATAL_ERROR "This project targets Windows only.")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force single-config Release for MSVC solutions
if (MSVC)
  set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)
endif()

add_library(GunBoundBrokerClient SHARED
    src/main.cpp
    src/tcp_server.cpp
    src/helpers/logs.cpp
    src/helpers/endian.cpp
    src/protocol/packet.cpp
    src/protocol/handler.cpp
)

target_include_directories(GunBoundBrokerClient PUBLIC include)

target_link_libraries(GunBoundBrokerClient PRIVATE ws2_32)

# Nome do binário (DLL)
set_target_properties(GunBoundBrokerClient PROPERTIES OUTPUT_NAME "broker")
