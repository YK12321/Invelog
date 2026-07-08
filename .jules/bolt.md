## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-11-20 - Suboptimal GORM Preload Ordering
**Learning:** In GORM queries like `SearchItems`, chaining `.Preload()` before applying conditions (like limits, offsets, or filters) or before calling `.Count()` can result in severe performance penalties. Preloads should only be triggered on the final result set after all pagination boundaries and conditions are applied.
**Action:** Always build the base query and apply `.Count()` and filtering/pagination constraints first. Chain `.Preload()` immediately prior to the final `.Find(&dest)` execution.
