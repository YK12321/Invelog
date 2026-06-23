## 2024-06-02 - ListItems pagination performance bottleneck
**Learning:** Returning large unbounded collections from `GORM.Find()` calls without any `Limit` constraint can cause massive performance and memory issues, especially when paired with `.Preload()`. Using limits and offsets allows for controlled data fetching.
**Action:** Always verify that endpoint logic correctly limits query results when working with list routes. For backward compatibility, make sure to use reasonable default boundaries (e.g. `1000`) and metadata headers (e.g., `X-Total-Count`).

## 2026-06-23 - Base Model Indexing for Pagination
**Learning:** In systems where pagination relies on sorting by `created_at` (e.g., `Order("created_at desc")`), lacking an index on this field leads to expensive filesorts and full table scans, especially harmful on low-powered hardware. Because many models inherit from a shared `Base` struct, placing the index directly on the `Base` struct's `CreatedAt` field universally improves list endpoint performance across the entire application.
**Action:** Always verify if a shared base struct is used when identifying missing indexes on common fields like `created_at`, and apply the `gorm:"index"` tag centrally to optimize all inherited models simultaneously.
