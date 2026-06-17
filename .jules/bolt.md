## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-06-17 - Prevent Unbounded GORM Queries with Preload
**Learning:** Returning large unbounded collections in list endpoints (like `/containers`) without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using `Limit` and `Offset` drastically reduces execution time and allocations.
**Action:** Always implement pagination using `limit` and `offset` along with `Order()` for stable sorting in list endpoints, especially when relations are eagerly loaded with GORM.
