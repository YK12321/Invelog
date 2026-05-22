# Invelog Search API Documentation

## Overview

The Invelog system provides comprehensive search capabilities through multiple endpoints, ranging from simple text queries to advanced filtered searches. All search endpoints support authentication via API keys if enabled.

---

## Authentication

All endpoints require authentication if the server is running with `--auth` enabled:

```bash
# API Key in header
X-API-Key: your-api-key-here

# OR Bearer token
Authorization: Bearer your-api-key-here
```

---

## Search Endpoints

### 1. Simple Item Search

Quick search across item names and descriptions.

**Endpoint**: `GET /api/search/items`

**Query Parameters**:
- `q` (required): Search query string

**Example Request**:
```bash
curl -X GET "http://localhost:8080/api/search/items?q=resistor" \
  -H "X-API-Key: your-api-key"
```

**Example Response**:
```json
[
  {
    "id": "550e8400-e29b-41d4-a716-446655440000",
    "name": "10k Resistor",
    "description": "Carbon film resistor 10kΩ",
    "quantity": 100,
    "category": {
      "id": "660e8400-e29b-41d4-a716-446655440000",
      "name": "Electronics"
    },
    "checkedOut": false,
    "createdAt": "2025-10-20T10:30:00Z"
  }
]
```

---

### 2. Advanced Search (Multi-Filter)

Powerful search with multiple filters, sorting, and pagination.

**Endpoint**: `POST /api/search`

**Request Body** (all fields optional):
```json
{
  "query": "string",           // Text search in name/description
  "categoryId": "uuid",         // Filter by category
  "locationId": "uuid",         // Filter by location
  "projectId": "uuid",          // Filter by project
  "containerId": "uuid",        // Filter by container
  "checkedOut": boolean,        // Filter by checked out status
  "minQuantity": number,        // Minimum quantity
  "maxQuantity": number,        // Maximum quantity
  "sortBy": "string",           // Sort field: "name", "quantity"
  "descending": boolean,        // Sort order (default: false)
  "limit": number               // Maximum results to return
}
```

**Example Request** (find all checked-out items in a location):
```bash
curl -X POST http://localhost:8080/api/search \
  -H "Content-Type: application/json" \
  -H "X-API-Key: your-api-key" \
  -d '{
    "locationId": "770e8400-e29b-41d4-a716-446655440000",
    "checkedOut": true,
    "sortBy": "name"
  }'
```

**Example Request** (low stock items in a category):
```bash
curl -X POST http://localhost:8080/api/search \
  -H "Content-Type: application/json" \
  -H "X-API-Key: your-api-key" \
  -d '{
    "categoryId": "660e8400-e29b-41d4-a716-446655440000",
    "maxQuantity": 10,
    "sortBy": "quantity",
    "descending": false
  }'
```

**Example Response**:
```json
[
  {
    "id": "550e8400-e29b-41d4-a716-446655440000",
    "name": "Capacitor 100uF",
    "quantity": 5,
    "category": { "id": "...", "name": "Electronics" },
    "checkedOut": false
  },
  {
    "id": "551e8400-e29b-41d4-a716-446655440001",
    "name": "LED Red 5mm",
    "quantity": 8,
    "category": { "id": "...", "name": "Electronics" },
    "checkedOut": false
  }
]
```

---

### 3. Search by Category

Find all items in a specific category.

**Endpoint**: `GET /api/search/category/:categoryId`

**Path Parameters**:
- `categoryId` (required): UUID of the category

**Example Request**:
```bash
curl -X GET "http://localhost:8080/api/search/category/660e8400-e29b-41d4-a716-446655440000" \
  -H "X-API-Key: your-api-key"
```

**Example Response**:
```json
[
  {
    "id": "550e8400-e29b-41d4-a716-446655440000",
    "name": "10k Resistor",
    "category": {
      "id": "660e8400-e29b-41d4-a716-446655440000",
      "name": "Electronics",
      "description": "Electronic components"
    },
    "quantity": 100
  }
]
```

---

### 4. Search by Location

Find all items at a specific location (across all containers).

**Endpoint**: `GET /api/search/location/:locationId`

**Path Parameters**:
- `locationId` (required): UUID of the location

**Example Request**:
```bash
curl -X GET "http://localhost:8080/api/search/location/770e8400-e29b-41d4-a716-446655440000" \
  -H "X-API-Key: your-api-key"
```

**Example Response**:
```json
[
  {
    "id": "550e8400-e29b-41d4-a716-446655440000",
    "name": "10k Resistor",
    "quantity": 100,
    "currentContainer": {
      "id": "880e8400-e29b-41d4-a716-446655440000",
      "name": "Parts Bin A1",
      "location": {
        "id": "770e8400-e29b-41d4-a716-446655440000",
        "name": "Workshop"
      }
    }
  }
]
```

---

### 5. Search by Project

Find all items allocated to a project.

**Endpoint**: `GET /api/search/project/:projectId`

**Path Parameters**:
- `projectId` (required): UUID of the project

**Example Request**:
```bash
curl -X GET "http://localhost:8080/api/search/project/990e8400-e29b-41d4-a716-446655440000" \
  -H "X-API-Key: your-api-key"
```

**Example Response**:
```json
[
  {
    "id": "550e8400-e29b-41d4-a716-446655440000",
    "name": "Arduino Uno",
    "quantity": 2,
    "category": { "id": "...", "name": "Microcontrollers" }
  }
]
```

---

### 6. Search by Container

Find all items in a specific container.

**Endpoint**: `GET /api/search/container/:containerId`

**Path Parameters**:
- `containerId` (required): UUID of the container

**Example Request**:
```bash
curl -X GET "http://localhost:8080/api/search/container/880e8400-e29b-41d4-a716-446655440000" \
  -H "X-API-Key: your-api-key"
```

**Example Response**:
```json
[
  {
    "id": "550e8400-e29b-41d4-a716-446655440000",
    "name": "10k Resistor",
    "quantity": 100,
    "currentContainer": {
      "id": "880e8400-e29b-41d4-a716-446655440000",
      "name": "Parts Bin A1"
    }
  }
]
```

---

## Common Use Cases

### 1. Find Low Stock Items
```bash
curl -X POST http://localhost:8080/api/search \
  -H "Content-Type: application/json" \
  -H "X-API-Key: your-api-key" \
  -d '{
    "maxQuantity": 10,
    "sortBy": "quantity"
  }'
```

### 2. Search Items by Name in Specific Location
```bash
curl -X POST http://localhost:8080/api/search \
  -H "Content-Type: application/json" \
  -H "X-API-Key: your-api-key" \
  -d '{
    "query": "resistor",
    "locationId": "770e8400-e29b-41d4-a716-446655440000"
  }'
```

### 3. Find All Checked Out Items
```bash
curl -X POST http://localhost:8080/api/search \
  -H "Content-Type: application/json" \
  -H "X-API-Key: your-api-key" \
  -d '{
    "checkedOut": true,
    "sortBy": "name"
  }'
```

### 4. Get Top 10 Most Abundant Items
```bash
curl -X POST http://localhost:8080/api/search \
  -H "Content-Type: application/json" \
  -H "X-API-Key: your-api-key" \
  -d '{
    "sortBy": "quantity",
    "descending": true,
    "limit": 10
  }'
```

### 5. Find Items in Project Within Quantity Range
```bash
curl -X POST http://localhost:8080/api/search \
  -H "Content-Type: application/json" \
  -H "X-API-Key: your-api-key" \
  -d '{
    "projectId": "990e8400-e29b-41d4-a716-446655440000",
    "minQuantity": 5,
    "maxQuantity": 50
  }'
```

---

## Error Responses

All endpoints return standard HTTP error codes:

**400 Bad Request** - Invalid parameters
```json
{
  "error": "Missing 'q' query parameter"
}
```

**404 Not Found** - Resource not found
```json
{
  "error": "Container not found"
}
```

**401 Unauthorized** - Missing or invalid authentication
```json
{
  "error": "Authentication required"
}
```

**500 Internal Server Error** - Server error
```json
{
  "error": "Failed to perform search: <details>"
}
```

---

## Performance Considerations

1. **Simple Search** (`GET /api/search/items`) is the fastest for basic queries
2. **Advanced Search** (`POST /api/search`) with multiple filters may be slower on large datasets
3. Use the `limit` parameter to paginate results
4. Category/Location/Project/Container searches use optimized lookups

---

## Filter Combinations

The advanced search supports **AND** logic across all filters. Examples:

```json
{
  "query": "resistor",          // Name/description contains "resistor"
  "categoryId": "...",           // AND in Electronics category
  "locationId": "...",           // AND at Workshop location
  "checkedOut": false,           // AND not checked out
  "minQuantity": 10              // AND quantity >= 10
}
```

All conditions must be met for an item to appear in results.

---

## Client Examples

### JavaScript/Node.js
```javascript
const axios = require('axios');

async function searchItems(query) {
  const response = await axios.get('http://localhost:8080/api/search/items', {
    params: { q: query },
    headers: { 'X-API-Key': 'your-api-key' }
  });
  return response.data;
}

async function advancedSearch(filters) {
  const response = await axios.post('http://localhost:8080/api/search', filters, {
    headers: { 
      'X-API-Key': 'your-api-key',
      'Content-Type': 'application/json'
    }
  });
  return response.data;
}

// Usage
const results = await searchItems('resistor');
const lowStock = await advancedSearch({ maxQuantity: 10, sortBy: 'quantity' });
```

### Python
```python
import requests

def search_items(query, api_key):
    response = requests.get(
        'http://localhost:8080/api/search/items',
        params={'q': query},
        headers={'X-API-Key': api_key}
    )
    return response.json()

def advanced_search(filters, api_key):
    response = requests.post(
        'http://localhost:8080/api/search',
        json=filters,
        headers={'X-API-Key': api_key}
    )
    return response.json()

# Usage
results = search_items('resistor', 'your-api-key')
low_stock = advanced_search({'maxQuantity': 10, 'sortBy': 'quantity'}, 'your-api-key')
```

---

## Future Enhancements

Planned improvements for search capabilities:

- [ ] Fuzzy matching for typo tolerance
- [ ] Full-text search with ranking
- [ ] Search history and saved queries
- [ ] Autocomplete suggestions
- [ ] Search across activity logs
- [ ] Advanced date range filters
- [ ] Regular expression support

---

**Last Updated**: October 2025  
**Version**: 0.3.1
