## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).
## 2024-06-03 - N+1 query optimization in ItemTypes low stock fetching
**Learning:** Fetching data and iterating over it to run aggregate queries (`SUM(quantity)`) leads to a severe N+1 problem.
**Action:** Always push computations like aggregate sums down to the database using `Joins`, `Group`, and `Having` to avoid executing queries inside loops.
