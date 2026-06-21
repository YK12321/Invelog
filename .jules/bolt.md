## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2026-06-21 - Unbounded Queries with Preloads
**Learning:** Returning an unbounded set of results (e.g., in `/containers`) via `Find()` along with GORM `.Preload()` statements can cause extreme memory bloat and database overhead on larger datasets, as preloads fetch entire relational graphs for every row.
**Action:** Always implement pagination (`Limit` and `Offset` along with stable `Order("created_at desc")`) on generic list endpoints, especially those involving multiple preloads.
