## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-06-19 - Added Pagination to ListContainers Endpoint
**Learning:** Unbounded GORM queries without limit constraints in list endpoints with `Preload` cause massive performance and memory issues. The `ListContainers` endpoint loaded all data which increased time/memory exponentially as rows scaled up.
**Action:** Always implement pagination with `limit` and `offset` limits along with `.Order("created_at desc")` for stable sorting on endpoints that return large lists of items. Added pagination logic to `ListContainers`.
