cmake_minimum_required(VERSION 3.10)
project(gps-tracking-sim)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find all source files in src/
file(GLOB SOURCES "src/*.cpp")

# Create the executable with the project name
add_executable(${PROJECT_NAME} ${SOURCES})

# Use CMake's built-in variables to handle build types
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/Release"
)

# Set common properties for both build types
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_NAME_DEBUG "outDebug"
    RUNTIME_OUTPUT_NAME_RELEASE "gps-tracking-sim"
)

# Add Proj library
find_package(PROJ REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE PROJ::proj)