## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).

## 2024-08-02 - GetAuditSummary Database Aggregation Optimization
**Learning:** For data aggregation tasks, pushing computations down to the database using GORM's `.Select()` with SQL functions (e.g., `SUM`, `CASE WHEN`, `COALESCE`) is significantly faster and uses less memory compared to fetching unbounded datasets into memory with `.Find()` to prevent O(N) memory bottlenecks.
**Action:** Use SQL aggregation queries instead of unbounded in-memory loops for producing summary metrics over a large volume of activity log data.
