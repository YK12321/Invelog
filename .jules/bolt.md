## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-05-30 - Prevent Unbounded Find Queries with Preload
**Learning:** Returning large unbounded collections with GORM `Find()` without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`.
**Action:** Always implement pagination with `limit` and `offset` for list endpoints. Use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`) to preserve backwards compatibility.
