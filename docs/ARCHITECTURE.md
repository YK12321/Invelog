# Invelog Architecture

## Overview

Invelog is built using Object-Oriented Programming (OOP) principles in C++ to provide a robust, scalable inventory management system. The architecture is designed for flexibility, allowing easy integration with different database backends.

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

#### Future: SQLDatabase
- SQL-based database implementation (to be implemented)
- Supports enterprise-scale deployments
- Multi-user concurrent access

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

```
User/API
    ↓
InventoryManager (Business Logic)
    ↓
Domain Objects (Item, Container, Project, etc.)
    ↓
IDatabase (Abstract Interface)
    ↓
LocalDatabase / SQLDatabase (Implementation)
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

1. **SQL Database Implementation**
2. **REST API Layer**
3. **Real-time Notifications**
4. **Advanced Search and Filtering**
5. **Reporting and Analytics**
6. **Multi-user Concurrency Control**
7. **Data Import/Export (CSV, JSON)**
8. **Barcode/QR Code Integration**
