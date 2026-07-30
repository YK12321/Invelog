## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).

## 2024-06-08 - ListContainers pagination missing
**Learning:** Similar to `ListItems`, the `ListContainers` endpoint was returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint, causing potential performance and memory issues.
**Action:** Applied pagination to `ListContainers` with a default limit of 1000 and max 10000, consistent with API conventions, and included metadata headers (`X-Total-Count`, `X-Limit`, `X-Offset`). This provides a massive speedup when the number of containers is large.
