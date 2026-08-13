## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-06-02 - GetLowStockItemTypes N+1 Optimization
**Learning:** The previous implementation of `GetLowStockItemTypes` fetched all item types and then executed a separate `SUM(quantity)` query inside a loop for each item type to calculate total quantities. This O(N) query pattern caused significant database overhead and performance degradation as the number of items grew.
**Action:** Replaced the loop with a single optimized SQL query using GORM's `.Joins()`, `.Group()`, and `.Having()` to push the computation and filtering down to the database level, efficiently resolving the N+1 problem.
