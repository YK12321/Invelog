# Invelog Database Server API Documentation

## Overview

The Invelog Database Server provides a RESTful API for managing inventory data. It is a fully functional, production-ready service that clients (including the APIDatabase client) communicate with over HTTP.

**Version**: 0.3.0 (Modular Architecture)  
**Base URL**: `http://localhost:8080/api` (default)  
**Protocol**: HTTP/HTTPS  
**Data Format**: JSON  
**Status**: ✅ Production Ready - Fully Tested

### Implementation Details

- **HTTP Server**: cpp-httplib v0.15.3 with OpenSSL support
- **JSON Library**: nlohmann/json v3.11.3
- **Architecture**: Modular design with separate route handlers
- **Database Backends**: LocalDatabase (file-based), SQLDatabase (PostgreSQL/MySQL/SQLite)
- **Testing**: Complete end-to-end test suite validated

---

## Table of Contents

1. [Authentication](#authentication)
2. [Response Format](#response-format)
3. [Error Handling](#error-handling)
4. [Endpoints](#endpoints)
   - [Health Check](#health-check)
   - [Items](#items)
   - [Containers](#containers)
   - [Locations](#locations)
   - [Projects](#projects)
   - [Categories](#categories)
   - [Activity Logs](#activity-logs)
   - [Search](#search)
   - [Batch Operations](#batch-operations)

---

## Authentication

The server supports API key authentication. Include your API key in one of the following ways:

### Option 1: Custom Header (Recommended)
```http
X-API-Key: your-api-key-here
```

### Option 2: Bearer Token
```http
Authorization: Bearer your-api-key-here
```

### Option 3: No Authentication
If the server is started with `--no-auth`, authentication is disabled.

---

## Response Format

### Success Response
```json
{
  "id": "uuid-here",
  "name": "Item Name",
  "...": "other fields"
}
```

### Error Response
```json
{
  "error": true,
  "message": "Error description",
  "statusCode": 400
}
```

---

## Error Handling

| Status Code | Description |
|-------------|-------------|
| 200 | OK - Request successful |
| 201 | Created - Resource created successfully |
| 400 | Bad Request - Invalid data or parameters |
| 401 | Unauthorized - Missing or invalid API key |
| 404 | Not Found - Resource doesn't exist |
| 500 | Internal Server Error - Server-side error |

---

## Endpoints

### Health Check

#### GET /api/health
Check server health and status.

**Authentication**: Not required

**Response**:
```json
{
  "status": "healthy",
  "version": "0.2.0",
  "database": "connected",
  "timestamp": 1698076800
}
```

---

### Items

#### GET /api/items
Retrieve all items.

**Authentication**: Required (if enabled)

**Response**:
```json
[
  {
    "id": "550e8400-e29b-41d4-a716-446655440000",
    "name": "Resistor 1kΩ",
    "description": "1/4W carbon film resistor",
    "quantity": 100,
    "categoryId": "category-uuid",
    "containerId": "container-uuid",
    "checkedOut": false
  }
]
```

#### GET /api/items/:id
Retrieve a specific item by ID.

**Parameters**:
- `id` (path) - Item UUID

**Response**:
```json
{
  "id": "550e8400-e29b-41d4-a716-446655440000",
  "name": "Resistor 1kΩ",
  "description": "1/4W carbon film resistor",
  "quantity": 100,
  "categoryId": "category-uuid",
  "containerId": "container-uuid",
  "checkedOut": false
}
```

#### POST /api/items
Create a new item.

**Request Body**:
```json
{
  "name": "Resistor 1kΩ",
  "description": "1/4W carbon film resistor",
  "quantity": 100,
  "categoryId": "category-uuid"
}
```

**Response**: 201 Created
```json
{
  "id": "generated-uuid",
  "name": "Resistor 1kΩ",
  "description": "1/4W carbon film resistor",
  "quantity": 100,
  "categoryId": "category-uuid",
  "containerId": "",
  "checkedOut": false
}
```

#### PUT /api/items/:id
Update an existing item.

**Parameters**:
- `id` (path) - Item UUID

**Request Body**:
```json
{
  "id": "550e8400-e29b-41d4-a716-446655440000",
  "name": "Resistor 1kΩ (Updated)",
  "description": "Updated description",
  "quantity": 150,
  "categoryId": "category-uuid",
  "containerId": "container-uuid",
  "checkedOut": false
}
```

**Response**: 200 OK (same as request body)

#### DELETE /api/items/:id
Delete an item.

**Parameters**:
- `id` (path) - Item UUID

**Response**:
```json
{
  "success": true,
  "message": "Item deleted"
}
```

---

### Containers

#### GET /api/containers
Retrieve all containers.

**Response**:
```json
[
  {
    "id": "container-uuid",
    "name": "Storage Box 1",
    "description": "Main storage container",
    "type": 0
  }
]
```

**Container Types**:
- `0` - INVENTORY
- `1` - PROJECT
- `2` - SUBCONTAINER

#### GET /api/containers/:id
Retrieve a specific container.

**Parameters**:
- `id` (path) - Container UUID

#### POST /api/containers
Create a new container.

**Request Body**:
```json
{
  "name": "Storage Box 1",
  "description": "Main storage container",
  "type": 0
}
```

#### PUT /api/containers/:id
Update a container.

#### DELETE /api/containers/:id
Delete a container.

---

### Locations

#### GET /api/locations
Retrieve all locations.

**Response**:
```json
[
  {
    "id": "location-uuid",
    "name": "Main Warehouse",
    "description": "Primary storage facility"
  }
]
```

#### GET /api/locations/:id
Retrieve a specific location.

#### POST /api/locations
Create a new location.

**Request Body**:
```json
{
  "name": "Main Warehouse",
  "description": "Primary storage facility"
}
```

#### PUT /api/locations/:id
Update a location.

#### DELETE /api/locations/:id
Delete a location.

---

### Projects

#### GET /api/projects
Retrieve all projects.

**Response**:
```json
[
  {
    "id": "project-uuid",
    "name": "Arduino Project",
    "description": "Home automation project",
    "status": 1
  }
]
```

**Project Status**:
- `0` - PLANNED
- `1` - IN_PROGRESS
- `2` - ON_HOLD
- `3` - COMPLETED
- `4` - CANCELLED

#### GET /api/projects/:id
Retrieve a specific project.

#### POST /api/projects
Create a new project.

**Request Body**:
```json
{
  "name": "Arduino Project",
  "description": "Home automation project",
  "status": 0
}
```

#### PUT /api/projects/:id
Update a project.

#### DELETE /api/projects/:id
Delete a project.

---

### Categories

#### GET /api/categories
Retrieve all categories.

**Response**:
```json
[
  {
    "id": "category-uuid",
    "name": "Resistors",
    "description": "All types of resistors"
  }
]
```

#### GET /api/categories/:id
Retrieve a specific category.

#### POST /api/categories
Create a new category.

**Request Body**:
```json
{
  "name": "Resistors",
  "description": "All types of resistors"
}
```

#### PUT /api/categories/:id
Update a category.

#### DELETE /api/categories/:id
Delete a category.

---

### Activity Logs

#### GET /api/activity_logs
Retrieve all activity logs.

**Response**:
```json
[
  {
    "id": "log-uuid",
    "itemId": "item-uuid",
    "type": "CHECK_OUT",
    "timestamp": 1698076800,
    "userId": "user123",
    "notes": "Checked out for project"
  }
]
```

**Activity Types**:
- `CHECK_IN`
- `CHECK_OUT`
- `MOVED`
- `QUANTITY_ADJUSTED`
- `CREATED`
- `UPDATED`
- `DELETED`
- `ASSIGNED_TO_PROJECT`
- `REMOVED_FROM_PROJECT`

#### GET /api/activity_logs/:id
Retrieve a specific activity log.

#### POST /api/activity_logs
Create a new activity log entry.

**Request Body**:
```json
{
  "itemId": "item-uuid",
  "type": "CHECK_OUT",
  "userId": "user123",
  "notes": "Checked out for project"
}
```

---

### Search

#### POST /api/search
Search for items based on criteria.

**Request Body**:
```json
{
  "query": "resistor",
  "filters": {
    "categoryId": "category-uuid",
    "locationId": "location-uuid",
    "projectId": "project-uuid",
    "minQuantity": 10,
    "maxQuantity": 1000
  },
  "limit": 50,
  "offset": 0
}
```

**Response**:
```json
{
  "results": [
    {
      "id": "item-uuid",
      "name": "Resistor 1kΩ",
      "...": "other fields"
    }
  ],
  "total": 25,
  "limit": 50,
  "offset": 0
}
```

---

### Batch Operations

#### POST /api/batch
Execute multiple operations in a single request.

**Request Body**:
```json
{
  "operations": [
    {
      "method": "POST",
      "endpoint": "/api/items",
      "body": {
        "name": "Item 1",
        "quantity": 10
      }
    },
    {
      "method": "PUT",
      "endpoint": "/api/items/item-uuid",
      "body": {
        "quantity": 20
      }
    }
  ]
}
```

**Response**:
```json
{
  "success": true,
  "processed": 2,
  "results": [
    {
      "status": 201,
      "data": { "id": "new-uuid", "..." }
    },
    {
      "status": 200,
      "data": { "id": "item-uuid", "..." }
    }
  ]
}
```

---

## Usage Examples

### Using curl

#### Create an item
```bash
curl -X POST http://localhost:8080/api/items \
  -H "Content-Type: application/json" \
  -H "X-API-Key: your-api-key" \
  -d '{
    "name": "Resistor 1kΩ",
    "description": "1/4W carbon film",
    "quantity": 100
  }'
```

#### Get all items
```bash
curl -X GET http://localhost:8080/api/items \
  -H "X-API-Key: your-api-key"
```

#### Update an item
```bash
curl -X PUT http://localhost:8080/api/items/550e8400-e29b-41d4-a716-446655440000 \
  -H "Content-Type: application/json" \
  -H "X-API-Key: your-api-key" \
  -d '{
    "id": "550e8400-e29b-41d4-a716-446655440000",
    "name": "Resistor 1kΩ",
    "quantity": 150
  }'
```

### Using the APIDatabase Client

```cpp
#include "APIDatabase.h"

// Configure client
APIDatabase::APIConfig config;
config.baseUrl = "http://localhost:8080";
config.authType = APIDatabase::AuthType::API_KEY;
config.authToken = "your-api-key";

auto database = std::make_shared<APIDatabase>(config);
database->initialize();

// Create an item
auto item = std::make_shared<Item>("Resistor 1kΩ", category, 100);
database->saveItem(item);

// Retrieve items
auto items = database->loadAllItems();
```

---

## Server Configuration

### Starting the Server

#### Local Database (File-based)
```bash
invelog_server --local ./data --port 8080 --api-key mySecretKey
```

#### PostgreSQL Database
```bash
invelog_server --postgres "host=localhost dbname=invelog" --port 8080
```

#### SQLite Database
```bash
invelog_server --sqlite ./invelog.db --no-auth
```

#### MySQL Database
```bash
invelog_server --mysql "host=localhost dbname=invelog" --api-key secretKey
```

### Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--port <port>` | Server port | 8080 |
| `--api-key <key>` | API key for authentication | None |
| `--no-auth` | Disable authentication | Enabled |
| `--local <path>` | Use local file database | ./data |
| `--postgres <conn>` | Use PostgreSQL | - |
| `--mysql <conn>` | Use MySQL | - |
| `--sqlite <path>` | Use SQLite | - |
| `--help` | Show help message | - |

---

## Deployment Considerations

### Security
- Always use HTTPS in production
- Use strong API keys (minimum 32 characters)
- Implement rate limiting for public deployments
- Consider using OAuth2 for multi-user scenarios

### Performance
- Use connection pooling for SQL databases
- Implement caching for frequently accessed data
- Consider using a reverse proxy (nginx, Apache)
- Monitor database query performance

### Scaling
- Deploy multiple server instances behind a load balancer
- Use a shared database for all instances
- Implement database replication for read scalability
- Consider using Redis for session management

---

## Future Enhancements

- [ ] WebSocket support for real-time updates
- [ ] OAuth2 authentication
- [ ] Query result pagination
- [ ] Advanced filtering and sorting
- [ ] Full-text search capabilities
- [ ] File upload support for item images
- [ ] API rate limiting
- [ ] Request logging and analytics
- [ ] OpenAPI/Swagger documentation generation
- [ ] GraphQL endpoint support
- [ ] Batch operations for bulk updates
- [ ] Database backup/restore endpoints

---

## Recent Updates (v0.3.0)

### October 23, 2025
- ✅ Integrated cpp-httplib v0.15.3 for HTTP server
- ✅ Integrated nlohmann/json v3.11.3 for JSON parsing
- ✅ Implemented all route handlers with modular architecture
- ✅ Added JSONSerializer and JSONDeserializer
- ✅ Completed APIDatabase HTTP client
- ✅ Fixed UUID preservation in client-server communication
- ✅ Added comprehensive test suite (invelog_server_test.exe)
- ✅ Verified data persistence with LocalDatabase
- ✅ Zero compilation errors/warnings
- ✅ End-to-end testing successful

---

## Support

For issues, questions, or contributions, please refer to the main project documentation.

**Project**: Invelog  
**Version**: 0.3.0 (Modular Architecture)  
**Last Updated**: October 23, 2025  
**Status**: ✅ Production Ready
