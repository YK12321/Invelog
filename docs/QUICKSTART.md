# Invelog Quick Start Guide

## Overview

This guide will help you get started with building and running the Invelog inventory management system.

## Prerequisites

Make sure you have the following installed:
- **CMake 3.15+**
- **C++17 compatible compiler** (Visual Studio 2017+, GCC 7+, or Clang 5+)
- **Git** (optional, for version control)

## Quick Start (Windows)

### 1. Build the Project

Open PowerShell and navigate to the project directory:

```powershell
cd "path\to\Invelog"
```

Create and enter the build directory:

```powershell
mkdir build
cd build
```

Generate the build files:

```powershell
cmake ..
```

Build the project:

```powershell
cmake --build . --config Release
```

### 2. Run the Demo Application

After successful build, you have two executables available:

**Demo Application (Main)**:
```powershell
.\bin\Release\invelog.exe
```

**Database Server**:
```powershell
.\bin\Release\invelog_server.exe --local ./data --no-auth
```

Or for Debug build:

```powershell
cmake --build . --config Debug
.\bin\Debug\invelog.exe
.\bin\Debug\invelog_server.exe --local ./data --no-auth
```

### 3. View Results

**Demo Application** will:
- Create sample categories, locations, and containers
- Add inventory items
- Create a project and allocate resources
- Demonstrate check-in/check-out operations
- Show search capabilities
- Display activity history

Data will be saved to `./invelog_data/` directory.

**Database Server** will:
- Start HTTP server on port 8080 (default)
- Expose REST API endpoints
- Allow multiple clients to connect
- Store data in specified backend (LocalDatabase, SQL, or API)

## Project Structure

```
Invelog/
├── include/              # Header files
│   ├── UUID.h           # Unique identifier system
│   ├── Location.h       # Physical location model
│   ├── Category.h       # Category system
│   ├── Item.h           # Inventory item model
│   ├── Container.h      # Storage container model
│   ├── ActivityLog.h    # Activity tracking
│   ├── Project.h        # Project management model
│   ├── Database.h       # Database interface
│   ├── LocalDatabase.h  # File-based database
│   ├── SQLDatabase.h    # SQL database support
│   ├── APIDatabase.h    # REST API database
│   ├── DatabaseServer.h # Database server
│   └── InventoryManager.h  # Main API facade
├── src/                 # Implementation files
│   ├── UUID.cpp
│   ├── Location.cpp
│   ├── Category.cpp
│   ├── Item.cpp
│   ├── Container.cpp
│   ├── ActivityLog.cpp
│   ├── Project.cpp
│   ├── LocalDatabase.cpp
│   ├── SQLDatabase.cpp
│   ├── APIDatabase.cpp
│   ├── DatabaseServer.cpp
│   ├── InventoryManager.cpp
│   ├── main.cpp         # Demo application
│   └── server_main.cpp  # Database server executable
├── examples/            # Example applications
│   ├── database_examples.cpp    # Database backend demos
│   └── server_client_demo.cpp   # Server/client demo
├── docs/                # Documentation
│   ├── ARCHITECTURE.md
│   ├── ARCHITECTURE_DIAGRAMS.md
│   ├── BUILD.md
│   ├── QUICKSTART.md
│   ├── ROADMAP.md
│   ├── DATABASE_GUIDE.md
│   ├── API_DOCUMENTATION.md
│   ├── DEPLOYMENT_GUIDE.md
│   └── SERVER_QUICKSTART.md
├── build/               # Build output (generated)
├── CMakeLists.txt    # Build configuration
├── .gitignore
└── README.md
```

## Basic Usage Example

```cpp
#include "InventoryManager.h"
#include "LocalDatabase.h"

int main() {
    // 1. Create database and manager
    auto database = std::make_shared<LocalDatabase>("./data");
    InventoryManager manager(database);
    manager.initialize();
    
    // 2. Create a category
    auto category = manager.createCategory("Resistors", "Passive components");
    
    // 3. Create a location
    auto location = manager.createLocation("Main Warehouse", "123 Tech St");
    
    // 4. Create a container
    auto container = manager.createContainer("Storage Box 1");
    location->addContainer(container);
    
    // 5. Create an item
    auto item = manager.createItem("Resistor 1kΩ", category, 100);
    
    // 6. Place item in container
    manager.moveItem(item->getId(), container->getId());
    
    // 7. Search for items
    auto results = manager.searchItems("Resistor");
    
    // 8. Save and shutdown
    manager.shutdown();
    
    return 0;
}
```

## Core Operations

### Creating Entities

```cpp
// Categories
auto category = manager.createCategory("Name", "Description");

// Locations
auto location = manager.createLocation("Lab", "Building A");

// Containers
auto container = manager.createContainer("Box 1", ContainerType::INVENTORY);

// Items
auto item = manager.createItem("Component", category, quantity, "Description");

// Projects
auto project = manager.createProject("Project Name", "Description");
```

### Moving Items

```cpp
// Move item to a container
manager.moveItem(itemId, containerId);

// Check out an item
manager.checkOutItem(itemId, "user_id");

// Check in an item
manager.checkInItem(itemId, "user_id");
```

### Searching

```cpp
// Search items by name
auto results = manager.searchItems("searchTerm");

// Find items by category
auto items = manager.findItemsByCategory(categoryId);

// Find items in location
auto items = manager.findItemsInLocation(locationId);

// Find items in project
auto items = manager.findItemsInProject(projectId);
```

### Project Management

```cpp
// Create project
auto project = manager.createProject("Arduino Project", "LED controller");

// Set project status
project->setStatus(ProjectStatus::IN_PROGRESS);

// Create project container
auto projContainer = manager.createContainer("Project Box", ContainerType::PROJECT);
project->addContainer(projContainer);

// Assign items to project
manager.assignItemToProject(itemId, projectId);

// Get project items
auto items = project->getAllAllocatedItems();
```

## Troubleshooting

### Build Errors

**CMake not found:**
```powershell
# Install CMake from https://cmake.org/download/
# Or use: winget install Kitware.CMake
```

**Compiler errors:**
- Ensure you have Visual Studio 2017 or newer installed
- Or install MinGW-w64 for GCC support

**C++17 errors:**
- Update your compiler to support C++17
- For Visual Studio: Install latest version
- For MinGW: Use version 7.0 or newer

### Runtime Errors

**Database connection failed:**
- Check write permissions in the data directory
- Ensure the directory path exists or can be created

**Items not saving:**
- Verify `manager.shutdown()` is called before program exit
- Check disk space availability

## Next Steps

1. **Explore the Code**: Read through `src/main.cpp` to see all features in action
2. **Read Architecture**: See `docs/ARCHITECTURE.md` for system design details
3. **Customize**: Modify the code to suit your specific needs
4. **Extend**: Add new features like SQL database support or REST API
5. **Test**: Write unit tests for your components

## Getting Help

- Check `docs/ARCHITECTURE.md` for system design
- See `docs/BUILD.md` for detailed build instructions
- Review `docs/ROADMAP.md` for planned features

## Contributing

Contributions are welcome! Areas for contribution:
- SQL database implementation
- JSON serialization/deserialization
- REST API layer
- Unit tests
- Documentation improvements
- Bug fixes

---

**Happy Inventing with Invelog!** 🚀
