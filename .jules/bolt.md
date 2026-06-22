## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-06-25 - Container Pagination Optimization
**Learning:** Returning massive, unbounded datasets in GORM queries, particularly those heavily relying on `Preload()`, causes severe OOM risks. Setting a fallback limit (e.g., 1000) prevents breaking clients that don't send pagination parameters yet still mitigates the unbounded memory risk.
**Action:** When inspecting backend list endpoints, always check for unpaginated `.Find()` calls and introduce default safe pagination boundaries using HTTP query parameters.
