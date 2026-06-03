## 2024-06-03 - Retrofitting pagination to unbounded endpoints
**Learning:** When retrofitting pagination to an endpoint that previously returned all records (unbounded), standard `limit=20` defaults will break existing clients that expect the full dataset.
**Action:** Always use high limits (e.g., limit=1000, max=10000) and preserve the JSON array response body by returning pagination metadata in HTTP headers (`X-Total-Count`, `X-Limit`, `X-Offset`) to maintain backward compatibility.
