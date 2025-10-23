# External Dependencies Guide

## Overview

Invelog uses modern C++ libraries to provide production-ready functionality for JSON processing, HTTP communication, and database access.

## Required Dependencies

### 1. nlohmann/json (Included via CMake)

**Purpose**: JSON serialization and deserialization  
**Version**: 3.11.3  
**Type**: Header-only library  
**License**: MIT  
**Auto-downloaded**: Yes

**Usage Example**:
```cpp
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Serialize
json j;
j["name"] = "Resistor";
j["quantity"] = 100;
std::string jsonStr = j.dump(); // {"name":"Resistor","quantity":100}

// Deserialize
json parsed = json::parse(jsonStr);
std::string name = parsed["name"].get<std::string>();
int quantity = parsed["quantity"].get<int>();
```

**Documentation**: https://json.nlohmann.me/

### 2. cpp-httplib (Included via CMake)

**Purpose**: HTTP server and client  
**Version**: 0.15.3  
**Type**: Header-only library  
**License**: MIT  
**Auto-downloaded**: Yes

**Server Example**:
```cpp
#include <httplib.h>

httplib::Server svr;

svr.Get("/hello", [](const httplib::Request& req, httplib::Response& res) {
    res.set_content("Hello World!", "text/plain");
});

svr.listen("0.0.0.0", 8080);
```

**Client Example**:
```cpp
#include <httplib.h>

httplib::Client cli("http://localhost", 8080);

auto res = cli.Get("/api/items");
if (res && res->status == 200) {
    std::cout << res->body << std::endl;
}
```

**Documentation**: https://github.com/yhirose/cpp-httplib

### 3. SQLite3 (Optional, Auto-downloaded)

**Purpose**: Embedded SQL database  
**Version**: 3.45.0  
**Type**: C library (amalgamation)  
**License**: Public Domain  
**Auto-downloaded**: Yes (if not found on system)

**Enable**: `-DUSE_SQLITE=ON` (default)

**Usage Example**:
```cpp
#include <sqlite3.h>

sqlite3* db;
sqlite3_open("inventory.db", &db);

// Execute query
const char* sql = "SELECT * FROM items";
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

while (sqlite3_step(stmt) == SQLITE_ROW) {
    // Process row
}

sqlite3_finalize(stmt);
sqlite3_close(db);
```

### 4. PostgreSQL (Optional, System Install Required)

**Purpose**: Enterprise-grade SQL database client  
**Version**: libpq 15+  
**Type**: C library  
**License**: PostgreSQL License  
**Auto-downloaded**: No

**Enable**: `-DUSE_POSTGRESQL=ON`

**Installation**:
```bash
# Windows (using vcpkg)
vcpkg install libpq

# Ubuntu/Debian
sudo apt-get install libpq-dev

# macOS
brew install postgresql
```

### 5. MySQL (Optional, System Install Required)

**Purpose**: MySQL/MariaDB database client  
**Version**: 8.0+  
**Type**: C++ library  
**License**: GPL/Commercial  
**Auto-downloaded**: No

**Enable**: `-DUSE_MYSQL=ON`

**Installation**:
```bash
# Windows (using vcpkg)
vcpkg install libmysql

# Ubuntu/Debian
sudo apt-get install libmysqlclient-dev

# macOS
brew install mysql-client
```

## CMake Configuration

### Default Build (SQLite only)

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### With PostgreSQL

```bash
cmake .. -DUSE_POSTGRESQL=ON -DUSE_SQLITE=OFF
cmake --build . --config Release
```

### With Multiple Databases

```bash
cmake .. -DUSE_SQLITE=ON -DUSE_POSTGRESQL=ON
cmake --build . --config Release
```

### Without SQL Support

```bash
cmake .. -DUSE_SQLITE=OFF
cmake --build . --config Release
```

## Platform-Specific Libraries

### Windows

Automatically linked:
- `ws2_32.dll` - Windows Sockets API
- `wsock32.dll` - Legacy Windows Sockets

### Linux/macOS

Automatically linked:
- `pthread` - POSIX threads

## Dependency Status

| Library | Status | Auto-Download | Manual Install |
|---------|--------|---------------|----------------|
| nlohmann/json | ✅ Integrated | Yes | No |
| cpp-httplib | ✅ Integrated | Yes | No |
| SQLite3 | ✅ Integrated | Yes | Optional |
| PostgreSQL | 🟡 Framework Ready | No | Required |
| MySQL | 🟡 Framework Ready | No | Required |

## Troubleshooting

### CMake Can't Find Dependencies

**Problem**: CMake reports missing libraries

**Solution**: The header-only libraries (json, httplib) are downloaded automatically. For SQL libraries:

```bash
# Check if FetchContent is working
cmake .. -DFETCHCONTENT_QUIET=OFF

# For PostgreSQL
cmake .. -DUSE_POSTGRESQL=ON -DPostgreSQL_ROOT=/path/to/postgresql

# For MySQL
cmake .. -DUSE_MYSQL=ON -DMYSQL_ROOT=/path/to/mysql
```

### Compilation Errors with nlohmann/json

**Problem**: `error: 'nlohmann' is not a namespace`

**Solution**: Ensure you're including correctly:
```cpp
#include <nlohmann/json.hpp>  // Correct
// NOT #include <json.hpp>
```

### Linking Errors with httplib

**Problem**: Undefined reference to socket functions

**Solution**: Ensure platform libraries are linked (already done in CMakeLists.txt):
- Windows: ws2_32, wsock32
- Linux: pthread

### SQLite3 Not Found

**Problem**: SQLite3 not found even though USE_SQLITE=ON

**Solution**: CMake will download automatically. If it fails:
```bash
# Clear cache and retry
rm -rf build
mkdir build
cd build
cmake .. -DUSE_SQLITE=ON
```

## Version Compatibility

### C++ Standard
- **Required**: C++17 or later
- **Recommended**: C++17

### Compiler Support
- **GCC**: 7.0+
- **Clang**: 5.0+
- **MSVC**: Visual Studio 2017 15.7+

### CMake Version
- **Minimum**: 3.15
- **Recommended**: 3.20+

## Performance Considerations

### nlohmann/json

- ✅ Fast for most use cases
- ⚠️ Not the fastest JSON library (consider simdjson for high-performance needs)
- 💡 Use `.dump()` sparingly in hot paths

### cpp-httplib

- ✅ Easy to use, good for moderate traffic
- ⚠️ Single-threaded by default
- 💡 Consider nginx/Apache reverse proxy for production

### SQLite3

- ✅ Excellent for single-user, embedded applications
- ⚠️ Limited concurrent write performance
- 💡 Use WAL mode for better concurrency: `PRAGMA journal_mode=WAL`

## Security Notes

### API Keys

The current implementation uses simple API key authentication. For production:

```cpp
// Current (basic)
server.setApiKey("mySecretKey");

// Recommended (hashed)
server.setApiKey(sha256Hash("mySecretKey"));

// Best practice (JWT)
server.setJWTSecret("mySecretKey");
```

### HTTPS

cpp-httplib supports HTTPS with OpenSSL:

```cpp
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

httplib::SSLServer svr("cert.pem", "key.pem");
```

### SQL Injection Prevention

Always use prepared statements:

```cpp
// BAD: SQL injection vulnerability
std::string sql = "SELECT * FROM items WHERE name = '" + userInput + "'";

// GOOD: Prepared statement
sqlite3_prepare_v2(db, "SELECT * FROM items WHERE name = ?", -1, &stmt, nullptr);
sqlite3_bind_text(stmt, 1, userInput.c_str(), -1, SQLITE_TRANSIENT);
```

## Additional Resources

- [nlohmann/json Documentation](https://json.nlohmann.me/)
- [cpp-httplib Repository](https://github.com/yhirose/cpp-httplib)
- [SQLite Documentation](https://www.sqlite.org/docs.html)
- [PostgreSQL C API](https://www.postgresql.org/docs/current/libpq.html)
- [MySQL C++ Connector](https://dev.mysql.com/doc/connector-cpp/8.0/en/)

## License Information

- **Invelog**: MIT License
- **nlohmann/json**: MIT License
- **cpp-httplib**: MIT License
- **SQLite3**: Public Domain
- **PostgreSQL**: PostgreSQL License (BSD-style)
- **MySQL**: GPL v2 (or Commercial)

---

**Last Updated**: January 2025  
**Invelog Version**: 0.3.1
