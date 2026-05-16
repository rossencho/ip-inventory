cmake_minimum_required(VERSION 3.20)
project(ip_inventory VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

find_package(Drogon REQUIRED)

set(SOURCES
    src/main.cpp
    src/app/AppConfig.cpp
    src/app/Router.cpp

    src/controllers/IpPoolController.cpp
    src/controllers/ReserveIpController.cpp
    src/controllers/AssignIpController.cpp
    src/controllers/TerminateIpController.cpp
    src/controllers/ServiceIdChangeController.cpp
    src/controllers/ServiceIdQueryController.cpp

    src/services/IpPoolService.cpp
    src/services/ReserveIpService.cpp
    src/services/AssignIpService.cpp
    src/services/TerminateIpService.cpp
    src/services/ServiceIdChangeService.cpp
    src/services/ServiceIdQueryService.cpp
    src/services/ReservationExpiryJob.cpp

    src/repository/IpRepository.cpp
    src/utils/IpUtils.cpp
    src/utils/JsonValidator.cpp
)

add_executable(ip_inventory ${SOURCES})

target_include_directories(ip_inventory PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ip_inventory PRIVATE Drogon::Drogon)

install(TARGETS ip_inventory DESTINATION bin)
install(FILES config/config.json DESTINATION etc/ip_inventory)
