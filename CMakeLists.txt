cmake_minimum_required(VERSION 3.5)
project(dbus-demo LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost)
find_package(PkgConfig REQUIRED)

pkg_check_modules(Systemd IMPORTED_TARGET GLOBAL libsystemd>=236)

add_library(sdbus-cpp
    sdbus-cpp/src/Connection.cpp
    sdbus-cpp/src/Connection.h
    sdbus-cpp/src/Error.cpp
    sdbus-cpp/src/Flags.cpp
    sdbus-cpp/src/IConnection.h
    sdbus-cpp/src/ISdBus.h
    sdbus-cpp/src/Message.cpp
    sdbus-cpp/src/MessageUtils.h
    sdbus-cpp/src/Object.cpp
    sdbus-cpp/src/Object.h
    sdbus-cpp/src/Proxy.cpp
    sdbus-cpp/src/Proxy.h
    sdbus-cpp/src/ScopeGuard.h
    sdbus-cpp/src/SdBus.cpp
    sdbus-cpp/src/SdBus.h
    sdbus-cpp/src/Types.cpp
    sdbus-cpp/src/Utils.h
    sdbus-cpp/src/VTableUtils.c
    sdbus-cpp/src/VTableUtils.h
)

target_include_directories(sdbus-cpp PUBLIC sdbus-cpp/include)
target_link_libraries(sdbus-cpp PUBLIC PkgConfig::Systemd)

add_executable(service service.cpp service-glue.h)
target_include_directories(service PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(service PRIVATE sdbus-cpp)

add_executable(client client.cpp client-glue.h SdbusAsio.h)
target_include_directories(client PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(client PRIVATE ${Boost_LIBRARIES} sdbus-cpp)
