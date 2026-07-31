package dto

type AuditItemRequest struct {
	PhysicalCount int    `json:"physical_count"`
	Notes         string `json:"notes"`
}

type AuditSummaryResponse struct {
	TotalAudits   int64 `json:"total_audits"`
	TotalItems    int64 `json:"total_items"`
	PositiveDrift int64 `json:"positive_drift"`
	NegativeDrift int64 `json:"negative_drift"`
	NetDrift      int64 `json:"net_drift"`
}
