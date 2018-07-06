cmake_minimum_required(VERSION 2.8)
project(astra_viewer)

SET(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-DCOMPILEDWITHC11)
message(STATUS "Using flag -std=c++11.")

find_package(CUDA REQUIRED)
find_package(OpenCV 3.0 QUIET)
if(NOT OpenCV_FOUND)
   find_package(OpenCV 2.4.3 QUIET)
   if(NOT OpenCV_FOUND)
      message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
   endif()
endif()


set(OPENNI_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/OpenNI-Linux-x64-2.3/Include)
set(OPENNI_LIB_DIR  ${PROJECT_SOURCE_DIR}/OpenNI-Linux-x64-2.3/Redist)

add_subdirectory(ORUtils)
set(GSLICR_LIB
gSLICr_Lib/engines/gSLICr_core_engine.h
gSLICr_Lib/engines/gSLICr_seg_engine.h
gSLICr_Lib/engines/gSLICr_seg_engine_GPU.h
gSLICr_Lib/engines/gSLICr_seg_engine_shared.h
gSLICr_Lib/engines/gSLICr_core_engine.cpp
gSLICr_Lib/engines/gSLICr_seg_engine.cpp
gSLICr_Lib/engines/gSLICr_seg_engine_GPU.cu
gSLICr_Lib/objects/gSLICr_settings.h
gSLICr_Lib/objects/gSLICr_spixel_info.h
gSLICr_Lib/gSLICr_defines.h
gSLICr_Lib/gSLICr.h
)



include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
${PROJECT_SOURCE_DIR}/src
${OPENNI_INCLUDE_DIR}
${CUDA_INCLUDE_DIRS}
${OpenCV_INCLUDE_DIRS}
)

SOURCE_GROUP(engines FILES ${GSLICR_LIB})
cuda_add_library(gSLICr_lib
			${GSLICR_LIB}
			src/NVTimer.h
			OPTIONS -gencode arch=compute_30,code=compute_30)
target_link_libraries(gSLICr_lib ${CUDA_LIBRARY})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(astra_viewer
src/main.cpp)
target_link_libraries(astra_viewer ${OpenCV_LIBS} ${OPENNI_LIB_DIR}/libOpenNI2.so)

add_executable(meanshift_seg
src/meanshift_seg.cpp)
target_link_libraries(meanshift_seg ${OpenCV_LIBS} ${OPENNI_LIB_DIR}/libOpenNI2.so)

add_executable(seg
src/seg.cpp)
target_link_libraries(seg ${OpenCV_LIBS} ${OPENNI_LIB_DIR}/libOpenNI2.so)

add_executable(astra_sp
src/astra_superpixel.cpp)
target_link_libraries(astra_sp gSLICr_lib ${OpenCV_LIBS} ${OPENNI_LIB_DIR}/libOpenNI2.so)
