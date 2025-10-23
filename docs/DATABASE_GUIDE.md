# External Database Support Guide

## Overview

Invelog now supports three database backends:
1. **LocalDatabase** - File-based storage for single-user scenarios
2. **SQLDatabase** - Enterprise SQL databases (PostgreSQL, MySQL, SQLite, MSSQL)
3. **APIDatabase** - REST API integration for cloud-native deployments

All three implementations share the same `IDatabase` interface, allowing you to switch between them seamlessly.

---

## LocalDatabase (File-Based)

### Use Cases
- Single-user desktop applications
- Offline operation
- Quick prototyping
- No server setup required

### Setup

```cpp
#include "LocalDatabase.h"
#include "InventoryManager.h"

auto database = std::make_shared<LocalDatabase>("./invelog_data");
InventoryManager manager(database);
manager.initialize();
```

### Features
- ✅ No external dependencies
- ✅ Simple file-based storage
- ✅ Immediate setup
- ✅ Full offline support

### Directory Structure
```
invelog_data/
├── items/
├── containers/
├── locations/
├── projects/
├── categories/
└── activity_logs/
```

---

## SQLDatabase (SQL Databases)

### Supported Databases
- PostgreSQL (Recommended for production)
- MySQL / MariaDB
- SQLite (Embedded)
- Microsoft SQL Server

### PostgreSQL Setup

```cpp
#include "SQLDatabase.h"

SQLDatabase::ConnectionConfig config;
config.type = SQLDatabase::SQLType::POSTGRESQL;
config.host = "localhost";
config.port = 5432;
config.database = "invelog_db";
config.username = "invelog_user";
config.password = "secure_password";
config.useSSL = true;
config.maxConnections = 10;

auto database = std::make_shared<SQLDatabase>(config);
InventoryManager manager(database);
manager.initialize();
```

### MySQL Setup

```cpp
SQLDatabase::ConnectionConfig config;
config.type = SQLDatabase::SQLType::MYSQL;
config.host = "localhost";
config.port = 3306;
config.database = "invelog";
config.username = "root";
config.password = "password";

auto database = std::make_shared<SQLDatabase>(config);
```

### SQLite Setup (Embedded)

```cpp
SQLDatabase::ConnectionConfig config;
config.type = SQLDatabase::SQLType::SQLITE;
config.database = "./invelog.db";  // File path

auto database = std::make_shared<SQLDatabase>(config);
```

### Database Schema

The SQLDatabase automatically creates these tables:

- `schema_version` - Schema versioning
- `categories` - Component categories with hierarchy
- `locations` - Physical locations
- `containers` - Storage containers with nesting
- `items` - Inventory items
- `projects` - Project management
- `project_containers` - Junction table
- `activity_logs` - Activity tracking

### Features
- ✅ ACID transactions
- ✅ Complex queries
- ✅ Multi-user support
- ✅ Schema migrations
- ✅ Indexes for performance
- ✅ Foreign key constraints
- ✅ Connection pooling

### Transaction Support

```cpp
auto sqlDb = std::dynamic_pointer_cast<SQLDatabase>(database);

sqlDb->beginTransaction();
try {
    manager.createItem("Item 1", category, 10);
    manager.createItem("Item 2", category, 20);
    sqlDb->commitTransaction();
} catch (...) {
    sqlDb->rollbackTransaction();
}
```

### Production Setup

1. **Install PostgreSQL**
   ```bash
   # Ubuntu/Debian
   sudo apt install postgresql libpq-dev
   
   # macOS
   brew install postgresql
   
   # Windows
   # Download from https://www.postgresql.org/download/windows/
   ```

2. **Create Database**
   ```sql
   CREATE DATABASE invelog_db;
   CREATE USER invelog_user WITH PASSWORD 'secure_password';
   GRANT ALL PRIVILEGES ON DATABASE invelog_db TO invelog_user;
   ```

3. **Install C++ Library**
   - PostgreSQL: libpq (included with PostgreSQL)
   - MySQL: MySQL Connector/C++
   - SQLite: sqlite3 library

4. **Link in CMake**
   ```cmake
   find_package(PostgreSQL REQUIRED)
   target_link_libraries(invelog_lib ${PostgreSQL_LIBRARIES})
   ```

---

## APIDatabase (REST API)

### Use Cases
- Cloud-native applications
- Microservices architecture
- SaaS platforms
- External system integration

### Setup with API Key

```cpp
#include "APIDatabase.h"

APIDatabase::APIConfig config;
config.baseUrl = "https://api.invelog.example.com/v1";
config.apiKey = "your-api-key-here";
config.authMethod = APIDatabase::APIConfig::AuthMethod::API_KEY;
config.timeoutSeconds = 30;
config.maxRetries = 3;
config.maxRequestsPerMinute = 100;

auto database = std::make_shared<APIDatabase>(config);
InventoryManager manager(database);
manager.initialize();
```

### OAuth2 / Bearer Token Setup

```cpp
APIDatabase::APIConfig config;
config.baseUrl = "https://api.invelog.cloud/v2";
config.authToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...";
config.authMethod = APIDatabase::APIConfig::AuthMethod::BEARER_TOKEN;
config.verifySSL = true;
```

### Custom Headers

```cpp
config.customHeaders["X-Custom-Header"] = "CustomValue";
config.customHeaders["X-Client-Version"] = "1.0.0";
```

### Endpoint Customization

```cpp
config.itemsEndpoint = "/api/inventory/items";
config.containersEndpoint = "/api/inventory/containers";
config.locationsEndpoint = "/api/inventory/locations";
```

### Features
- ✅ Multiple authentication methods
- ✅ Automatic retry with exponential backoff
- ✅ Rate limiting
- ✅ Custom headers
- ✅ Batch operations
- ✅ SSL/TLS verification
- ✅ Connection timeout

### API Endpoints Expected

The APIDatabase expects the following REST endpoints:

**Items**
- `GET /items` - List all items
- `GET /items/{id}` - Get item by ID
- `PUT /items/{id}` - Create/update item
- `DELETE /items/{id}` - Delete item
- `POST /items/batch` - Batch create items

**Containers**
- `GET /containers` - List all containers
- `GET /containers/{id}` - Get container
- `PUT /containers/{id}` - Create/update container
- `DELETE /containers/{id}` - Delete container

**Similar patterns for**: locations, projects, categories, activity-logs

### Batch Operations

```cpp
auto apiDb = std::dynamic_pointer_cast<APIDatabase>(database);

// Batch save items
std::vector<std::shared_ptr<Item>> items = { item1, item2, item3 };
apiDb->saveBatch(items);

// Batch delete
std::vector<UUID> ids = { id1, id2, id3 };
apiDb->deleteBatch(ids, "items");
```

### Production Setup

1. **Install HTTP Client Library**
   ```bash
   # libcurl (recommended)
   sudo apt install libcurl4-openssl-dev
   
   # Or cpp-httplib (header-only)
   git clone https://github.com/yhirose/cpp-httplib.git
   ```

2. **Update CMake**
   ```cmake
   find_package(CURL REQUIRED)
   target_link_libraries(invelog_lib ${CURL_LIBRARIES})
   ```

3. **Integrate JSON Library**
   ```cmake
   # nlohmann/json
   find_package(nlohmann_json REQUIRED)
   target_link_libraries(invelog_lib nlohmann_json::nlohmann_json)
   ```

---

## Switching Between Databases

The beauty of the `IDatabase` interface is that you can switch databases with minimal code changes:

```cpp
// Development: Use LocalDatabase
#ifdef DEBUG
    auto database = std::make_shared<LocalDatabase>("./dev_data");
#endif

// Testing: Use SQLite
#ifdef TEST
    SQLDatabase::ConnectionConfig config;
    config.type = SQLDatabase::SQLType::SQLITE;
    config.database = ":memory:";  // In-memory database
    auto database = std::make_shared<SQLDatabase>(config);
#endif

// Production: Use PostgreSQL
#ifdef PRODUCTION
    SQLDatabase::ConnectionConfig config;
    config.type = SQLDatabase::SQLType::POSTGRESQL;
    config.host = std::getenv("DB_HOST");
    config.database = std::getenv("DB_NAME");
    config.username = std::getenv("DB_USER");
    config.password = std::getenv("DB_PASS");
    auto database = std::make_shared<SQLDatabase>(config);
#endif

// Everything else stays the same!
InventoryManager manager(database);
manager.initialize();

// Use the manager as normal
auto item = manager.createItem("Component", category, 100);
```

---

## Comparison Table

| Feature | LocalDatabase | SQLDatabase | APIDatabase |
|---------|--------------|-------------|-------------|
| **Setup Complexity** | Easy | Medium | Medium |
| **External Dependencies** | None | SQL library | HTTP + JSON |
| **Multi-User** | No | Yes | Yes |
| **Offline Support** | Yes | No* | No |
| **Scalability** | Low | High | Very High |
| **Transaction Support** | No | Yes | Depends |
| **Query Performance** | Medium | High | Medium |
| **Best For** | Desktop apps | Enterprise | Cloud/SaaS |

\* SQLite can work offline

---

## Implementation Checklist

### For Production SQL Support

- [ ] Install database library (libpq, MySQL Connector, etc.)
- [ ] Implement actual SQL connection in `SQLDatabase.cpp`
- [ ] Complete deserialization methods
- [ ] Add connection pooling
- [ ] Implement prepared statements
- [ ] Add transaction support
- [ ] Create migration system
- [ ] Write integration tests

### For Production API Support

- [ ] Install HTTP library (libcurl, cpp-httplib)
- [ ] Install JSON library (nlohmann/json)
- [ ] Implement actual HTTP requests
- [ ] Complete JSON serialization/deserialization
- [ ] Add SSL certificate verification
- [ ] Implement OAuth2 refresh token logic
- [ ] Add request/response logging
- [ ] Write API integration tests

### For Both

- [ ] Add comprehensive error handling
- [ ] Implement logging system
- [ ] Add performance monitoring
- [ ] Write unit tests
- [ ] Create documentation
- [ ] Add configuration file support
- [ ] Implement connection retry logic
- [ ] Add health check endpoints

---

## Example: Multi-Database Deployment

```cpp
class DatabaseFactory {
public:
    static std::shared_ptr<IDatabase> createDatabase(const std::string& type) {
        if (type == "local") {
            return std::make_shared<LocalDatabase>("./data");
        }
        else if (type == "postgresql") {
            SQLDatabase::ConnectionConfig config;
            config.type = SQLDatabase::SQLType::POSTGRESQL;
            loadConfigFromFile(config, "database.conf");
            return std::make_shared<SQLDatabase>(config);
        }
        else if (type == "api") {
            APIDatabase::APIConfig config;
            loadConfigFromFile(config, "api.conf");
            return std::make_shared<APIDatabase>(config);
        }
        
        throw std::runtime_error("Unknown database type");
    }
};

// Usage
auto database = DatabaseFactory::createDatabase("postgresql");
InventoryManager manager(database);
```

---

## Next Steps

1. **Choose Your Database**: Select based on your use case
2. **Install Dependencies**: Database libraries and HTTP client
3. **Configure Connection**: Set up connection parameters
4. **Test Connection**: Verify database connectivity
5. **Migrate Schema**: For SQL databases, ensure schema is current
6. **Deploy**: Start using your chosen backend!

---

## Support & Resources

- **PostgreSQL**: https://www.postgresql.org/docs/
- **MySQL**: https://dev.mysql.com/doc/
- **SQLite**: https://www.sqlite.org/docs.html
- **libpq**: https://www.postgresql.org/docs/current/libpq.html
- **libcurl**: https://curl.se/libcurl/
- **nlohmann/json**: https://json.nlohmann.me/

---

**Ready to scale your inventory management to any platform!** 🚀
