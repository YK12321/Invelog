## 2026-05-23 - [Unbounded Query DoS]
**Vulnerability:** The `/api/v1/activity-logs` endpoint queried the entire `activity_logs` table using `Find()` without a limit or pagination, causing the result set to grow continuously. This leads to unbounded memory consumption on the server and excessive database load, constituting a Denial of Service (DoS) risk.
**Learning:** In a production application handling an ever-growing dataset (like audit logs), fetching all records at once should never be permitted. Even during initial development, endpoints accessing list resources must implement bounded limits to prevent system exhaustion under load.
**Prevention:** Always implement pagination (e.g., using limit and offset defaults) for list-based API endpoints. When possible, add hard upper limits on the maximum number of items that can be fetched in a single query to ensure resource usage remains predictable and safe.

## 2026-06-03 - [Mass Assignment via GORM Associations]
**Vulnerability:** Several REST endpoints (e.g., `CreateContainer`, `UpdateContainer`) were directly binding incoming JSON payloads to GORM domain models (`models.Container`). When database configurations enable `FullSaveAssociations` (common when migrating from SQLite to Postgres or modifying defaults), this allows attackers to pass nested JSON objects (like `{"location": {"name": "Hacked"}}`) and perform unauthorized mass assignment/modifications on related database records.
**Learning:** Directly binding HTTP requests to domain models that have relation mappings (like `Location`, `Project`, `Parent`) creates a dangerous mass assignment vector that might lay dormant until ORM settings or DB drivers are changed.
**Prevention:** Always use dedicated Data Transfer Objects (DTOs) for request parsing (e.g., in `pkg/dto/`) that only expose primitive scalar fields (e.g., `LocationID` instead of a full `Location` object) and strictly defined allowed updateable fields, then map these explicitly to domain models before saving.
## 2024-08-04 - Fix hardcoded JWT secret

**Vulnerability:** A hardcoded, highly predictable JWT secret fallback (`"invelog-default-secret-key-change-in-prod"`) was present in `pkg/api/middleware/auth.go`.
**Learning:** Hardcoded fallbacks are extremely dangerous and can allow attackers to easily forge admin tokens in environments that omit setting the required configuration variables.
**Prevention:** If an environment variable is required for cryptographic security but absent, always use a cryptographically strong, randomly generated ephemeral secret. This fail-safe approach means tokens invalidate upon server restart, but the system remains secure against token forging attacks without causing an immediate application crash.
