## 2026-05-23 - [Unbounded Query DoS]
**Vulnerability:** The `/api/v1/activity-logs` endpoint queried the entire `activity_logs` table using `Find()` without a limit or pagination, causing the result set to grow continuously. This leads to unbounded memory consumption on the server and excessive database load, constituting a Denial of Service (DoS) risk.
**Learning:** In a production application handling an ever-growing dataset (like audit logs), fetching all records at once should never be permitted. Even during initial development, endpoints accessing list resources must implement bounded limits to prevent system exhaustion under load.
**Prevention:** Always implement pagination (e.g., using limit and offset defaults) for list-based API endpoints. When possible, add hard upper limits on the maximum number of items that can be fetched in a single query to ensure resource usage remains predictable and safe.

## 2026-05-27 - [Hardcoded Database Credentials]
**Vulnerability:** The application used hardcoded fallback credentials (`postgres`/`postgres`) for database connections in `cmd/invelog/main.go`. This is a critical security vulnerability that could lead to unauthorized database access if deployed without proper environment variables.
**Learning:** Default fallback credentials should never be included in the source code, as they provide a false sense of security and a known attack vector for default deployments.
**Prevention:** Remove fallback values for sensitive configurations like database user and password. Require explicit configuration via environment variables, ensuring that missing configurations are safely handled or lead to a failure rather than using unsafe defaults.
