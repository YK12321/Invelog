cmake_minimum_required(VERSION 3.15)
project(Invelog VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/UUID.cpp
    # Add more source files as they are implemented
)

# Header files
set(HEADERS
    include/UUID.h
    include/Location.h
    include/Category.h
    include/Item.h
    include/Container.h
    include/ActivityLog.h
    include/Project.h
    include/Database.h
    include/LocalDatabase.h
    include/InventoryManager.h
)

# Create library
add_library(invelog_lib STATIC ${SOURCES} ${HEADERS})
target_include_directories(invelog_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Main executable (when you're ready to create main.cpp)
# add_executable(invelog src/main.cpp)
# target_link_libraries(invelog invelog_lib)

# Compiler warnings
if(MSVC)
    target_compile_options(invelog_lib PRIVATE /W4)
else()
    target_compile_options(invelog_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable testing
enable_testing()

# Print configuration
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
