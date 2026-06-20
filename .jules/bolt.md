## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).

## 2024-06-20 - ListContainers pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls in the  endpoint without any `Limit` constraint caused massive performance and memory issues, especially with . Using limits and offsets allows for controlled data fetching.
## 2024-06-20 - ListContainers unbounded queries
**Learning:** Found that the ListContainers endpoint lacked pagination, which caused massive performance hits on the server.
**Action:** Implemented Limit and Offset with standard metadata headers, which sped up the list query from ~270M ns/op to ~35M ns/op.
