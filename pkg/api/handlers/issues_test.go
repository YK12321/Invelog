package handlers_test

import (
	"bytes"
	"encoding/json"
	"mime/multipart"
	"net/http"
	"net/http/httptest"
	"testing"

	"invelog/pkg/api/router"
	"invelog/pkg/database"
	"invelog/pkg/dto"
	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
	"github.com/stretchr/testify/assert"
)

func setupTestApp(t *testing.T) (*gin.Engine, string, string) {
	gin.SetMode(gin.TestMode)

	db, err := database.Connect(database.Config{
		Type:     "sqlite",
		Database: ":memory:",
	})
	assert.NoError(t, err)

	err = database.Migrate(db)
	assert.NoError(t, err)

	r := router.SetupRouter(db)

	// Register Admin user
	adminReg := dto.RegisterRequest{
		Username: "adminuser",
		Email:    "admin@test.com",
		Password: "password123",
		Role:     "admin",
	}
	body, _ := json.Marshal(adminReg)
	req := httptest.NewRequest("POST", "/api/v1/auth/register", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	var adminResp dto.AuthResponse
	json.Unmarshal(w.Body.Bytes(), &adminResp)
	adminToken := adminResp.Token

	// Register Normal user
	userReg := dto.RegisterRequest{
		Username: "normaluser",
		Email:    "user@test.com",
		Password: "password123",
		Role:     "user",
	}
	body, _ = json.Marshal(userReg)
	req = httptest.NewRequest("POST", "/api/v1/auth/register", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	var userResp dto.AuthResponse
	json.Unmarshal(w.Body.Bytes(), &userResp)
	userToken := userResp.Token

	return r, adminToken, userToken
}

func TestIssue89_JWTAuthAndRBAC(t *testing.T) {
	r, adminToken, userToken := setupTestApp(t)

	// Unauthenticated request should fail
	req := httptest.NewRequest("GET", "/api/v1/categories", nil)
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusUnauthorized, w.Code)

	// Create location as user
	createLoc := dto.CreateLocationRequest{Name: "Warehouse A"}
	body, _ := json.Marshal(createLoc)
	req = httptest.NewRequest("POST", "/api/v1/locations", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+userToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	var loc models.Location
	json.Unmarshal(w.Body.Bytes(), &loc)

	// User attempts DELETE (should fail with 403 Forbidden - Admin only)
	req = httptest.NewRequest("DELETE", "/api/v1/locations/"+loc.ID.String(), nil)
	req.Header.Set("Authorization", "Bearer "+userToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusForbidden, w.Code)

	// Admin attempts DELETE (should succeed)
	req = httptest.NewRequest("DELETE", "/api/v1/locations/"+loc.ID.String(), nil)
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusNoContent, w.Code)
}

func TestIssue96_FKAndQuantityValidation(t *testing.T) {
	r, _, userToken := setupTestApp(t)

	// Negative quantity create item should fail
	negQty := -5
	createItem := dto.CreateItemRequest{
		Name:     "Resistor",
		Quantity: &negQty,
	}
	body, _ := json.Marshal(createItem)
	req := httptest.NewRequest("POST", "/api/v1/items", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+userToken)
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusBadRequest, w.Code)

	// Non-existent FK should fail
	fakeID := uuid.New()
	invalidFKItem := dto.CreateItemRequest{
		Name:       "Capacitor",
		CategoryID: &fakeID,
	}
	body, _ = json.Marshal(invalidFKItem)
	req = httptest.NewRequest("POST", "/api/v1/items", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+userToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusBadRequest, w.Code)
}

func TestIssue91_LowStockAlerts(t *testing.T) {
	r, _, userToken := setupTestApp(t)

	// Create item with min_quantity = 10, quantity = 5
	qty := 5
	minQty := 10
	createItem := dto.CreateItemRequest{
		Name:        "Low Stock Part",
		Quantity:    &qty,
		MinQuantity: &minQty,
	}
	body, _ := json.Marshal(createItem)
	req := httptest.NewRequest("POST", "/api/v1/items", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+userToken)
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	// Query low stock items
	req = httptest.NewRequest("GET", "/api/v1/items/low-stock", nil)
	req.Header.Set("Authorization", "Bearer "+userToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusOK, w.Code)

	var lowStockItems []models.Item
	json.Unmarshal(w.Body.Bytes(), &lowStockItems)
	assert.Len(t, lowStockItems, 1)
	assert.Equal(t, "Low Stock Part", lowStockItems[0].Name)
}

func TestIssue92_ParametricMetadata(t *testing.T) {
	r, _, userToken := setupTestApp(t)

	// Create ItemType with parameters JSON
	params := json.RawMessage(`{"package":"0805","tolerance":"1pct"}`)
	createItemType := dto.CreateItemTypeRequest{
		Name:       "SMD Resistor 10k",
		Parameters: params,
	}
	body, _ := json.Marshal(createItemType)
	req := httptest.NewRequest("POST", "/api/v1/item-types", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+userToken)
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	// Parametric filter query
	req = httptest.NewRequest("GET", "/api/v1/item-types?param.package=0805", nil)
	req.Header.Set("Authorization", "Bearer "+userToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusOK, w.Code)

	var itemTypes []models.ItemType
	json.Unmarshal(w.Body.Bytes(), &itemTypes)
	assert.Len(t, itemTypes, 1)
	assert.Equal(t, "SMD Resistor 10k", itemTypes[0].Name)
}

func TestIssue94_BarcodeAndRapidScan(t *testing.T) {
	r, _, userToken := setupTestApp(t)

	// Create item without custom SKU/Barcode (should auto-generate)
	qty := 20
	createItem := dto.CreateItemRequest{
		Name:     "Scan Test Item",
		Quantity: &qty,
	}
	body, _ := json.Marshal(createItem)
	req := httptest.NewRequest("POST", "/api/v1/items", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+userToken)
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	var item models.Item
	json.Unmarshal(w.Body.Bytes(), &item)
	assert.NotEmpty(t, item.SKU)
	assert.NotEmpty(t, item.Barcode)

	// Scan by SKU
	req = httptest.NewRequest("GET", "/api/v1/scan/"+item.SKU, nil)
	req.Header.Set("Authorization", "Bearer "+userToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusOK, w.Code)

	var scanResult map[string]interface{}
	json.Unmarshal(w.Body.Bytes(), &scanResult)
	assert.Equal(t, "item", scanResult["type"])
}

func TestIssue95_CycleCountAudit(t *testing.T) {
	r, _, userToken := setupTestApp(t)

	// Create item with quantity 50
	qty := 50
	createItem := dto.CreateItemRequest{
		Name:     "Audit Reel 100k",
		Quantity: &qty,
	}
	body, _ := json.Marshal(createItem)
	req := httptest.NewRequest("POST", "/api/v1/items", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+userToken)
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	var item models.Item
	json.Unmarshal(w.Body.Bytes(), &item)

	// Perform physical audit finding 42 items (variance -8)
	auditReq := dto.AuditItemRequest{
		PhysicalCount: 42,
		Notes:         "Found 42 on shelf B",
	}
	body, _ = json.Marshal(auditReq)
	req = httptest.NewRequest("POST", "/api/v1/items/"+item.ID.String()+"/audit", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+userToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusOK, w.Code)

	var auditResp map[string]interface{}
	json.Unmarshal(w.Body.Bytes(), &auditResp)
	assert.Equal(t, float64(-8), auditResp["variance"])

	// Check audit summary
	req = httptest.NewRequest("GET", "/api/v1/audit/summary", nil)
	req.Header.Set("Authorization", "Bearer "+userToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusOK, w.Code)

	var summary dto.AuditSummaryResponse
	json.Unmarshal(w.Body.Bytes(), &summary)
	assert.Equal(t, int64(1), summary.TotalAudits)
	assert.Equal(t, int64(-8), summary.NegativeDrift)
}

func TestIssue93_BulkCSVImportAndExport(t *testing.T) {
	r, _, userToken := setupTestApp(t)

	// Prepare CSV content
	csvData := "name,description,quantity,min_quantity\nCSV Part 1,Test desc,15,5\nCSV Part 2,Test desc 2,25,10\n"

	bodyBuf := &bytes.Buffer{}
	writer := multipart.NewWriter(bodyBuf)
	part, err := writer.CreateFormFile("file", "items.csv")
	assert.NoError(t, err)
	part.Write([]byte(csvData))
	writer.Close()

	req := httptest.NewRequest("POST", "/api/v1/items/import", bodyBuf)
	req.Header.Set("Content-Type", writer.FormDataContentType())
	req.Header.Set("Authorization", "Bearer "+userToken)
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusOK, w.Code)

	var importResp map[string]interface{}
	json.Unmarshal(w.Body.Bytes(), &importResp)
	assert.Equal(t, float64(2), importResp["imported_count"])

	// Test Export
	req = httptest.NewRequest("GET", "/api/v1/items/export", nil)
	req.Header.Set("Authorization", "Bearer "+userToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusOK, w.Code)
	assert.Contains(t, w.Body.String(), "CSV Part 1")
	assert.Contains(t, w.Body.String(), "CSV Part 2")
}
