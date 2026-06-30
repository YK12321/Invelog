## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-06-30 - Unbounded GORM Preloads memory bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint causes massive performance and memory issues, especially when paired with multiple `.Preload()` associations like in `ListContainers`.
**Action:** Always implement pagination using `Limit()` and `Offset()` alongside a stable `.Order("created_at desc")` to prevent OOM errors and excessive database load.
