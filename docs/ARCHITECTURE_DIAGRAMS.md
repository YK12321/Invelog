# Invelog System Architecture Diagrams

## 1. Overall System Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                         CLIENT LAYER                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐            │
│  │  Desktop App │  │   Web App    │  │  Mobile App  │            │
│  │  (C++)       │  │  (Future)    │  │  (Future)    │            │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘            │
│         │                  │                  │                     │
│         └──────────────────┴──────────────────┘                     │
│                            │                                        │
│                    InventoryManager                                 │
│                            │                                        │
│         ┌──────────────────┴──────────────────┐                    │
│         │                                      │                    │
│    LocalDatabase                          APIDatabase               │
│    (Direct File Access)              (HTTP Client - Network)        │
│         │                                      │                    │
└─────────┼──────────────────────────────────────┼────────────────────┘
          │                                      │
          │                                      │ HTTP/HTTPS
          │                                      │ REST API
          │                                      ▼
          │                    ┌─────────────────────────────────────┐
          │                    │       DATABASE SERVER               │
          │                    ├─────────────────────────────────────┤
          │                    │  invelog_server executable          │
          │                    │  • HTTP Server                      │
          │                    │  • REST API (30+ endpoints)         │
          │                    │  • Authentication (API Keys)        │
          │                    │  • Request Routing                  │
          │                    │  • JSON Serialization               │
          │                    │  Port: 8080 (configurable)          │
          │                    └──────────────┬──────────────────────┘
          │                                   │ IDatabase Interface
          │                                   │
          │                    ┌──────────────┴──────────────────┐
          │                    │                                  │
          ▼                    ▼                                  ▼
┌──────────────────┐  ┌──────────────────┐        ┌──────────────────┐
│  LocalDatabase   │  │   SQLDatabase    │        │   APIDatabase    │
│  (File System)   │  │   (SQL Server)   │        │ (Remote API)     │
├──────────────────┤  ├──────────────────┤        ├──────────────────┤
│ • JSON files     │  │ • PostgreSQL     │        │ • HTTP Client    │
│ • Directory-     │  │ • MySQL          │        │ • REST calls     │
│   based storage  │  │ • SQLite         │        │ • JSON payload   │
│ • Fast, simple   │  │ • MS SQL Server  │        │ • Cloud-native   │
│ • Single-user    │  │ • Transactions   │        │ • Proxying       │
│                  │  │ • Multi-user     │        │                  │
└──────────────────┘  └──────────────────┘        └──────────────────┘
```

---

## 2. Database Server Deployment Scenarios

### Scenario A: Local Development (Single Machine)

```
┌─────────────────────────────────────────────────────────┐
│              Developer's Computer                       │
│                                                         │
│  ┌─────────────┐           ┌──────────────────┐       │
│  │   Client    │  localhost│  Database Server │       │
│  │   App       ├──────────>│  (Port 8080)     │       │
│  │             │  HTTP     │                  │       │
│  └─────────────┘           └────────┬─────────┘       │
│                                     │                  │
│                            ┌────────▼─────────┐       │
│                            │  LocalDatabase   │       │
│                            │  ./data/         │       │
│                            └──────────────────┘       │
└─────────────────────────────────────────────────────────┘
```

Command:
```bash
invelog_server --local ./data --no-auth
```

---

### Scenario B: Team on Local Network (LAN)

```
┌──────────────┐                  ┌────────────────────────────┐
│  Client PC 1 │                  │    Server PC               │
│              │  192.168.1.100   │  (192.168.1.100)           │
│  Desktop App ├─────────────────>│                            │
└──────────────┘                  │  ┌──────────────────┐     │
                                  │  │ Database Server  │     │
┌──────────────┐   HTTP/8080     │  │ (Port 8080)      │     │
│  Client PC 2 ├─────────────────>│  └────────┬─────────┘     │
│              │                  │           │                │
│  Desktop App │                  │  ┌────────▼─────────┐     │
└──────────────┘                  │  │ LocalDatabase or │     │
                                  │  │ SQLite           │     │
┌──────────────┐                  │  └──────────────────┘     │
│  Client PC N ├─────────────────>│                            │
│              │                  └────────────────────────────┘
│  Desktop App │
└──────────────┘
```

Command:
```bash
invelog_server --local ./data --port 8080 --api-key teamKey123
```

---

### Scenario C: Production Cloud Deployment

```
                           Internet
                              │
                              │ HTTPS/443
                              ▼
┌─────────────────────────────────────────────────────────┐
│                    Cloud Server                         │
│  (AWS / Azure / GCP / VPS)                              │
│                                                         │
│  ┌──────────────────────────────────────────┐          │
│  │  nginx (Reverse Proxy)                   │          │
│  │  • SSL/TLS Termination                   │          │
│  │  • Load Balancing                        │          │
│  │  • Security Headers                      │          │
│  │  Port: 443 (HTTPS)                       │          │
│  └────────────────┬─────────────────────────┘          │
│                   │ HTTP/8080                           │
│                   ▼                                     │
│  ┌──────────────────────────────────────────┐          │
│  │  invelog_server                          │          │
│  │  • systemd service                       │          │
│  │  • API Key authentication                │          │
│  │  • Logging                               │          │
│  │  Port: 8080 (internal)                   │          │
│  └────────────────┬─────────────────────────┘          │
│                   │                                     │
│                   ▼                                     │
│  ┌──────────────────────────────────────────┐          │
│  │  PostgreSQL Database                     │          │
│  │  • Connection pooling                    │          │
│  │  • Backups                               │          │
│  │  • Replication                           │          │
│  │  Port: 5432 (internal)                   │          │
│  └──────────────────────────────────────────┘          │
└─────────────────────────────────────────────────────────┘
                              ▲
                              │ HTTPS
                              │
            ┌─────────────────┴─────────────────┐
            │                                    │
      ┌──────────┐                         ┌──────────┐
      │ Client 1 │                         │ Client N │
      │ (Office) │                         │  (Home)  │
      └──────────┘                         └──────────┘
```

URL: `https://inventory.example.com`

---

## 3. Request Flow

```
┌─────────────┐
│   Client    │
│ Application │
└──────┬──────┘
       │ 1. Client creates item via InventoryManager
       │    manager.createItem("Resistor", category, 100)
       ▼
┌─────────────────┐
│ InventoryManager│
│   (Facade)      │
└──────┬──────────┘
       │ 2. Calls database->saveItem(item)
       ▼
┌─────────────────┐
│  APIDatabase    │
│  (HTTP Client)  │
└──────┬──────────┘
       │ 3. Builds HTTP request
       │    POST /api/items
       │    Header: X-API-Key: myKey
       │    Body: {"name":"Resistor",...}
       ▼
       ║ ═══ NETWORK (HTTP) ═══
       ║
       ▼
┌─────────────────┐
│ DatabaseServer  │
│  (HTTP Server)  │
└──────┬──────────┘
       │ 4. Receives request
       │    - Checks authentication
       │    - Routes to handler: handleCreateItem()
       │    - Parses JSON body
       ▼
┌─────────────────┐
│  IDatabase      │
│ Implementation  │
│ (LocalDatabase/ │
│  SQLDatabase)   │
└──────┬──────────┘
       │ 5. Saves to backend
       │    - LocalDatabase: Write JSON file
       │    - SQLDatabase: INSERT INTO items...
       ▼
┌─────────────────┐
│   Storage       │
│ (File/SQL DB)   │
└─────────────────┘
       │
       │ 6. Response flows back
       ▼
   HTTP 201 Created
   {"id":"uuid","name":"Resistor",...}
```

---

## 4. Database Backend Options

```
                    ┌──────────────────┐
                    │ DatabaseServer   │
                    │ (Can use any     │
                    │  IDatabase impl) │
                    └────────┬─────────┘
                             │
            ┌────────────────┼────────────────┐
            │                │                │
            ▼                ▼                ▼
  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
  │LocalDatabase │  │ SQLDatabase  │  │ APIDatabase  │
  └──────────────┘  └──────────────┘  └──────────────┘
         │                 │                  │
         ▼                 │                  ▼
  ┌──────────────┐        │           ┌──────────────┐
  │ File System  │        │           │ Remote API   │
  │              │        │           │ Server       │
  │ ./data/      │        │           └──────────────┘
  │ ├─items/     │        │
  │ ├─containers/│        │
  │ └─...        │        │
  └──────────────┘        │
                          │
         ┌────────────────┴─────────────────┐
         │                                   │
         ▼                                   ▼
  ┌──────────────┐                   ┌──────────────┐
  │ PostgreSQL   │                   │    MySQL     │
  │              │                   │              │
  │ Port: 5432   │                   │ Port: 3306   │
  └──────────────┘                   └──────────────┘
         │                                   │
         ▼                                   ▼
  ┌──────────────┐                   ┌──────────────┐
  │    SQLite    │                   │  MS SQL      │
  │              │                   │  Server      │
  │ File-based   │                   │ Port: 1433   │
  └──────────────┘                   └──────────────┘
```

---

## 5. Multi-Client Concurrent Access

```
Time ──────────────────────────────────────────────>

Client 1:  [Create Item A]────────────┐
                                      │
Client 2:        [Search "Resistor"]──┼────┐
                                      │    │
Client 3:              [Update Item B]┼────┼─────┐
                                      │    │     │
                                      ▼    ▼     ▼
                            ┌──────────────────────┐
                            │   DatabaseServer     │
                            │  (Handles requests   │
                            │   concurrently)      │
                            └──────────┬───────────┘
                                       │
                                       ▼
                            ┌──────────────────────┐
                            │   Backend Database   │
                            │  (Thread-safe ops)   │
                            └──────────────────────┘
                                       │
                                       │
Response to Client 1: [201 Created]◀───┤
Response to Client 2: [200 OK, 5 items]◀┤
Response to Client 3: [200 OK]◀─────────┘
```

---

## 6. Security Layers

```
┌─────────────────────────────────────────────────────┐
│  1. Transport Layer (HTTPS)                         │
│     ─ SSL/TLS encryption                            │
│     ─ Certificate validation                        │
└─────────────────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────┐
│  2. Network Layer (Firewall)                        │
│     ─ Port restrictions                             │
│     ─ IP whitelisting (optional)                    │
└─────────────────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────┐
│  3. Reverse Proxy (nginx)                           │
│     ─ Rate limiting                                 │
│     ─ Request filtering                             │
│     ─ DDoS protection                               │
└─────────────────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────┐
│  4. Application Layer (DatabaseServer)              │
│     ─ API key validation                            │
│     ─ Request authentication                        │
│     ─ Authorization (future)                        │
└─────────────────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────┐
│  5. Database Layer                                  │
│     ─ SQL injection prevention                      │
│     ─ Prepared statements                           │
│     ─ Access control                                │
└─────────────────────────────────────────────────────┘
```

---

## 7. Scaling Architecture (Future)

```
                        Load Balancer
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐    ┌───────────────┐    ┌───────────────┐
│   Server 1    │    │   Server 2    │    │   Server 3    │
│ invelog_server│    │ invelog_server│    │ invelog_server│
│  (Port 8080)  │    │  (Port 8080)  │    │  (Port 8080)  │
└───────┬───────┘    └───────┬───────┘    └───────┬───────┘
        │                    │                    │
        └────────────────────┼────────────────────┘
                             │
                             ▼
                  ┌──────────────────────┐
                  │  PostgreSQL Master   │
                  │  (Write Operations)  │
                  └──────────┬───────────┘
                             │ Replication
                ┌────────────┼────────────┐
                │            │            │
                ▼            ▼            ▼
        ┌──────────┐  ┌──────────┐  ┌──────────┐
        │ Replica 1│  │ Replica 2│  │ Replica 3│
        │  (Read)  │  │  (Read)  │  │  (Read)  │
        └──────────┘  └──────────┘  └──────────┘
```

---

## 8. Development to Production Path

```
┌────────────────────────────────────────────────────────────┐
│  DEVELOPMENT                                               │
│  • Single machine                                          │
│  • LocalDatabase with --no-auth                            │
│  • http://localhost:8080                                   │
└────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌────────────────────────────────────────────────────────────┐
│  TEAM (LAN)                                                │
│  • Dedicated server on network                             │
│  • LocalDatabase or SQLite with --api-key                  │
│  • http://192.168.1.100:8080                               │
└────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌────────────────────────────────────────────────────────────┐
│  STAGING                                                   │
│  • Cloud server (small instance)                           │
│  • PostgreSQL database                                     │
│  • nginx + SSL certificate                                 │
│  • https://staging-inventory.example.com                   │
└────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌────────────────────────────────────────────────────────────┐
│  PRODUCTION                                                │
│  • High-availability cloud setup                           │
│  • PostgreSQL with replication                             │
│  • nginx + SSL + monitoring                                │
│  • systemd service with auto-restart                       │
│  • Backups + logging + alerts                              │
│  • https://inventory.example.com                           │
└────────────────────────────────────────────────────────────┘
```

---

## Legend

```
┌────────┐
│  Box   │  = Component/System
└────────┘

   │
   ▼        = Data/Control Flow

   ║
   ║        = Network Boundary

───────     = Connection/Relationship
```

---

**Version**: 0.2.0  
**Last Updated**: October 2025
