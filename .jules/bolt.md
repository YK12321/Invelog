## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-08-01 - AuditSummary unbounded collection performance bottleneck
**Learning:** Fetching unbounded slices of data into memory for metric computation (e.g. `Find(&logs)`) creates an O(N) memory and processing bottleneck that scales poorly with data volume.
**Action:** Always push aggregations (like SUM, COUNT, MAX) down to the database level using `Select` combined with database aggregation functions (like `SUM` and `CASE WHEN`) and `Scan`.
