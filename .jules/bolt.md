## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2026-06-09 - Unbounded GORM Queries with Preload Cause Bottlenecks
**Learning:** Returning large collections with unbounded GORM `Find()` queries, particularly when `.Preload()` is used, can cause massive memory usage and excessive database load, acting as a major performance bottleneck.
**Action:** Always implement pagination (with `limit` and `offset`) for list endpoints. Enforce default and maximum limits (e.g., default 1000, max 10000 for high-volume endpoints) and return pagination metadata via headers (`X-Total-Count`, `X-Limit`, `X-Offset`) to preserve API compatibility without altering the raw JSON array payload.
