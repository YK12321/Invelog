## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).

## 2024-06-24 - Pagination Implementation and GORM Associations Optimization
**Learning:** Returning large unbounded collections from `Find()` queries, especially when paired with GORM's `Preload()` for nested relations, causes severe performance degradation and memory bloat. For high-volume endpoints (e.g., containers, item types), the absence of a `.Limit()` and `.Offset()` causes massive DB loads.
**Action:** Always implement explicit pagination with `limit` and `offset`, alongside an explicit `.Order("created_at desc")` for stable sorting when fetching large collections, particularly when preloading relationships.
