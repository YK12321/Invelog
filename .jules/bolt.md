## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-08-10 - O(N) Memory in GetAuditSummary
**Learning:** Fetching unbounded datasets into memory with .Find() for aggregation causes significant performance and memory bottlenecks, specifically in GetAuditSummary.
**Action:** Push computations down to the database using GORM.Select() with SQL functions like SUM and CASE WHEN.
