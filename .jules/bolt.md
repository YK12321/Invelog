## 2026-05-29 - [Unbounded GORM Find Queries in Endpoints]
**Learning:** Found an unbounded `h.DB.Find(&items)` query in `ListItems` (`pkg/api/handlers/items.go`). This codebase pattern uses unpaginated `.Find(&entity)` for retrieving lists without limits. In large collections like an inventory system, this can consume excessive memory and crash the application.
**Action:** Always wrap `.Find()` calls with `.Limit()` and `.Offset()` pagination and include backward-compatible headers to ensure large lists do not cause OOM errors while leaving clients unbroken.
