## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2026-06-12 - Unbounded Queries with Associations Cause Memory Bloat
**Learning:** Using GORM's `Find()` with `Preload()` associations without explicitly limiting the result size causes severe application memory and response time bloat on endpoints returning collections, as GORM must allocate large slices and instantiate complex relation graphs for every result matching the query.
**Action:** When working on list endpoints that fetch collections, always ensure an explicit `.Limit()` and `.Offset()` constraint is applied to the GORM query.
