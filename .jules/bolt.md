## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).

## 2026-06-04 - Unbounded GORM Find() Queries Lead to Performance Bottlenecks
**Learning:** Using `h.DB.Find(&items)` without any `.Limit()` restriction in a high-volume endpoint returning many models causes massive memory consumption and poor performance, especially when paired with `.Preload()`. The API originally had unbounded `Find()` calls for `containers`, `categories`, `locations`, and `projects`.
**Action:** When working on list endpoints, avoid `Find()` calls without pagination unless working with an explicitly tiny constant-size dataset. Always enforce pagination with `Limit()` and `Offset()` (defaults typically limit=100) to ensure memory constraints and fast query times.
