## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).

## 2026-06-07 - Unbounded query missing pagination in ListContainers
**Learning:** Like ListItems, the ListContainers endpoint was returning unbounded queries causing severe memory spikes. This indicates that while pagination patterns exist, they were not universally applied during development to all root aggregate endpoints.
**Action:** When working on API listings or adding new entities, actively ensure pagination logic with fallback defaults (e.g., limit=1000) and headers (X-Total-Count, X-Limit, X-Offset) is implemented to prevent catastrophic memory issues on growth.
