## 2026-05-23 - [Unbounded Query DoS]
**Vulnerability:** The `/api/v1/activity-logs` endpoint queried the entire `activity_logs` table using `Find()` without a limit or pagination, causing the result set to grow continuously. This leads to unbounded memory consumption on the server and excessive database load, constituting a Denial of Service (DoS) risk.
**Learning:** In a production application handling an ever-growing dataset (like audit logs), fetching all records at once should never be permitted. Even during initial development, endpoints accessing list resources must implement bounded limits to prevent system exhaustion under load.
**Prevention:** Always implement pagination (e.g., using limit and offset defaults) for list-based API endpoints. When possible, add hard upper limits on the maximum number of items that can be fetched in a single query to ensure resource usage remains predictable and safe.

## 2026-06-03 - [Mass Assignment via GORM Associations]
**Vulnerability:** Several REST endpoints (e.g., `CreateContainer`, `UpdateContainer`) were directly binding incoming JSON payloads to GORM domain models (`models.Container`). When database configurations enable `FullSaveAssociations` (common when migrating from SQLite to Postgres or modifying defaults), this allows attackers to pass nested JSON objects (like `{"location": {"name": "Hacked"}}`) and perform unauthorized mass assignment/modifications on related database records.
**Learning:** Directly binding HTTP requests to domain models that have relation mappings (like `Location`, `Project`, `Parent`) creates a dangerous mass assignment vector that might lay dormant until ORM settings or DB drivers are changed.
**Prevention:** Always use dedicated Data Transfer Objects (DTOs) for request parsing (e.g., in `pkg/dto/`) that only expose primitive scalar fields (e.g., `LocationID` instead of a full `Location` object) and strictly defined allowed updateable fields, then map these explicitly to domain models before saving.

## 2026-08-10 - [Hardcoded Fallback JWT Secret]
**Vulnerability:** The application used a hardcoded string fallback for the JWT secret if the JWT_SECRET environment variable was missing. This allows anyone with access to the source code to forge valid JWT tokens and bypass authentication.
**Learning:** Fallback secrets should never be deterministic or hardcoded strings, as it compromises the entire authentication system if the environment variable is accidentally omitted.
**Prevention:** When an environment variable required for cryptographic security is missing, fall back to a cryptographically strong, randomly generated ephemeral secret instantiated at startup (e.g., using crypto/rand in init()). This prevents forged token attacks while avoiding application crashes.
