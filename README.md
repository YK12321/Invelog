# Invelog

> A comprehensive inventory management system for electronic components with advanced tracking, multi-location support, and project management capabilities.

## Overview

Invelog is a robust inventory management solution designed to track electronic components across multiple locations, containers, and projects. Built with scalability and flexibility in mind, it provides real-time tracking of components with unique identification and comprehensive activity logging.

## Key Features

### 🏗️ Hierarchical Organization
- **Multi-Location Support**: Manage inventory across multiple physical locations
- **Container Management**: Organize items in containers and subcontainers
- **Category System**: Classify components by categories for easy organization
- **UUID Tracking**: Every subcontainer and item is assigned a unique identifier for precise tracking

### 📦 Inventory Management
- **Main Storage Containers**: Designated inventory containers for primary storage
- **Project-Specific Containers**: Create dedicated containers for individual projects
- **Item Movement**: Seamlessly transfer items between main inventory and project containers
- **Component Tracking**: Monitor individual components regardless of their current location

### 📊 Activity Logging
- **Usage Tracking**: Log all activity for each item in the system
- **Check-In/Check-Out**: Track when items are checked out and checked in
- **Time Monitoring**: Record the duration items spend in different containers or projects
- **Audit Trail**: Maintain complete history of component movements and usage

### 🗂️ Project Management
- **Project Creation**: Set up projects with dedicated container allocation
- **Resource Allocation**: Move components from main inventory to project-specific containers
- **Project Tracking**: Monitor component usage per project over time

### 💾 Flexible Database Support
- **Local File-Based Storage**: Perfect for single-user scenarios and offline access
- **SQL Database Integration**: Connect to PostgreSQL, MySQL, SQLite, or MS SQL Server
- **REST API Support**: Integrate with cloud-native applications via HTTP API
- **Custom Database Server**: Deploy standalone server for multi-client access
- **Seamless Switching**: Change database backends without modifying application code
- **Transaction Support**: ACID compliance for SQL databases
- **Connection Pooling**: Optimized performance for multi-user scenarios
- **Schema Management**: Automatic table creation and migration support

## Architecture

Invelog leverages object-oriented programming (OOP) to provide a clean, maintainable codebase with clear separation of concerns. The system architecture supports:

- **Modular Design**: Easy to extend and customize
- **Database Abstraction**: Switch between local and SQL databases seamlessly
- **Scalability**: Handles small workshops to large enterprise inventories

## Use Cases

- **Electronics Workshops**: Track components across multiple storage locations
- **Makerspaces**: Manage shared inventory and project-specific allocations
- **R&D Labs**: Monitor component usage across research projects
- **Manufacturing**: Track parts from warehouse to production floor
- **Educational Institutions**: Manage lab equipment and student project materials
- **Multi-Location Teams**: Share inventory data across network with database server
- **Remote Teams**: Access centralized inventory via REST API from anywhere

## Getting Started

### Prerequisites
- CMake 3.15 or higher
- C++17 compatible compiler (Visual Studio 2017+, GCC 7+, or Clang 5+)

### Quick Build (Windows)

```powershell
# Navigate to project directory
cd "path\to\Invelog"

# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
cmake --build . --config Release

# Run the demo
.\bin\Release\invelog.exe

# Or run the database server
.\bin\Release\invelog_server.exe --local ./data --port 8080 --api-key mySecretKey
```

For detailed build instructions, see [docs/BUILD.md](docs/BUILD.md).

For a comprehensive quick start guide, see [docs/QUICKSTART.md](docs/QUICKSTART.md).

### Basic Usage

```cpp
#include "InventoryManager.h"
#include "LocalDatabase.h"

int main() {
    // Initialize with local database
    auto database = std::make_shared<LocalDatabase>("./data");
    InventoryManager manager(database);
    manager.initialize();
    
    // Create entities
    auto category = manager.createCategory("Resistors");
    auto location = manager.createLocation("Main Warehouse");
    auto container = manager.createContainer("Storage Box 1");
    auto item = manager.createItem("Resistor 1kΩ", category, 100);
    
    // Organize inventory
    location->addContainer(container);
    manager.moveItem(item->getId(), container->getId());
    
    // Shutdown
    manager.shutdown();
    return 0;
}
```

### Using Different Database Backends

```cpp
// Option 1: Local File-Based Database
auto localDb = std::make_shared<LocalDatabase>("./data");

// Option 2: PostgreSQL Database
SQLDatabase::ConnectionConfig sqlConfig;
sqlConfig.type = SQLDatabase::SQLType::POSTGRESQL;
sqlConfig.host = "localhost";
sqlConfig.port = 5432;
sqlConfig.database = "invelog";
sqlConfig.username = "user";
sqlConfig.password = "pass";
auto sqlDb = std::make_shared<SQLDatabase>(sqlConfig);

// Option 3: REST API Database
APIDatabase::APIConfig apiConfig;
apiConfig.baseUrl = "https://api.example.com";
apiConfig.authType = APIDatabase::AuthType::BEARER_TOKEN;
apiConfig.authToken = "your-token-here";
auto apiDb = std::make_shared<APIDatabase>(apiConfig);

// All databases use the same interface!
InventoryManager manager(localDb); // or sqlDb, or apiDb
```

For more database examples, see [examples/database_examples.cpp](examples/database_examples.cpp).

### Running the Database Server

The modular database server offers enhanced configuration options:

```powershell
# Start server with all features
.\bin\Release\invelog_server.exe --local ./data --port 8080 --api-key mySecretKey --cors

# Start server with PostgreSQL and custom limits
.\bin\Release\invelog_server.exe --postgres "host=localhost dbname=invelog" --port 8080 --max-request 20971520 --timeout 600

# Start server for network access with CORS
.\bin\Release\invelog_server.exe --sqlite ./invelog.db --port 8080 --api-key secureKey456 --cors
```

**New Configuration Options (v0.3.0):**
- `--cors` - Enable CORS headers for web applications
- `--max-request <bytes>` - Set maximum request size (default: 10 MB)
- `--timeout <seconds>` - Set request timeout (default: 300s)
- `--no-auth` - Disable authentication for development

Clients can then connect from anywhere:
```cpp
APIDatabase::APIConfig config;
config.baseUrl = "http://192.168.1.100:8080";  // Server IP
config.authType = APIDatabase::AuthType::API_KEY;
config.authToken = "secureKey456";

InventoryManager manager(std::make_shared<APIDatabase>(config));
```

For deployment guide, see [docs/DEPLOYMENT_GUIDE.md](docs/DEPLOYMENT_GUIDE.md).
For modular architecture details, see [MODULAR_ARCHITECTURE.md](MODULAR_ARCHITECTURE.md).

## Current Status

**Version 0.3.0** - Modular Server Architecture ✅

All essential backend components have been implemented with professional architecture:
- ✅ Complete data model (Item, Container, Location, Project, Category)
- ✅ UUID-based tracking system
- ✅ Activity logging and audit trails
- ✅ Check-in/check-out operations
- ✅ Search and query capabilities
- ✅ Local file-based database
- ✅ SQL database support (PostgreSQL, MySQL, SQLite, MS SQL Server)
- ✅ REST API database integration
- ✅ Database abstraction layer for seamless switching
- ✅ **Modular server architecture (HTTP, Routes, Serialization, Auth)**
- ✅ **External dependencies integrated (nlohmann/json, cpp-httplib, SQLite3)**
- ✅ **Production-ready REST API server with authentication**
- ✅ **CORS support and configurable security**
- ✅ Comprehensive demo applications
- ✅ CMake build system with FetchContent
- ✅ Full documentation

**Latest Updates (v0.3.0):**
- 🎉 **Modular Architecture**: Refactored 1,200+ line monolithic server into clean, maintainable modules
- 🎉 **Separation of Concerns**: HTTP, Routes, Serialization, and Auth in separate modules
- 🎉 **Enhanced Configuration**: ServerConfig with CORS, timeouts, request limits
- 🎉 **Professional Code Quality**: Each module 20-220 lines, easy to test and maintain

**Next Steps:**
- 🔄 Frontend development (Web UI)
- 🔄 Unit testing framework
- 🔄 Rate limiting and advanced security
- 🔄 Real-time updates via WebSocket

See [docs/ROADMAP.md](docs/ROADMAP.md) for planned features and development timeline.

## Documentation

- **[Modular Architecture](MODULAR_ARCHITECTURE.md)** - New modular server design (v0.3.0)
- **[Architecture Guide](docs/ARCHITECTURE.md)** - System design and patterns
- **[Build Instructions](docs/BUILD.md)** - Detailed build guide for all platforms
- **[Quick Start Guide](docs/QUICKSTART.md)** - Get up and running quickly
- **[Database Guide](docs/DATABASE_GUIDE.md)** - External database setup and configuration
- **[API Documentation](docs/API_DOCUMENTATION.md)** - REST API reference for database server
- **[Deployment Guide](docs/DEPLOYMENT_GUIDE.md)** - Deploy database server in production
- **[Dependency Guide](docs/DEPENDENCY_GUIDE.md)** - External dependencies and CMake FetchContent
- **[Development Roadmap](docs/ROADMAP.md)** - Planned features and timeline

### Documentation Overview

The system is built with the following core components:

1. **UUID System**: Cryptographically secure unique identifiers
2. **Data Models**: Item, Container, Location, Project, Category, ActivityLog
3. **Database Layer**: Abstract interface with three implementations:
   - **LocalDatabase**: File-based storage for single-user scenarios
   - **SQLDatabase**: Enterprise SQL support (PostgreSQL, MySQL, SQLite, MS SQL)
   - **APIDatabase**: REST API integration for cloud-native applications
4. **Modular Server Architecture** (v0.3.0):
   - **HTTP Module**: Request/response handling with cpp-httplib
   - **Route Handlers**: Dedicated handlers per entity type (Items, Containers, etc.)
   - **JSON Serialization**: Centralized entity ↔ JSON conversion
   - **Authentication**: API key and Bearer token support
   - **Configuration**: Flexible ServerConfig with CORS, timeouts, limits
5. **Database Server**: Standalone HTTP REST API server for multi-client access
6. **Business Logic**: InventoryManager facade for all operations
7. **Activity Tracking**: Comprehensive logging of all operations
8. **External Dependencies**: Automatic download via CMake FetchContent
   - nlohmann/json v3.11.3
   - cpp-httplib v0.15.3
   - SQLite3 amalgamation

## Contributing

Contributions are welcome! Here are some areas where you can help:

### High Priority
1. **Frontend Development** - Web-based UI for inventory management
2. **Unit Tests** - Build comprehensive test suite
3. **Rate Limiting** - Add rate limiter middleware to server
4. **WebSocket Support** - Real-time inventory updates

### Medium Priority
1. **Migration System** - Database schema versioning
2. **Batch Operations** - Bulk import/export
3. **Advanced Search** - Full-text search capabilities
4. **Metrics & Monitoring** - Prometheus metrics endpoint
5. **GraphQL API** - Alternative API alongside REST

### Getting Started
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

See [docs/ROADMAP.md](docs/ROADMAP.md) for detailed feature planning.

## Project Structure

```
Invelog/
├── include/                     # Core library headers
│   ├── UUID.h                   # Unique identifier system
│   ├── Item.h                   # Inventory item model
│   ├── Container.h              # Storage container model
│   ├── Location.h               # Physical location model
│   ├── Project.h                # Project management model
│   ├── Category.h               # Category system
│   ├── ActivityLog.h            # Activity tracking
│   ├── Database.h               # Database interface
│   ├── LocalDatabase.h          # File-based database
│   ├── SQLDatabase.h            # SQL database support
│   ├── APIDatabase.h            # REST API database
│   ├── DatabaseServer.h         # Legacy server (deprecated)
│   └── InventoryManager.h       # Main API facade
├── src/                         # Core implementations + executables
│   ├── *.cpp                    # Core implementations
│   ├── main.cpp                 # Demo application
│   └── server_main.cpp          # Database server entry point
├── server/                      # Modular server architecture (v0.3.0)
│   ├── include/
│   │   ├── http/                # HTTP abstractions
│   │   │   ├── HTTPRequest.h
│   │   │   ├── HTTPResponse.h
│   │   │   ├── RouteHandler.h
│   │   │   └── HTTPServer.h
│   │   ├── routes/              # Entity route handlers
│   │   │   ├── ItemRoutes.h
│   │   │   ├── ContainerRoutes.h
│   │   │   ├── LocationRoutes.h
│   │   │   ├── ProjectRoutes.h
│   │   │   ├── CategoryRoutes.h
│   │   │   └── ActivityLogRoutes.h
│   │   ├── serialization/       # JSON conversion
│   │   │   ├── JSONSerializer.h
│   │   │   └── JSONDeserializer.h
│   │   ├── auth/                # Authentication
│   │   │   └── Authenticator.h
│   │   ├── ServerConfig.h       # Server configuration
│   │   └── DatabaseAPIServer.h  # Server coordinator
│   └── src/                     # Modular implementations
├── examples/                    # Example applications
│   ├── database_examples.cpp    # Database backend demos
│   └── server_client_demo.cpp   # Server/client demo
├── docs/                        # Documentation
│   ├── ARCHITECTURE.md          # System architecture
│   ├── ARCHITECTURE_DIAGRAMS.md # Architecture diagrams
│   ├── BUILD.md                 # Build instructions
│   ├── QUICKSTART.md            # Quick start guide
│   ├── DATABASE_GUIDE.md        # Database backend guide
│   ├── DEPENDENCY_GUIDE.md      # External dependencies
│   ├── API_DOCUMENTATION.md     # REST API reference
│   ├── DEPLOYMENT_GUIDE.md      # Deployment guide
│   ├── SERVER_QUICKSTART.md     # Server quick start
│   ├── DATABASE_SERVER_SUMMARY.md # Server implementation
│   ├── SUMMARY.md               # Project summary
│   └── ROADMAP.md               # Development roadmap
├── MODULAR_ARCHITECTURE.md      # Modular server guide (v0.3.0)
├── CMakeLists.txt               # Build configuration
└── README.md
```

## License

*(Coming soon - License information)*

---

**Built for makers and organizations who need precise inventory control.**
