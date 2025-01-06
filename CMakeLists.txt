cmake_minimum_required(VERSION 3.15)
project(AirPiano)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable
add_executable(AirPiano src/main.cpp src/findFingers.cpp)
# Add the convolution executable
add_executable(findFingersTest src/findFingersTest.cpp)

# Link OpenCV libraries
target_link_libraries(AirPiano ${OpenCV_LIBS})
target_link_libraries(findFingersTest ${OpenCV_LIBS})
