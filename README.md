# Invelog

> A highly-optimized, scalable REST API for managing electronic component inventories.

## Overview

Invelog is a robust inventory management backend designed to track electronic components across multiple locations, containers, and projects. Completely rewritten in Go for maximum efficiency, it features a small memory footprint capable of running on low-powered embedded hardware (like a Raspberry Pi), while scaling effortlessly to support massive enterprise deployments.

## Key Features

- **Hierarchical Organization**: Manage inventory across Locations, Containers, and Subcontainers.
- **Item & ItemType Tracking**: Maintain shared component specifications (`ItemType`) while tracking individual physical units (`Item`).
- **Project Allocation**: Assign specific containers and components to projects.
- **Check-In/Check-Out**: Robust tracking of when items are deployed or retrieved.
- **Comprehensive Activity Logging**: Auto-generated audit trails for every movement, creation, and modification.
- **Advanced Search API**: Instantly query your inventory across multiple fields and relations.
- **Auto-Generated Documentation**: Integrated Swagger UI to visualize and interact with the API.
- **Multi-Database Support**: Seamlessly switch between SQLite (default local storage), PostgreSQL, and MySQL using GORM.

## Tech Stack

- **Language**: [Go](https://go.dev/)
- **Web Framework**: [Gin](https://gin-gonic.com/)
- **ORM / Database Layer**: [GORM](https://gorm.io/)
- **API Documentation**: [Swaggo](https://github.com/swaggo/swag)

## Getting Started

### Prerequisites

- Go 1.22+ installed on your system.

### Build and Run

1. Clone the repository and navigate to the project directory:
   ```bash
   git clone https://github.com/YK12321/Invelog
   cd invelog
   ```

2. Tidy dependencies:
   ```bash
   go mod tidy
   ```

3. Build the server:
   ```bash
   go build -o bin/invelog cmd/invelog/main.go
   ```

4. Run the server:
   ```bash
   DB_NAME=invelog.db ./bin/invelog
   ```
   By default, the server will start on `localhost:8080`. Note that a `DB_NAME` must be explicitly provided.

### API Documentation

Once the server is running, you can access the interactive Swagger UI at:

```
http://localhost:8080/swagger/index.html
```

*(Note: If you modify the API, you must regenerate the documentation using `swag init -g cmd/invelog/main.go --parseDependency --parseInternal`)*

## Configuration

Invelog uses environment variables to configure the database and server port.

| Variable | Description | Default |
|----------|-------------|---------|
| `PORT` | The port the HTTP server binds to | `8080` |
| `DB_TYPE` | Database driver (`sqlite`, `postgres`, `mysql`) | `sqlite` |
| `DB_NAME` | Database name or SQLite file path | **(Required)** |
| `DB_HOST` | Database host | `localhost` |
| `DB_PORT` | Database port | `5432` |
| `DB_USER` | Database user | `postgres` |
| `DB_PASSWORD` | Database password | `postgres` |
| `DB_SSLMODE` | SSL Mode for Postgres | `disable` |

### Example: Running with PostgreSQL

```bash
export DB_TYPE=postgres
export DB_HOST=localhost
export DB_PORT=5432
export DB_USER=myuser
export DB_PASSWORD=mypassword
export DB_NAME=invelog_db

./bin/invelog
```

## Architecture

This project strictly adheres to a modular design.
- `cmd/invelog`: Application entrypoint. Connects to the DB, runs migrations, and starts the Gin server.
- `pkg/api`: Contains the REST handlers and routing logic.
- `pkg/database`: Configures the GORM dialectors.
- `pkg/models`: Defines the data structures and GORM annotations.
- `docs/`: Auto-generated Swagger specifications.
