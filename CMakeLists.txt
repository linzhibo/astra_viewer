cmake_minimum_required(VERSION 2.8)
project(astra_viewer)

SET(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-DCOMPILEDWITHC11)
message(STATUS "Using flag -std=c++11.")

find_package(OpenCV 3.0 QUIET)
if(NOT OpenCV_FOUND)
   find_package(OpenCV 2.4.3 QUIET)
   if(NOT OpenCV_FOUND)
      message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
   endif()
endif()


set(OPENNI_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/OpenNI-Linux-x64-2.3/Include)
set(OPENNI_LIB_DIR  ${PROJECT_SOURCE_DIR}/OpenNI-Linux-x64-2.3/Redist)


include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
${OPENNI_INCLUDE_DIR}
)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(astra_viewer
src/main.cpp)
target_link_libraries(astra_viewer ${OpenCV_LIBS} ${OPENNI_LIB_DIR}/libOpenNI2.so)
