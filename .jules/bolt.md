## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).

## 2026-08-07 - Optimize data aggregation
**Learning:** For data aggregation tasks, fetching unbounded datasets into memory with `.Find()` causes O(N) memory bottlenecks and excessive data transfer overhead.
**Action:** Push computations down to the database using GORM's `.Select()` with SQL functions (e.g., `SUM`, `CASE WHEN`, `COALESCE`) rather than fetching unbounded datasets into memory.
