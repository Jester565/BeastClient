cmake_minimum_required ( VERSION 3.1 )
set (BT_PROJECT_NAME BeastClient)

project(${BT_PROJECT_NAME})

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /MT")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_GENERATOR_PLATFORM x64)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.66.0 COMPONENTS thread REQUIRED)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/beastclient)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADERS
	${HEADER_DIR}/Client.h
    ${HEADER_DIR}/Condvar.h
    ${HEADER_DIR}/EventManager.h
    ${HEADER_DIR}/HttpConnection.h
    ${HEADER_DIR}/Service.h
    ${HEADER_DIR}/TCPConnector.h
    ${HEADER_DIR}/Typedefs.h)

set(SOURCES
	${SOURCE_DIR}/Client.cpp
    ${SOURCE_DIR}/EventManager.cpp
    ${SOURCE_DIR}/HttpConnection.cpp
    ${SOURCE_DIR}/Service.cpp
    ${SOURCE_DIR}/Source.cpp
    ${SOURCE_DIR}/TCPConnector.cpp)

include_directories(
    ${HEADER_DIR})

include_directories(
    ${Boost_INCLUDE_DIRS})

add_library(${BT_PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${BT_PROJECT_NAME} 
    ${Boost_LIBRARIES})

set_property(TARGET ${BT_PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${BT_PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED 11)