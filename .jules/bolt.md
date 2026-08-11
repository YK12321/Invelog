## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-06-03 - Database level summation vs memory iteration
**Learning:** For aggregations like summing positive/negative drift from logs, fetching unbounded records into application memory (`Find()`) just to calculate a sum is a massive bottleneck. It scales linearly in memory, CPU, and network time.
**Action:** Push data reductions down to the database level using `GORM.Select` with standard SQL aggregations (e.g., `COALESCE(SUM(CASE WHEN ...))` to offload the work, reducing memory overhead to O(1) and significantly improving query latency.
