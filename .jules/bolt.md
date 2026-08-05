## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).

## 2024-08-05 - Audit Summary Calculation Optimization
**Learning:** Computing aggregates in memory by fetching unbounded large datasets can cause memory bottlenecks. Pushing computation to the database using SQL aggregates is highly performant.
**Action:** Use SQL functions like SUM and CASE WHEN via GORM's Select instead of loading full collections to calculate metrics in memory.
