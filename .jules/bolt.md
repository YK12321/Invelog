## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2026-07-05 - Avoid unbounded GORM queries with Preload
**Learning:** Unbounded `.Find()` queries on endpoints with large datasets (like `ListContainers` or `ListItems`) paired with `.Preload()` associations can consume excessive memory and crash the application, or severely degrade performance.
**Action:** Always implement pagination with `Limit` and `Offset` along with stable sorting (`.Order("created_at desc")`) on list endpoints that fetch database models with relational associations.
