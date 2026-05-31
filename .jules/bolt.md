
## 2026-05-31 - Safe Pagination Retrofitting
**Learning:** When retrofitting pagination to an existing unbounded list endpoint (like `ListItems`), using standard defaults (e.g., limit=20) can break existing API clients that expect all data in one request.
**Action:** Always implement pagination with sufficiently high default and maximum limits (e.g., limit=1000, max=10000) when adding it to previously unbounded endpoints to prevent memory exhaustion while maintaining backward compatibility.
