cmake_minimum_required(VERSION 3.2)
project(ComputationalGeometry)

#find_package( OpenCV REQUIRED )

set(CMAKE_PREFIX_PATH "/usr/local/opt/opencv3/share/OpenCV")

set(OpenCV_INCLUDE_DIRS "/usr/local/opt/opencv3/include")
set(OpenCV_LIBS "/usr/local/opt/opencv3/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( OpenCV REQUIRED )

file(GLOB SOURCE_FILES
    include/*.hpp
    include/*.h
    include/Shape/*.hpp
    src/*.cpp)

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( include )

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# Link your application with OpenCV libraries
add_executable(ComputationalGeometry ${SOURCE_FILES} include/geo_persistent_rbtree.h)
TARGET_LINK_LIBRARIES(ComputationalGeometry ${OpenCV_LIBS} )