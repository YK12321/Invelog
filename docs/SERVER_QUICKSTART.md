# Invelog Database Server - Quick Reference

## Starting the Server

### Local Development
```powershell
# File-based database (simplest)
invelog_server --local ./data --no-auth

# With authentication
invelog_server --local ./data --api-key mySecretKey
```

### Network/Team Usage
```powershell
# Start server accessible on local network
invelog_server --local ./data --port 8080 --api-key teamSharedKey

# Clients connect to: http://SERVER_IP:8080
```

### Production with PostgreSQL
```powershell
invelog_server --postgres "host=localhost dbname=invelog user=invelog_user password=strong_pass" --port 8080 --api-key PRODUCTION_KEY
```

### Production with MySQL
```powershell
invelog_server --mysql "host=localhost dbname=invelog user=invelog_user password=strong_pass" --port 8080 --api-key PRODUCTION_KEY
```

### SQLite (Embedded SQL)
```powershell
invelog_server --sqlite ./invelog.db --port 8080 --api-key myKey
```

---

## Command Line Options

| Option | Description | Example |
|--------|-------------|---------|
| `--local <path>` | Use file-based database | `--local ./data` |
| `--postgres <conn>` | Use PostgreSQL | `--postgres "host=localhost dbname=invelog"` |
| `--mysql <conn>` | Use MySQL | `--mysql "host=localhost dbname=invelog"` |
| `--sqlite <path>` | Use SQLite | `--sqlite ./invelog.db` |
| `--port <port>` | Set server port | `--port 8080` |
| `--api-key <key>` | Set API key | `--api-key mySecretKey` |
| `--no-auth` | Disable authentication | `--no-auth` |
| `--help` | Show help | `--help` |

---

## API Endpoints

### Health Check
```http
GET /api/health
```
Returns server status.

### Items
```http
GET    /api/items              # List all items
GET    /api/items/:id          # Get specific item
POST   /api/items              # Create item
PUT    /api/items/:id          # Update item
DELETE /api/items/:id          # Delete item
```

### Containers
```http
GET    /api/containers         # List all containers
GET    /api/containers/:id     # Get specific container
POST   /api/containers         # Create container
PUT    /api/containers/:id     # Update container
DELETE /api/containers/:id     # Delete container
```

### Locations, Projects, Categories
Same pattern as Items and Containers:
- `/api/locations`
- `/api/projects`
- `/api/categories`

### Activity Logs
```http
GET    /api/activity_logs      # List all logs
GET    /api/activity_logs/:id  # Get specific log
POST   /api/activity_logs      # Create log entry
```

### Search
```http
POST   /api/search             # Search with criteria
POST   /api/batch              # Batch operations
```

---

## Client Connection

### C++ Client (APIDatabase)

```cpp
#include "APIDatabase.h"
#include "InventoryManager.h"

// Configure connection
APIDatabase::APIConfig config;
config.baseUrl = "http://localhost:8080";
config.authType = APIDatabase::AuthType::API_KEY;
config.authToken = "mySecretKey";

// Create database and manager
auto database = std::make_shared<APIDatabase>(config);
InventoryManager manager(database);
manager.initialize();

// Use normally
auto item = manager.createItem("Resistor", category, 100);
```

### curl (Testing)

```bash
# Health check
curl http://localhost:8080/api/health

# With authentication
curl -H "X-API-Key: mySecretKey" http://localhost:8080/api/items

# Create item
curl -X POST http://localhost:8080/api/items \
  -H "Content-Type: application/json" \
  -H "X-API-Key: mySecretKey" \
  -d '{"name":"Resistor 1k","quantity":100}'
```

---

## Authentication Methods

### Option 1: Custom Header (Recommended)
```http
X-API-Key: your-api-key-here
```

### Option 2: Bearer Token
```http
Authorization: Bearer your-api-key-here
```

### Option 3: No Authentication (Dev Only)
Start server with `--no-auth`

---

## Common Use Cases

### 1. Single Developer (No Network)
```powershell
invelog_server --local ./data --no-auth
```
Access: `http://localhost:8080`

### 2. Small Team (Same Network)
```powershell
invelog_server --local ./data --port 8080 --api-key teamKey123
```
Access: `http://192.168.1.100:8080` (your server IP)

### 3. Production (PostgreSQL + nginx)
```bash
# Start server (managed by systemd)
sudo systemctl start invelog-server

# nginx provides HTTPS
# Access: https://inventory.example.com
```

---

## Troubleshooting

### Port Already in Use
```powershell
# Check what's using port 8080
netstat -ano | findstr :8080

# Use different port
invelog_server --local ./data --port 9090
```

### Can't Connect from Other Machines
1. Check firewall allows port 8080
2. Use correct server IP address
3. Verify server is running: `curl http://localhost:8080/api/health`
4. Check API key matches

### Authentication Errors
- Verify API key is correct
- Check header format: `X-API-Key: yourkey` or `Authorization: Bearer yourkey`
- For testing, use `--no-auth` (not for production!)

### Database Connection Failed
```powershell
# LocalDatabase: Check directory exists and is writable
# PostgreSQL/MySQL: Verify database is running and credentials are correct
# SQLite: Check file path and permissions
```

---

## Security Notes

⚠️ **Development vs Production**

**Development** (OK):
- `--no-auth` for quick testing
- `http://localhost`
- Simple API keys

**Production** (REQUIRED):
- Strong API keys (32+ characters)
- HTTPS only (use nginx reverse proxy)
- Firewall configured
- Regular backups
- Monitor logs

---

## Performance Tips

### For Many Clients
- Use PostgreSQL or MySQL (not LocalDatabase)
- Enable connection pooling (future feature)
- Use nginx as reverse proxy
- Monitor with `systemctl status invelog-server`

### For Large Datasets
- Add database indexes
- Use pagination in queries (future feature)
- Consider caching frequently accessed data

---

## File Locations

### Server Executable
- Windows: `.\bin\Release\invelog_server.exe`
- Linux: `./bin/invelog_server`

### Data Storage
- LocalDatabase: `./data/` (or path specified with `--local`)
- SQLite: Path specified with `--sqlite`
- PostgreSQL/MySQL: In database server

### Logs
- stdout/stderr (run in terminal to see)
- systemd: `journalctl -u invelog-server -f`

---

## Next Steps

1. **Test locally**: Start with `--local ./data --no-auth`
2. **Add authentication**: Use `--api-key` for security
3. **Network access**: Configure firewall and connect from other machines
4. **Production deploy**: Follow [DEPLOYMENT_GUIDE.md](DEPLOYMENT_GUIDE.md)
5. **Monitor**: Set up logging and health checks

---

## Full Documentation

- **[API Reference](API_DOCUMENTATION.md)**: Complete API documentation
- **[Deployment Guide](DEPLOYMENT_GUIDE.md)**: Production deployment steps
- **[Database Guide](DATABASE_GUIDE.md)**: Database configuration

---

## Implementation Status

✅ **FULLY FUNCTIONAL** - All features are now working!

- ✅ HTTP server using cpp-httplib v0.15.3
- ✅ JSON serialization/deserialization using nlohmann/json v3.11.3
- ✅ Complete REST API with all endpoints
- ✅ Client-server communication tested and verified
- ✅ Data persistence working correctly
- ✅ Authentication and CORS support
- ✅ Error handling and status codes
- ✅ Health check and monitoring endpoints

The database server is production-ready and fully tested!

---

**Version**: 0.3.0 (Modular Architecture)  
**Last Updated**: October 23, 2025
