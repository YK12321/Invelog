# Invelog Architecture

## Overview

Invelog is built using Object-Oriented Programming (OOP) principles in C++ to provide a robust, scalable inventory management system. The architecture is designed for flexibility, allowing easy integration with different database backends. **Version 0.3.0 introduces a modular server architecture** with clean separation of concerns for maintainability and scalability.

## Architecture Layers

```
┌─────────────────────────────────────────────────────────┐
│                    Client Applications                   │
│          (Demo App, Web UI, Mobile, CLI, etc.)          │
└─────────────────────────────────────────────────────────┘
                            │
                            ↓
┌─────────────────────────────────────────────────────────┐
│              REST API Server (Optional)                  │
│              DatabaseAPIServer (Modular)                 │
│    ┌──────────────────────────────────────────────┐    │
│    │ HTTP │ Routes │ Auth │ Serialization │ etc.  │    │
│    └──────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
                            │
                            ↓
┌─────────────────────────────────────────────────────────┐
│              Business Logic Layer                        │
│                 InventoryManager                         │
└─────────────────────────────────────────────────────────┘
                            │
                            ↓
┌─────────────────────────────────────────────────────────┐
│              Database Abstraction Layer                  │
│                    IDatabase Interface                   │
│    ┌──────────────────────────────────────────────┐    │
│    │ LocalDB │ SQLDatabase │ APIDatabase          │    │
│    └──────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
                            │
                            ↓
┌─────────────────────────────────────────────────────────┐
│                 Storage Backends                         │
│      Files │ PostgreSQL │ MySQL │ SQLite │ Remote API   │
└─────────────────────────────────────────────────────────┘
```

## Core Components

### 1. UUID System
- **Purpose**: Provides unique identification for all entities
- **Implementation**: UUID v4 generation using cryptographic random numbers
- **Usage**: Every container, item, location, project, and category has a unique UUID

### 2. Data Model Classes

#### Location
- Represents physical locations where inventory is stored
- Can contain multiple containers
- Attributes: ID, name, address

#### Category
- Hierarchical categorization system
- Supports subcategories for detailed organization
- Attributes: ID, name, description, subcategories

#### Item
- Core inventory unit representing a component or part
- Tracks quantity, location, and activity history
- Supports check-in/check-out operations
- Attributes: ID, name, description, category, quantity, current container, activity history

#### Container
- Flexible storage unit with support for nesting (subcontainers)
- Three types: INVENTORY (main storage), PROJECT (project-specific), SUBCONTAINER (nested)
- Can contain items and other containers
- Attributes: ID, name, description, type, location, parent container, items, subcontainers

#### Project
- Manages project-specific resource allocation
- Contains dedicated containers for project items
- Tracks project status and timeline
- Attributes: ID, name, description, status, dates, containers

#### ActivityLog
- Comprehensive tracking of all item operations
- Records timestamp, user, and activity details
- Activity types: CHECK_IN, CHECK_OUT, MOVED, QUANTITY_ADJUSTED, etc.
- Attributes: ID, type, timestamp, user, item, containers, quantity change

### 3. Database Layer

#### IDatabase (Interface)
- Abstract base class defining database operations
- Provides contract for different database implementations
- Operations: CRUD for all entity types, activity log management

#### LocalDatabase
- File-based database implementation
- Stores data in local directory structure
- Suitable for single-user scenarios or offline operation

#### SQLDatabase
- SQL-based database implementation (PostgreSQL, MySQL, SQLite)
- Supports enterprise-scale deployments
- Multi-user concurrent access
- Configurable connection parameters

#### APIDatabase
- REST API-based database implementation
- Connects to remote Invelog database servers
- Suitable for distributed deployments
- Supports API key authentication

#### DatabaseServer (Modular Architecture v0.3.0)
- **DatabaseAPIServer**: Main coordinator orchestrating all server components
- **HTTP Module**:
  - `HTTPServer`: Wraps cpp-httplib with clean interface
  - `HTTPRequest`/`HTTPResponse`: Request/response structures
  - `RouteHandler`: Function type for route handlers
- **Route Handlers**: Dedicated handler classes per entity type
  - `ItemRoutes`: Item CRUD + move/checkout/checkin operations
  - `ContainerRoutes`: Container CRUD + items/subcontainers
  - `LocationRoutes`: Location CRUD + containers
  - `ProjectRoutes`: Project CRUD + containers
  - `CategoryRoutes`: Category CRUD operations
  - `ActivityLogRoutes`: Activity log queries
- **Serialization**:
  - `JSONSerializer`: Entity → JSON conversion
  - `JSONDeserializer`: JSON → Entity conversion
- **Authentication**:
  - `Authenticator`: API key and Bearer token validation
- **Configuration**:
  - `ServerConfig`: Port, auth, CORS, limits, timeouts
- Features:
  - 30+ REST endpoints (GET, POST, PUT, DELETE)
  - API key authentication (X-API-Key header or Bearer token)
  - CORS support for web clients
  - Configurable request limits and timeouts
  - Health check endpoint
  - Modular design (25 files, 20-220 lines each)
  - Clean separation of concerns
  - Easy to test and maintain
- Works with any IDatabase backend (local, SQL, API)
- Standalone executable (`invelog_server`)
- See [API_DOCUMENTATION.md](API_DOCUMENTATION.md) for endpoint details
- See [MODULAR_ARCHITECTURE.md](../MODULAR_ARCHITECTURE.md) for architecture details

### 4. Business Logic Layer

#### InventoryManager
- Facade pattern: Single entry point for all inventory operations
- Manages relationships between entities
- Handles activity logging automatically
- Provides search and query capabilities
- Key operations:
  - Entity creation and deletion
  - Item movement between containers
  - Check-in/check-out operations
  - Project assignment
  - Activity tracking

## Design Patterns

### 1. **Facade Pattern**
- `InventoryManager` serves as a unified interface
- Simplifies complex subsystem interactions

### 2. **Strategy Pattern**
- `IDatabase` interface allows switching between database implementations
- Easy to extend with new storage backends

### 3. **Factory Pattern** (Future)
- Can be used for creating database instances
- Will support configuration-based database selection

### 4. **Observer Pattern** (Future)
- Can notify UI/external systems of inventory changes
- Real-time updates for collaborative environments

## Data Flow

### Standard Application Flow
```
User/API
    ↓
InventoryManager (Business Logic)
    ↓
Domain Objects (Item, Container, Project, etc.)
    ↓
IDatabase (Abstract Interface)
    ↓
LocalDatabase / SQLDatabase / APIDatabase (Implementation)
    ↓
Storage (Files / SQL Database / Remote API)
```

### Database Server Flow
```
HTTP Client
    ↓
HTTP REST API (DatabaseServer)
    ↓
Authentication (API Key)
    ↓
IDatabase (Abstract Interface)
    ↓
LocalDatabase / SQLDatabase (Backend)
    ↓
Storage (Files / SQL Database)
```

## Key Features Implementation

### UUID Tracking
- Every entity gets a unique UUID upon creation
- Enables precise tracking across system

### Multi-Location Support
- Locations contain containers
- Containers can be nested
- Items track their container location

### Activity Logging
- Automatic logging via InventoryManager
- All operations create activity records
- Complete audit trail for compliance

### Project Management
- Projects have dedicated containers
- Items can be assigned to projects
- Track item movement to/from projects

### Flexible Storage
- Database abstraction allows multiple backends
- Easy migration between storage types
- Supports both local and remote databases

## Extensibility Points

1. **New Database Types**: Implement `IDatabase` interface
2. **Custom Activity Types**: Extend `ActivityType` enum
3. **Additional Container Types**: Extend `ContainerType` enum
4. **Search Algorithms**: Add methods to `InventoryManager`
5. **Export/Import**: Add serialization layers
6. **Custom REST Endpoints**: Extend `DatabaseServer` class
7. **Authentication Methods**: Customize API authentication in `DatabaseServer`

## Performance Considerations

- In-memory caching in `InventoryManager` for frequent queries
- Lazy loading for large object graphs
- Batch operations for database efficiency
- Indexing strategy for database implementations

## Security Considerations

- User ID tracking for all activities
- Audit trail for accountability
- (Future) Role-based access control
- (Future) Data encryption for sensitive information

## Future Enhancements

1. ✅ **SQL Database Implementation** - COMPLETED
2. ✅ **REST API Layer** - COMPLETED (DatabaseServer)
3. **Real-time Notifications** - WebSocket support
4. **Advanced Search and Filtering** - Full-text search
5. **Reporting and Analytics** - Data visualization
6. **Multi-user Concurrency Control** - Lock management
7. **Data Import/Export** - CSV, JSON, XML support
8. **Barcode/QR Code Integration** - Scanner support
9. **Mobile Application** - React Native or Flutter client
10. **Web Dashboard** - React/Vue.js web interface

## Additional Documentation

- [API_DOCUMENTATION.md](API_DOCUMENTATION.md) - Complete REST API reference
- [DATABASE_GUIDE.md](DATABASE_GUIDE.md) - Database backend configuration
- [DEPLOYMENT_GUIDE.md](DEPLOYMENT_GUIDE.md) - Production deployment guide
- [SERVER_QUICKSTART.md](SERVER_QUICKSTART.md) - Quick server setup
- [ARCHITECTURE_DIAGRAMS.md](ARCHITECTURE_DIAGRAMS.md) - Visual architecture diagrams
