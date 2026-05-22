# Database Server Deployment Guide

## Overview

This guide covers deploying the Invelog Database Server as a standalone service that can be accessed by multiple clients over the network.

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Deployment Options](#deployment-options)
3. [Local Deployment](#local-deployment)
4. [Network Deployment](#network-deployment)
5. [Production Deployment](#production-deployment)
6. [Security Best Practices](#security-best-practices)
7. [Monitoring and Maintenance](#monitoring-and-maintenance)
8. [Troubleshooting](#troubleshooting)

---

## Architecture Overview

```
┌─────────────┐      HTTP/HTTPS      ┌──────────────────┐
│   Client 1  │──────────────────────>│                  │
└─────────────┘                       │  Database        │      ┌──────────────┐
                                      │  Server          │─────>│  Backend     │
┌─────────────┐      HTTP/HTTPS      │  (Port 8080)     │      │  Database    │
│   Client 2  │──────────────────────>│                  │      └──────────────┘
└─────────────┘                       │  - REST API      │       (LocalDB/SQL/
                                      │  - Authentication│        API)
┌─────────────┐      HTTP/HTTPS      │  - Rate Limiting │
│   Client N  │──────────────────────>│  - CORS Support  │
└─────────────┘                       └──────────────────┘
```

**Key Components:**
- **Database Server**: HTTP server that exposes REST API endpoints
- **Backend Database**: Can be LocalDatabase, SQLDatabase, or another APIDatabase
- **Clients**: Applications using APIDatabase class to connect

---

## Deployment Options

### Option 1: Same Machine (Development)
- Server and client on the same computer
- Use `localhost` or `127.0.0.1`
- Fast, low latency
- Good for testing and development

### Option 2: Local Network (Small Team)
- Server on one machine, clients on LAN
- Use local IP address (e.g., `192.168.1.100`)
- Shared inventory across team
- No internet required

### Option 3: Cloud Deployment (Production)
- Server on cloud platform (AWS, Azure, GCP)
- Clients access via public URL
- Scalable and accessible anywhere
- Requires proper security

### Option 4: Container Deployment (Modern)
- Server in Docker container
- Easy deployment and scaling
- Kubernetes orchestration
- Cloud-native approach

---

## Local Deployment

### Building the Server

```powershell
# Windows (PowerShell)
cd "path\to\Invelog"
mkdir build
cd build
cmake ..
cmake --build . --config Release

# Server executable will be at: .\bin\Release\invelog_server.exe
```

```bash
# Linux/macOS
cd /path/to/Invelog
mkdir build
cd build
cmake ..
cmake --build .

# Server executable will be at: ./bin/invelog_server
```

### Running the Server Locally

#### With Local Database (File-based)
```powershell
# Windows
.\bin\Release\invelog_server.exe --local .\data --port 8080 --api-key mySecretKey123
```

```bash
# Linux/macOS
./bin/invelog_server --local ./data --port 8080 --api-key mySecretKey123
```

#### With SQLite (Embedded)
```powershell
.\bin\Release\invelog_server.exe --sqlite .\invelog.db --port 8080 --api-key mySecretKey123
```

#### Without Authentication (Development Only!)
```powershell
.\bin\Release\invelog_server.exe --local .\data --no-auth
```

### Testing the Server

```powershell
# Test health endpoint
curl http://localhost:8080/api/health

# Expected response:
# {"status":"healthy","version":"0.2.0","database":"connected","timestamp":...}
```

---

## Network Deployment

### Step 1: Configure Firewall

#### Windows Firewall
```powershell
# Allow incoming connections on port 8080
New-NetFirewallRule -DisplayName "Invelog Server" -Direction Inbound -LocalPort 8080 -Protocol TCP -Action Allow
```

#### Linux (ufw)
```bash
sudo ufw allow 8080/tcp
sudo ufw reload
```

### Step 2: Find Server IP Address

#### Windows
```powershell
ipconfig
# Look for IPv4 Address (e.g., 192.168.1.100)
```

#### Linux/macOS
```bash
ifconfig
# or
ip addr show
```

### Step 3: Start Server with Network Access

```powershell
# Start server that listens on all network interfaces
.\bin\Release\invelog_server.exe --local .\data --port 8080 --api-key secureKey456
```

### Step 4: Connect from Client

```cpp
// Client configuration
APIDatabase::APIConfig config;
config.baseUrl = "http://192.168.1.100:8080";  // Server IP
config.authType = APIDatabase::AuthType::API_KEY;
config.authToken = "secureKey456";

auto database = std::make_shared<APIDatabase>(config);
database->initialize();
```

### Testing Network Connection

```powershell
# From client machine
curl http://192.168.1.100:8080/api/health
```

---

## Production Deployment

### Prerequisites

1. **Server Requirements**:
   - 2+ CPU cores
   - 4+ GB RAM
   - 20+ GB storage
   - Linux recommended (Ubuntu 20.04/22.04)

2. **Database Backend**:
   - PostgreSQL 12+ (recommended)
   - MySQL 8+
   - Or high-performance file system for LocalDatabase

3. **Network**:
   - Static IP or domain name
   - SSL/TLS certificate
   - Reverse proxy (nginx or Apache)

### Step 1: Set Up PostgreSQL

```bash
# Install PostgreSQL
sudo apt update
sudo apt install postgresql postgresql-contrib

# Create database and user
sudo -u postgres psql
CREATE DATABASE invelog;
CREATE USER invelog_user WITH PASSWORD 'strong_password_here';
GRANT ALL PRIVILEGES ON DATABASE invelog TO invelog_user;
\q
```

### Step 2: Configure nginx as Reverse Proxy

```bash
# Install nginx
sudo apt install nginx

# Create configuration
sudo nano /etc/nginx/sites-available/invelog
```

```nginx
server {
    listen 80;
    server_name inventory.example.com;

    # Redirect to HTTPS
    return 301 https://$server_name$request_uri;
}

server {
    listen 443 ssl http2;
    server_name inventory.example.com;

    ssl_certificate /etc/letsencrypt/live/inventory.example.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/inventory.example.com/privkey.pem;

    # Security headers
    add_header Strict-Transport-Security "max-age=31536000" always;
    add_header X-Content-Type-Options "nosniff" always;
    add_header X-Frame-Options "DENY" always;

    location / {
        proxy_pass http://localhost:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        
        # WebSocket support (future)
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
    }
}
```

```bash
# Enable site
sudo ln -s /etc/nginx/sites-available/invelog /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl restart nginx
```

### Step 3: Set Up SSL Certificate

```bash
# Install certbot
sudo apt install certbot python3-certbot-nginx

# Get certificate
sudo certbot --nginx -d inventory.example.com
```

### Step 4: Create systemd Service

```bash
sudo nano /etc/systemd/system/invelog-server.service
```

```ini
[Unit]
Description=Invelog Database Server
After=network.target postgresql.service

[Service]
Type=simple
User=invelog
Group=invelog
WorkingDirectory=/opt/invelog
ExecStart=/opt/invelog/bin/invelog_server --postgres "host=localhost dbname=invelog user=invelog_user password=strong_password_here" --port 8080 --api-key PRODUCTION_KEY_HERE
Restart=on-failure
RestartSec=10

# Security
NoNewPrivileges=true
PrivateTmp=true
ProtectSystem=strict
ProtectHome=true
ReadWritePaths=/opt/invelog/data

[Install]
WantedBy=multi-user.target
```

```bash
# Create user and directories
sudo useradd -r -s /bin/false invelog
sudo mkdir -p /opt/invelog
sudo chown invelog:invelog /opt/invelog

# Copy built server
sudo cp -r ./build/bin /opt/invelog/
sudo chown -R invelog:invelog /opt/invelog

# Start service
sudo systemctl daemon-reload
sudo systemctl enable invelog-server
sudo systemctl start invelog-server

# Check status
sudo systemctl status invelog-server
```

### Step 5: Configure Client for Production

```cpp
APIDatabase::APIConfig config;
config.baseUrl = "https://inventory.example.com";
config.authType = APIDatabase::AuthType::BEARER_TOKEN;
config.authToken = "PRODUCTION_KEY_HERE";
config.timeout = 30;
config.maxRetries = 3;
config.verifySsl = true;  // Enable SSL verification in production!

auto database = std::make_shared<APIDatabase>(config);
```

---

## Security Best Practices

### 1. Authentication

```bash
# Generate strong API key (Linux/macOS)
openssl rand -base64 32

# Windows (PowerShell)
$bytes = New-Object byte[] 32
[Security.Cryptography.RNGCryptoServiceProvider]::Create().GetBytes($bytes)
[Convert]::ToBase64String($bytes)
```

**Key Management**:
- Use environment variables for keys
- Rotate keys regularly
- Never commit keys to version control
- Use different keys for dev/staging/production

### 2. Network Security

- **Always use HTTPS in production**
- Use firewall to restrict access
- Implement rate limiting
- Use VPN for internal deployments
- Monitor for unusual traffic patterns

### 3. Database Security

- Use strong database passwords
- Enable SSL for database connections
- Restrict database network access
- Regular backups
- Keep software updated

### 4. Application Security

```cpp
// Enable security features in client
APIDatabase::APIConfig config;
config.verifySsl = true;           // Verify SSL certificates
config.timeout = 30;               // Timeout for requests
config.maxRetries = 3;             // Limit retry attempts
config.customHeaders["User-Agent"] = "InvelogClient/1.0";
```

---

## Monitoring and Maintenance

### Logging

```bash
# View server logs
sudo journalctl -u invelog-server -f

# View nginx logs
sudo tail -f /var/log/nginx/access.log
sudo tail -f /var/log/nginx/error.log
```

### Health Monitoring

Create a monitoring script:

```bash
#!/bin/bash
# monitor_invelog.sh

HEALTH_URL="http://localhost:8080/api/health"
ALERT_EMAIL="admin@example.com"

response=$(curl -s -o /dev/null -w "%{http_code}" $HEALTH_URL)

if [ "$response" != "200" ]; then
    echo "Invelog server is DOWN! Response: $response" | mail -s "Server Alert" $ALERT_EMAIL
    systemctl restart invelog-server
fi
```

Add to cron:
```bash
# Run every 5 minutes
*/5 * * * * /opt/invelog/monitor_invelog.sh
```

### Backup Strategy

```bash
#!/bin/bash
# backup_invelog.sh

BACKUP_DIR="/backup/invelog"
DATE=$(date +%Y%m%d_%H%M%S)

# Backup PostgreSQL
sudo -u postgres pg_dump invelog > "$BACKUP_DIR/invelog_$DATE.sql"

# Backup local data if using LocalDatabase
tar -czf "$BACKUP_DIR/data_$DATE.tar.gz" /opt/invelog/data/

# Keep only last 30 days
find $BACKUP_DIR -name "invelog_*.sql" -mtime +30 -delete
find $BACKUP_DIR -name "data_*.tar.gz" -mtime +30 -delete
```

---

## Troubleshooting

### Server Won't Start

```bash
# Check if port is already in use
netstat -tuln | grep 8080

# Kill process using port
kill -9 $(lsof -t -i:8080)

# Check server logs
journalctl -u invelog-server -n 50

# Verify database connection
psql -h localhost -U invelog_user -d invelog
```

### Clients Can't Connect

```bash
# Test from server itself
curl http://localhost:8080/api/health

# Test from client machine
curl http://SERVER_IP:8080/api/health

# Check firewall
sudo ufw status
sudo iptables -L -n

# Check nginx
sudo nginx -t
sudo systemctl status nginx
```

### Slow Performance

1. **Check database**:
```sql
-- PostgreSQL: Check slow queries
SELECT * FROM pg_stat_statements 
ORDER BY mean_exec_time DESC 
LIMIT 10;
```

2. **Check server resources**:
```bash
# CPU and memory
htop

# Disk I/O
iostat -x 1

# Network
iftop
```

3. **Enable connection pooling** (for SQL databases)

4. **Add database indexes** for frequently queried fields

### Database Connection Errors

```bash
# Check PostgreSQL is running
sudo systemctl status postgresql

# Check connection limits
sudo -u postgres psql
SHOW max_connections;

# Increase if needed
sudo nano /etc/postgresql/14/main/postgresql.conf
# Set: max_connections = 200
sudo systemctl restart postgresql
```

---

## Docker Deployment (Optional)

### Dockerfile

```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir build && cd build && \
    cmake .. && \
    cmake --build . --config Release

EXPOSE 8080

CMD ["./build/bin/invelog_server", "--postgres", "host=db dbname=invelog user=invelog password=password", "--port", "8080", "--api-key", "DOCKER_KEY"]
```

### docker-compose.yml

```yaml
version: '3.8'

services:
  db:
    image: postgres:14
    environment:
      POSTGRES_DB: invelog
      POSTGRES_USER: invelog
      POSTGRES_PASSWORD: password
    volumes:
      - postgres_data:/var/lib/postgresql/data

  server:
    build: .
    ports:
      - "8080:8080"
    depends_on:
      - db
    environment:
      DB_HOST: db
      DB_NAME: invelog
      DB_USER: invelog
      DB_PASS: password
      API_KEY: DOCKER_KEY

volumes:
  postgres_data:
```

```bash
# Deploy
docker-compose up -d

# Check logs
docker-compose logs -f server

# Stop
docker-compose down
```

---

## Performance Tuning

### Server Configuration

```cpp
// In DatabaseServer.cpp, add connection pooling:
// TODO: Implement connection pool for concurrent requests
```

### Database Optimization

```sql
-- PostgreSQL: Add indexes
CREATE INDEX idx_items_name ON items(name);
CREATE INDEX idx_items_category ON items(category_id);
CREATE INDEX idx_containers_location ON containers(location_id);
CREATE INDEX idx_activity_logs_item ON activity_logs(item_id);
CREATE INDEX idx_activity_logs_timestamp ON activity_logs(timestamp);
```

### nginx Tuning

```nginx
# In nginx.conf
worker_processes auto;
worker_connections 1024;

# Enable caching
proxy_cache_path /var/cache/nginx levels=1:2 keys_zone=api_cache:10m max_size=100m;

location /api/ {
    proxy_cache api_cache;
    proxy_cache_valid 200 60s;
    proxy_pass http://localhost:8080;
}
```

---

## Next Steps

1. **Integrate HTTP Library**: Replace placeholder HTTP client with libcurl or cpp-httplib
2. **Add WebSocket Support**: For real-time updates
3. **Implement Connection Pooling**: For better performance
4. **Add Metrics Endpoint**: For monitoring (Prometheus-compatible)
5. **Create Admin Panel**: Web interface for server management

---

## Support

For issues or questions:
- Check server logs: `journalctl -u invelog-server`
- Test API manually with curl
- Review nginx logs
- Check database connectivity

**Last Updated**: October 2025  
**Version**: 0.2.0
