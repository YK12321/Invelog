# Invelog Modular Server Architecture

## Overview

The Invelog database server has been refactored from a monolithic 1,200+ line file into a clean, modular architecture. This document explains the new structure and how to work with it.

## Directory Structure

```
server/
├── include/
│   ├── http/                    # HTTP abstractions
│   │   ├── HTTPRequest.h        # Request structure with helper methods
│   │   ├── HTTPResponse.h       # Response structure with factory methods
│   │   ├── RouteHandler.h       # Route handler function type
│   │   └── HTTPServer.h         # HTTP server wrapper around cpp-httplib
│   ├── routes/                  # Route handlers (one per entity type)
│   │   ├── ItemRoutes.h         # Item CRUD + move/checkout/checkin
│   │   ├── ContainerRoutes.h    # Container CRUD + relationships
│   │   ├── LocationRoutes.h     # Location CRUD + containers
│   │   ├── ProjectRoutes.h      # Project CRUD + containers
│   │   ├── CategoryRoutes.h     # Category CRUD
│   │   └── ActivityLogRoutes.h  # Activity log queries
│   ├── serialization/           # JSON conversion
│   │   ├── JSONSerializer.h     # Entity → JSON
│   │   └── JSONDeserializer.h   # JSON → Entity
│   ├── auth/                    # Authentication
│   │   └── Authenticator.h      # API key/Bearer token validation
│   ├── ServerConfig.h           # Server configuration struct
│   └── DatabaseAPIServer.h      # Main server coordinator
└── src/                         # Implementations (mirrors include/)
    ├── http/
    ├── routes/
    ├── serialization/
    ├── auth/
    └── DatabaseAPIServer.cpp
```

## Key Components

### 1. HTTPServer (`http/HTTPServer.h`)

Wraps the `cpp-httplib` library to provide a clean interface:

```cpp
HTTPServer server(8080);
server.registerRoute("GET", "/api/items", [](const HTTPRequest& req) {
    return HTTPResponse::ok("{}", "application/json");
});
server.start();
```

**Features:**
- Automatic route registration
- Request/response abstraction
- Middleware support
- Thread-safe operation

### 2. Route Handlers (`routes/*.h`)

Each entity type has its own route handler class:

```cpp
class ItemRoutes {
public:
    explicit ItemRoutes(std::shared_ptr<IDatabase> database);
    
    HTTPResponse handleGetAll(const HTTPRequest& req);
    HTTPResponse handleGetById(const HTTPRequest& req);
    HTTPResponse handleCreate(const HTTPRequest& req);
    HTTPResponse handleUpdate(const HTTPRequest& req);
    HTTPResponse handleDelete(const HTTPRequest& req);
    // ... specialized methods
};
```

**Benefits:**
- Single Responsibility Principle
- Easy to test individually
- Clear separation of concerns
- ~100-150 lines per handler (vs. 1,200+ line monolith)

### 3. JSON Serialization (`serialization/*.h`)

Centralized JSON conversion:

```cpp
// Entity to JSON
std::string json = JSONSerializer::serialize(item);
std::string json = JSONSerializer::serialize(items_vector);

// JSON to Entity
auto item = JSONDeserializer::deserializeItem(jsonString);
JSONDeserializer::updateItem(existingItem, jsonString);
```

**Features:**
- Type-safe conversion
- Error handling
- Consistent format
- Array support

### 4. Authentication (`auth/Authenticator.h`)

Handles API key and Bearer token authentication:

```cpp
Authenticator auth("your-secret-key");
if (auth.authenticate(request)) {
    // Proceed with request
}
```

**Supports:**
- `X-API-Key` header
- `Authorization: Bearer <token>` header
- Configurable per-route

### 5. DatabaseAPIServer (`DatabaseAPIServer.h`)

The main coordinator that ties everything together:

```cpp
ServerConfig config;
config.port = 8080;
config.authRequired = true;
config.apiKey = "secret";
config.enableCORS = true;

auto server = std::make_shared<DatabaseAPIServer>(database, config);
server->start();
```

**Responsibilities:**
- Initialize all components
- Register all routes
- Apply middleware (auth, CORS)
- Coordinate requests

## API Endpoints

### Items
- `GET /api/items` - Get all items
- `GET /api/items/{id}` - Get item by ID
- `POST /api/items` - Create new item
- `PUT /api/items/{id}` - Update item
- `DELETE /api/items/{id}` - Delete item

### Containers
- `GET /api/containers` - Get all containers
- `GET /api/containers/{id}` - Get container by ID
- `POST /api/containers` - Create new container
- `PUT /api/containers/{id}` - Update container
- `DELETE /api/containers/{id}` - Delete container
- `GET /api/containers/{id}/items` - Get items in container
- `GET /api/containers/{id}/subcontainers` - Get sub-containers

### Locations
- `GET /api/locations` - Get all locations
- `GET /api/locations/{id}` - Get location by ID
- `POST /api/locations` - Create new location
- `PUT /api/locations/{id}` - Update location
- `DELETE /api/locations/{id}` - Delete location
- `GET /api/locations/{id}/containers` - Get containers at location

### Projects
- `GET /api/projects` - Get all projects
- `GET /api/projects/{id}` - Get project by ID
- `POST /api/projects` - Create new project
- `PUT /api/projects/{id}` - Update project
- `DELETE /api/projects/{id}` - Delete project
- `GET /api/projects/{id}/containers` - Get project containers

### Categories
- `GET /api/categories` - Get all categories
- `GET /api/categories/{id}` - Get category by ID
- `POST /api/categories` - Create new category
- `PUT /api/categories/{id}` - Update category
- `DELETE /api/categories/{id}` - Delete category

### Activity Logs
- `GET /api/logs` - Get all activity logs
- `GET /api/logs/{id}` - Get log by ID
- `GET /api/logs/item/{id}` - Get logs for item
- `GET /api/logs/user?user_id=<id>` - Get logs by user
- `GET /api/logs/date-range?start_date=<date>&end_date=<date>` - Get logs by date range

### Other
- `GET /health` - Health check (no auth required)
- `GET /api/search?query=<text>` - Search items

## Building the Project

The CMakeLists.txt has been updated to include all modular server files:

```bash
# Configure
cmake -B build

# Build
cmake --build build

# Run server
./build/bin/invelog_server --local ./data --port 8080 --api-key mykey --cors
```

## Adding New Routes

To add a new route handler:

1. **Create header** in `server/include/routes/`:
```cpp
// NewEntityRoutes.h
class NewEntityRoutes {
public:
    explicit NewEntityRoutes(std::shared_ptr<IDatabase> db);
    HTTPResponse handleGetAll(const HTTPRequest& req);
    // ... other handlers
private:
    std::shared_ptr<IDatabase> database;
};
```

2. **Create implementation** in `server/src/routes/`:
```cpp
// NewEntityRoutes.cpp
#include "../include/routes/NewEntityRoutes.h"
#include "../include/serialization/JSONSerializer.h"

NewEntityRoutes::NewEntityRoutes(std::shared_ptr<IDatabase> db) 
    : database(db) {}

HTTPResponse NewEntityRoutes::handleGetAll(const HTTPRequest& req) {
    try {
        auto entities = database->loadAllNewEntities();
        std::string json = JSONSerializer::serialize(entities);
        return HTTPResponse::ok(json, "application/json");
    } catch (const std::exception& e) {
        return HTTPResponse::internalError(
            JSONSerializer::serializeError(e.what())
        );
    }
}
```

3. **Register in DatabaseAPIServer**:
```cpp
// In DatabaseAPIServer constructor
newEntityRoutes = std::make_unique<NewEntityRoutes>(database);

// In registerAllRoutes()
httpServer->registerRoute("GET", "/api/newentities", 
    [this](const HTTPRequest& req) { 
        return newEntityRoutes->handleGetAll(req); 
    });
```

4. **Update CMakeLists.txt**:
```cmake
set(SERVER_SOURCES
    # ... existing files
    server/src/routes/NewEntityRoutes.cpp
)

set(SERVER_HEADERS
    # ... existing files
    server/include/routes/NewEntityRoutes.h
)
```

## Testing

Each component can be tested independently:

```cpp
// Test a route handler
auto db = std::make_shared<LocalDatabase>("./test_data");
ItemRoutes routes(db);

HTTPRequest req;
req.method = "GET";
req.path = "/api/items";

HTTPResponse res = routes.handleGetAll(req);
assert(res.statusCode == 200);
```

## Advantages Over Monolithic Design

### Before (DatabaseServer.h/cpp - 1,200+ lines)
- ❌ All concerns mixed together
- ❌ Hard to navigate and maintain
- ❌ Difficult to test specific features
- ❌ Multiple developers can't work simultaneously
- ❌ Changes affect entire file

### After (Modular Architecture)
- ✅ Clear separation of concerns
- ✅ Each file is 20-220 lines
- ✅ Easy to test individual components
- ✅ Multiple developers can work on different modules
- ✅ Changes are isolated to specific files
- ✅ Better code reuse
- ✅ Easier to understand and onboard new developers

## Configuration Options

### ServerConfig Structure

```cpp
struct ServerConfig {
    int port = 8080;                      // Server port
    bool authRequired = false;            // Enable authentication
    std::string apiKey = "";              // API key for auth
    bool enableCORS = false;              // Enable CORS headers
    size_t maxRequestSize = 10485760;    // Max request size (10 MB)
    int timeoutSeconds = 300;             // Request timeout (5 min)
};
```

### Command Line Options

```bash
invelog_server --port 8080 \
               --api-key your-secret-key \
               --cors \
               --max-request 20971520 \
               --timeout 600 \
               --sqlite ./invelog.db
```

## Migration from Old Server

The old `DatabaseServer` class is still available but deprecated. To migrate:

```cpp
// OLD (deprecated)
auto server = std::make_shared<DatabaseServer>(database, port);
server->setAuthRequired(true);
server->setApiKey("key");
server->start();

// NEW (modular)
ServerConfig config;
config.port = port;
config.authRequired = true;
config.apiKey = "key";
auto server = std::make_shared<DatabaseAPIServer>(database, config);
server->start();
```

## Future Enhancements

Potential improvements to the modular architecture:

1. **Rate Limiting**: Add rate limiter middleware
2. **Logging**: Add structured logging component
3. **Metrics**: Add Prometheus metrics endpoint
4. **WebSocket Support**: Add real-time updates
5. **GraphQL**: Add GraphQL endpoint alongside REST
6. **Caching**: Add Redis-based caching layer
7. **API Versioning**: Support `/api/v1/`, `/api/v2/` endpoints

## Version History

- **v0.3.0** (Current) - Modular architecture implementation
- **v0.2.0** - External dependency integration (nlohmann/json, cpp-httplib, SQLite3)
- **v0.1.0** - Initial monolithic implementation

---

For questions or issues, please refer to the main [README.md](README.md) or open an issue on GitHub.
