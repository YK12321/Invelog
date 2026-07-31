package handlers_test

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"net/http/httptest"
	"testing"
	"time"

	"invelog/pkg/api/router"
	"invelog/pkg/database"
	"invelog/pkg/dto"
	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/stretchr/testify/assert"
)

func setupSEPMTestApp(t *testing.T) (*gin.Engine, string, dto.UserResponse, string, dto.UserResponse) {
	gin.SetMode(gin.TestMode)

	db, err := database.Connect(database.Config{
		Type:     "sqlite",
		Database: ":memory:",
	})
	assert.NoError(t, err)

	err = database.Migrate(db)
	assert.NoError(t, err)

	r := router.SetupRouter(db)

	// Admin user
	adminReg := dto.RegisterRequest{
		Username: "sepm_admin",
		Email:    "admin@sepm.com",
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

	// Lead Engineer User
	leadReg := dto.RegisterRequest{
		Username: "lead_engineer",
		Email:    "lead@sepm.com",
		Password: "password123",
		Role:     "user",
	}
	body, _ = json.Marshal(leadReg)
	req = httptest.NewRequest("POST", "/api/v1/auth/register", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)
	var leadResp dto.AuthResponse
	json.Unmarshal(w.Body.Bytes(), &leadResp)

	return r, adminResp.Token, adminResp.User, leadResp.Token, leadResp.User
}

func TestSEPMProjectLifecycleAndTeam(t *testing.T) {
	r, adminToken, adminUser, _, leadUser := setupSEPMTestApp(t)

	// 1. Create SEPM Project
	trl := 4
	budget := 100000.0
	createProj := dto.CreateProjectRequest{
		Name:        "CubeSat Flight Payload Project",
		Description: "Systems Engineering payload development",
		Phase:       "Definition",
		Status:      "Active",
		TRL:         &trl,
		Budget:      &budget,
		OwnerID:     &adminUser.ID,
	}
	body, _ := json.Marshal(createProj)
	req := httptest.NewRequest("POST", "/api/v1/projects", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	var proj models.Project
	json.Unmarshal(w.Body.Bytes(), &proj)
	assert.Equal(t, "Definition", proj.Phase)
	assert.Equal(t, 4, proj.TRL)

	// 2. Add Lead Engineer to Project Team
	addMember := dto.AddProjectMemberRequest{
		UserID:      leadUser.ID,
		ProjectRole: "SystemsEngineer",
	}
	body, _ = json.Marshal(addMember)
	req = httptest.NewRequest("POST", fmt.Sprintf("/api/v1/projects/%s/members", proj.ID.String()), bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	// List members
	req = httptest.NewRequest("GET", fmt.Sprintf("/api/v1/projects/%s/members", proj.ID.String()), nil)
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusOK, w.Code)

	var members []models.ProjectMember
	json.Unmarshal(w.Body.Bytes(), &members)
	assert.Len(t, members, 2) // Owner + Lead Engineer
}

func TestTaskKanbanAndResourceReservation(t *testing.T) {
	r, adminToken, _, leadToken, leadUser := setupSEPMTestApp(t)

	// Create Project
	createProj := dto.CreateProjectRequest{Name: "Logistics PM Project"}
	body, _ := json.Marshal(createProj)
	req := httptest.NewRequest("POST", "/api/v1/projects", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	var proj models.Project
	json.Unmarshal(w.Body.Bytes(), &proj)

	// Create Task assigned to leadUser
	estHours := 12.5
	now := time.Now()
	dueDate := now.Add(48 * time.Hour)
	createTask := dto.CreateTaskRequest{
		Title:          "Assemble Sensor Reel",
		Description:    "Mount sensors to ESD bag container",
		Status:         "InProgress",
		Priority:       "High",
		AssigneeID:     &leadUser.ID,
		StartDate:      &now,
		DueDate:        &dueDate,
		EstimatedHours: &estHours,
	}
	body, _ = json.Marshal(createTask)
	req = httptest.NewRequest("POST", fmt.Sprintf("/api/v1/projects/%s/tasks", proj.ID.String()), bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	var task models.ProjectTask
	json.Unmarshal(w.Body.Bytes(), &task)
	assert.Equal(t, "InProgress", task.Status)

	// Query /my-tasks as Lead Engineer
	req = httptest.NewRequest("GET", "/api/v1/my-tasks", nil)
	req.Header.Set("Authorization", "Bearer "+leadToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusOK, w.Code)
	var myTasks []models.ProjectTask
	json.Unmarshal(w.Body.Bytes(), &myTasks)
	assert.Len(t, myTasks, 1)
	assert.Equal(t, "Assemble Sensor Reel", myTasks[0].Title)

	// Create Item to reserve for Task
	qty := 100
	createItem := dto.CreateItemRequest{Name: "Microcontroller Chip", Quantity: &qty}
	body, _ = json.Marshal(createItem)
	req = httptest.NewRequest("POST", "/api/v1/items", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	var item models.Item
	json.Unmarshal(w.Body.Bytes(), &item)

	// Reserve 10 Microcontroller Chips for Task
	resQty := 10
	reserveReq := dto.AllocateTaskResourceRequest{
		ItemID:           &item.ID,
		QuantityReserved: &resQty,
		Notes:            "Reserved for Sensor Reel assembly",
	}
	body, _ = json.Marshal(reserveReq)
	req = httptest.NewRequest("POST", fmt.Sprintf("/api/v1/projects/%s/tasks/%s/reserve-resource", proj.ID.String(), task.ID.String()), bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+leadToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)
}

func TestRequirementsWBSAndRiskFMEA(t *testing.T) {
	r, adminToken, _, _, _ := setupSEPMTestApp(t)

	// Create Project
	createProj := dto.CreateProjectRequest{Name: "Aerospace SE Project"}
	body, _ := json.Marshal(createProj)
	req := httptest.NewRequest("POST", "/api/v1/projects", bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w := httptest.NewRecorder()
	r.ServeHTTP(w, req)
	var proj models.Project
	json.Unmarshal(w.Body.Bytes(), &proj)

	// Create Requirement (REQ-01)
	createReq := dto.CreateRequirementRequest{
		Code:               "REQ-01",
		Title:              "Power Consumption Limit",
		Description:        "Payload shall consume < 5W peak power",
		Category:           "Performance",
		VerificationMethod: "Test",
		Status:             "Verified",
	}
	body, _ = json.Marshal(createReq)
	req = httptest.NewRequest("POST", fmt.Sprintf("/api/v1/projects/%s/requirements", proj.ID.String()), bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	// Create WBS Node (1.1)
	prog := 80
	createWBS := dto.CreateWBSNodeRequest{
		Code:        "1.1",
		Name:        "Electrical Power Subsystem",
		ProgressPct: &prog,
		Status:      "InProgress",
	}
	body, _ = json.Marshal(createWBS)
	req = httptest.NewRequest("POST", fmt.Sprintf("/api/v1/projects/%s/wbs", proj.ID.String()), bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	// Create Risk Item (FMEA - Severity 4, Likelihood 3 => RPN 12)
	sev := 4
	lik := 3
	createRisk := dto.CreateRiskItemRequest{
		Title:              "Thermal Overheating in Orbit",
		Category:           "Technical",
		Severity:           &sev,
		Likelihood:         &lik,
		MitigationStrategy: "Add thermal radiator copper strap",
		Status:             "Open",
	}
	body, _ = json.Marshal(createRisk)
	req = httptest.NewRequest("POST", fmt.Sprintf("/api/v1/projects/%s/risks", proj.ID.String()), bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	var risk models.RiskItem
	json.Unmarshal(w.Body.Bytes(), &risk)
	assert.Equal(t, 12, risk.RPN)

	// Create Milestone (PDR)
	createM := dto.CreateMilestoneRequest{
		Name:           "Preliminary Design Review (PDR)",
		GateReviewType: "PDR",
		Status:         "Achieved",
	}
	body, _ = json.Marshal(createM)
	req = httptest.NewRequest("POST", fmt.Sprintf("/api/v1/projects/%s/milestones", proj.ID.String()), bytes.NewBuffer(body))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusCreated, w.Code)

	// 4. Query PM & SEPM Dashboard
	req = httptest.NewRequest("GET", fmt.Sprintf("/api/v1/projects/%s/dashboard", proj.ID.String()), nil)
	req.Header.Set("Authorization", "Bearer "+adminToken)
	w = httptest.NewRecorder()
	r.ServeHTTP(w, req)
	assert.Equal(t, http.StatusOK, w.Code)

	var dash dto.PMDashboardResponse
	json.Unmarshal(w.Body.Bytes(), &dash)
	assert.Equal(t, proj.ID, dash.ProjectID)
	assert.Equal(t, int64(1), dash.TotalRequirements)
	assert.Equal(t, int64(1), dash.VerifiedRequirements)
	assert.Equal(t, float64(100), dash.RequirementsVerifyPct)
	assert.Equal(t, 12, dash.MaxRiskRPN)
	assert.Equal(t, int64(1), dash.AchievedMilestones)
}
