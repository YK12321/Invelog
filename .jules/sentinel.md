## 2026-05-23 - [Unbounded Query DoS]
**Vulnerability:** The `/api/v1/activity-logs` endpoint queried the entire `activity_logs` table using `Find()` without a limit or pagination, causing the result set to grow continuously. This leads to unbounded memory consumption on the server and excessive database load, constituting a Denial of Service (DoS) risk.
**Learning:** In a production application handling an ever-growing dataset (like audit logs), fetching all records at once should never be permitted. Even during initial development, endpoints accessing list resources must implement bounded limits to prevent system exhaustion under load.
**Prevention:** Always implement pagination (e.g., using limit and offset defaults) for list-based API endpoints. When possible, add hard upper limits on the maximum number of items that can be fetched in a single query to ensure resource usage remains predictable and safe.

## 2024-05-24 - [GORM Mass Assignment Vulnerability]
**Vulnerability:** API handlers directly bound user JSON input to GORM domain models (e.g., `models.ItemType`). This allowed mass assignment, where an attacker could potentially modify unauthorized fields or nested associations during `Create` or `Save` operations.
**Learning:** GORM's automatic association saving behavior can be exploited if user input is directly bound to domain models. Always use explicit Data Transfer Objects (DTOs) for API request binding to strictly control which fields can be modified.
**Prevention:** Define explicit DTO structs (e.g., in `pkg/dto`) for all create and update operations, and manually map validated DTO fields to the domain model before performing database operations.
