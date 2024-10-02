cmake_minimum_required(VERSION 3.10) # Minimum version for CMake
project(EfficientPngLoading) # Project name

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV and zlib packages
find_package(OpenCV REQUIRED)
set(LIB ${OpenCV_LIBS} ${LIB})
set(INC ${OpenCV_INCLUDE_DIRS} ${INC})

find_package(ZLIB REQUIRED)
set(LIB ${ZLIB_LIBRARIES} ${LIB})

# SRC
set(SRC main.cpp ${SRC})
set(SRC EPL/parsing_chunks.cpp ${SRC})
set(SRC EPL/png_properties.cpp ${SRC})
set(INC EPL ${INC})

message(STATUS "SRC: " ${SRC})
message(STATUS "INC: " ${INC})
message(STATUS "LIB: " ${LIB})

# Add executable
add_executable(${PROJECT_NAME} ${SRC})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${LIB})

# Optionally, include OpenCV headers
target_include_directories(${PROJECT_NAME} PRIVATE ${INC})
