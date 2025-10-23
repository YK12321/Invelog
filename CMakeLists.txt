cmake_minimum_required(VERSION 3.15)
project(Invelog VERSION 1.0.0 LANGUAGES CXX C)

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
include_directories(${PROJECT_SOURCE_DIR}/external)
include_directories(${PROJECT_SOURCE_DIR}/server/include)

# Options
option(USE_SQLITE "Build with SQLite support" ON)
option(USE_POSTGRESQL "Build with PostgreSQL support" OFF)
option(USE_MYSQL "Build with MySQL support" OFF)

# Find packages
include(FetchContent)

# nlohmann/json - JSON library
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# cpp-httplib - HTTP library
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)
FetchContent_MakeAvailable(httplib)

# Google Test - Testing framework
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# SQLite3 (if enabled)
if(USE_SQLITE)
    find_package(SQLite3)
    if(NOT SQLite3_FOUND)
        message(STATUS "SQLite3 not found, will use embedded version")
        # Download SQLite amalgamation
        FetchContent_Declare(
            sqlite3
            URL https://www.sqlite.org/2024/sqlite-amalgamation-3450000.zip
        )
        FetchContent_MakeAvailable(sqlite3)
        add_library(sqlite3 STATIC 
            ${sqlite3_SOURCE_DIR}/sqlite3.c
        )
        target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})
        set_source_files_properties(${sqlite3_SOURCE_DIR}/sqlite3.c PROPERTIES LANGUAGE C)
        set(SQLite3_LIBRARIES sqlite3)
    endif()
    add_compile_definitions(USE_SQLITE)
endif()

# PostgreSQL (if enabled)
if(USE_POSTGRESQL)
    find_package(PostgreSQL REQUIRED)
    add_compile_definitions(USE_POSTGRESQL)
endif()

# MySQL (if enabled)
if(USE_MYSQL)
    find_package(MySQL REQUIRED)
    add_compile_definitions(USE_MYSQL)
endif()

# Platform-specific libraries
if(WIN32)
    set(PLATFORM_LIBS ws2_32 wsock32)
else()
    set(PLATFORM_LIBS pthread)
endif()

# Source files
set(SOURCES
    src/UUID.cpp
    src/Location.cpp
    src/Category.cpp
    src/Item.cpp
    src/Container.cpp
    src/ActivityLog.cpp
    src/Project.cpp
    src/LocalDatabase.cpp
    src/SQLDatabase.cpp
    src/APIDatabase.cpp
    # src/DatabaseServer.cpp  # DEPRECATED - Using modular server/src/DatabaseAPIServer.cpp instead
    src/InventoryManager.cpp
)

# Modular server source files
set(SERVER_SOURCES
    server/src/http/HTTPRequest.cpp
    server/src/http/HTTPResponse.cpp
    server/src/http/HTTPServer.cpp
    server/src/auth/Authenticator.cpp
    server/src/serialization/JSONSerializer.cpp
    server/src/serialization/JSONDeserializer.cpp
    server/src/routes/ItemRoutes.cpp
    server/src/routes/ContainerRoutes.cpp
    server/src/routes/LocationRoutes.cpp
    server/src/routes/ProjectRoutes.cpp
    server/src/routes/CategoryRoutes.cpp
    server/src/routes/ActivityLogRoutes.cpp
    server/src/DatabaseAPIServer.cpp
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
    include/SQLDatabase.h
    include/APIDatabase.h
    include/DatabaseServer.h
    include/InventoryManager.h
)

# Modular server header files
set(SERVER_HEADERS
    server/include/http/HTTPRequest.h
    server/include/http/HTTPResponse.h
    server/include/http/RouteHandler.h
    server/include/http/HTTPServer.h
    server/include/auth/Authenticator.h
    server/include/serialization/JSONSerializer.h
    server/include/serialization/JSONDeserializer.h
    server/include/routes/ItemRoutes.h
    server/include/routes/ContainerRoutes.h
    server/include/routes/LocationRoutes.h
    server/include/routes/ProjectRoutes.h
    server/include/routes/CategoryRoutes.h
    server/include/routes/ActivityLogRoutes.h
    server/include/ServerConfig.h
    server/include/DatabaseAPIServer.h
)

# Create library
add_library(invelog_lib STATIC ${SOURCES} ${HEADERS})
target_include_directories(invelog_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Create modular server library
add_library(invelog_server_lib STATIC ${SERVER_SOURCES} ${SERVER_HEADERS})
target_include_directories(invelog_server_lib PUBLIC 
    ${PROJECT_SOURCE_DIR}/server/include
    ${PROJECT_SOURCE_DIR}/include
)

# Link external libraries
target_link_libraries(invelog_lib PUBLIC 
    nlohmann_json::nlohmann_json
    httplib::httplib
    ${PLATFORM_LIBS}
)

# Link external libraries for server
target_link_libraries(invelog_server_lib PUBLIC 
    invelog_lib
    nlohmann_json::nlohmann_json
    httplib::httplib
    ${PLATFORM_LIBS}
)

# Add SQL libraries if enabled
if(USE_SQLITE)
    target_link_libraries(invelog_lib PUBLIC ${SQLite3_LIBRARIES})
    target_link_libraries(invelog_server_lib PUBLIC ${SQLite3_LIBRARIES})
endif()

if(USE_POSTGRESQL)
    target_link_libraries(invelog_lib PUBLIC PostgreSQL::PostgreSQL)
    target_link_libraries(invelog_server_lib PUBLIC PostgreSQL::PostgreSQL)
endif()

if(USE_MYSQL)
    target_link_libraries(invelog_lib PUBLIC ${MYSQL_LIBRARIES})
    target_include_directories(invelog_lib PUBLIC ${MYSQL_INCLUDE_DIRS})
    target_link_libraries(invelog_server_lib PUBLIC ${MYSQL_LIBRARIES})
    target_include_directories(invelog_server_lib PUBLIC ${MYSQL_INCLUDE_DIRS})
endif()

# Main executable (demo application)
add_executable(invelog src/main.cpp)
target_link_libraries(invelog invelog_lib)

# Database server executable
add_executable(invelog_server src/server_main.cpp)
target_link_libraries(invelog_server invelog_server_lib invelog_lib)

# Server test executable (tests API connectivity)
add_executable(invelog_server_test src/server_test.cpp)
target_link_libraries(invelog_server_test invelog_lib)

# Unit tests executable
add_executable(invelog_tests
    tests/test_uuid.cpp
    tests/test_entities.cpp
    tests/test_database.cpp
    tests/test_inventory_manager.cpp
)
target_link_libraries(invelog_tests 
    invelog_lib
    GTest::gtest_main
    GTest::gmock_main
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(invelog_tests)

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
