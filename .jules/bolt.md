## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).

## 2024-08-04 - Unbounded Find with Loop Memory Bottleneck
**Learning:** In list endpoints like `GetAuditSummary`, fetching all matching records with `GORM.Find()` into memory and using a loop to compute aggregations (e.g. `SUM`, drift) can lead to significant memory consumption, high garbage collection overhead, and slow execution when dealing with large datasets.
**Action:** Push data aggregations and drift computations to the database layer directly using GORM's `.Select()` with efficient SQL operations (like `SUM` and `CASE WHEN`) and `.Scan()` to reduce memory footprint and execution time.
