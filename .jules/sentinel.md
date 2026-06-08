## 2026-05-23 - [Unbounded Query DoS]
**Vulnerability:** The `/api/v1/activity-logs` endpoint queried the entire `activity_logs` table using `Find()` without a limit or pagination, causing the result set to grow continuously. This leads to unbounded memory consumption on the server and excessive database load, constituting a Denial of Service (DoS) risk.
**Learning:** In a production application handling an ever-growing dataset (like audit logs), fetching all records at once should never be permitted. Even during initial development, endpoints accessing list resources must implement bounded limits to prevent system exhaustion under load.
**Prevention:** Always implement pagination (e.g., using limit and offset defaults) for list-based API endpoints. When possible, add hard upper limits on the maximum number of items that can be fetched in a single query to ensure resource usage remains predictable and safe.

## 2026-06-03 - [Mass Assignment via GORM Associations]
**Vulnerability:** Several REST endpoints (e.g., `CreateContainer`, `UpdateContainer`) were directly binding incoming JSON payloads to GORM domain models (`models.Container`). When database configurations enable `FullSaveAssociations` (common when migrating from SQLite to Postgres or modifying defaults), this allows attackers to pass nested JSON objects (like `{"location": {"name": "Hacked"}}`) and perform unauthorized mass assignment/modifications on related database records.
**Learning:** Directly binding HTTP requests to domain models that have relation mappings (like `Location`, `Project`, `Parent`) creates a dangerous mass assignment vector that might lay dormant until ORM settings or DB drivers are changed.
**Prevention:** Always use dedicated Data Transfer Objects (DTOs) for request parsing (e.g., in `pkg/dto/`) that only expose primitive scalar fields (e.g., `LocationID` instead of a full `Location` object) and strictly defined allowed updateable fields, then map these explicitly to domain models before saving.

## 2024-06-08 - [Hardcoded Default Database Credentials]
**Vulnerability:** The application used hardcoded fallback values (`postgres`/`postgres`) for database connections when `DB_USER` and `DB_PASSWORD` environment variables were not provided.
**Learning:** Hardcoding default database credentials as fallbacks can lead to databases being inadvertently exposed or accessible using widely-known default combinations in misconfigured environments.
**Prevention:** Always use explicit environment variables for sensitive configurations like database credentials and avoid providing fallback values within the application code to ensure explicit and intentional configuration.
