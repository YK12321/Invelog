package handlers

import (
	"encoding/csv"
	"fmt"
	"io"
	"net/http"
	"strconv"
	"strings"

	"invelog/pkg/dto"
	"invelog/pkg/models"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

func (h *Handler) validateItemFKs(itemTypeID, categoryID, containerID, originLocationID *uuid.UUID) error {
	if itemTypeID != nil {
		var count int64
		h.DB.Model(&models.ItemType{}).Where("id = ?", itemTypeID).Count(&count)
		if count == 0 {
			return fmt.Errorf("Referenced item_type_id does not exist")
		}
	}
	if categoryID != nil {
		var count int64
		h.DB.Model(&models.Category{}).Where("id = ?", categoryID).Count(&count)
		if count == 0 {
			return fmt.Errorf("Referenced category_id does not exist")
		}
	}
	if containerID != nil {
		var count int64
		h.DB.Model(&models.Container{}).Where("id = ?", containerID).Count(&count)
		if count == 0 {
			return fmt.Errorf("Referenced container_id does not exist")
		}
	}
	if originLocationID != nil {
		var count int64
		h.DB.Model(&models.Location{}).Where("id = ?", originLocationID).Count(&count)
		if count == 0 {
			return fmt.Errorf("Referenced origin_location_id does not exist")
		}
	}
	return nil
}

// @Summary Create Item
// @Description Create a new item
// @Tags Items
// @Accept json
// @Produce json
// @Param item body dto.CreateItemRequest true "Item Data"
// @Success 201 {object} models.Item
// @Failure 400 {object} map[string]string
// @Router /items [post]
func (h *Handler) CreateItem(c *gin.Context) {
	var req dto.CreateItemRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if req.Quantity != nil && *req.Quantity < 0 {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Quantity cannot be negative"})
		return
	}

	if err := h.validateItemFKs(req.ItemTypeID, req.CategoryID, req.ContainerID, req.OriginLocationID); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	qty := 1
	if req.Quantity != nil {
		qty = *req.Quantity
	}

	minQty := 0
	if req.MinQuantity != nil {
		if *req.MinQuantity < 0 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "min_quantity cannot be negative"})
			return
		}
		minQty = *req.MinQuantity
	}

	reorderQty := 0
	if req.ReorderQuantity != nil {
		if *req.ReorderQuantity < 0 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "reorder_quantity cannot be negative"})
			return
		}
		reorderQty = *req.ReorderQuantity
	}

	item := models.Item{
		Name:             req.Name,
		Description:      req.Description,
		Quantity:         qty,
		MinQuantity:      minQty,
		ReorderQuantity:  reorderQty,
		SKU:              req.SKU,
		Barcode:          req.Barcode,
		IndividualNotes:  req.IndividualNotes,
		SerialNumber:     req.SerialNumber,
		ItemTypeID:       req.ItemTypeID,
		CategoryID:       req.CategoryID,
		ContainerID:      req.ContainerID,
		OriginLocationID: req.OriginLocationID,
	}

	if err := h.DB.Create(&item).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create item"})
		return
	}

	h.LogActivityWithContext(c, "CREATE", "Item", item.ID, "Created item: "+item.Name)
	c.JSON(http.StatusCreated, item)
}

// @Summary List Items
// @Description Get items (paginated)
// @Tags Items
// @Produce json
// @Param limit query int false "Limit (default 1000, max 10000)"
// @Param offset query int false "Offset (default 0)"
// @Success 200 {array} models.Item
// @Router /items [get]
func (h *Handler) ListItems(c *gin.Context) {
	limitStr := c.DefaultQuery("limit", "1000")
	offsetStr := c.DefaultQuery("offset", "0")

	limit, err := strconv.Atoi(limitStr)
	if err != nil || limit <= 0 {
		limit = 1000
	}
	if limit > 10000 {
		limit = 10000
	}

	offset, err := strconv.Atoi(offsetStr)
	if err != nil || offset < 0 {
		offset = 0
	}

	var items []models.Item
	var total int64

	if err := h.DB.Model(&models.Item{}).Count(&total).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list items"})
		return
	}

	if err := h.DB.Preload("Category").Preload("Container").Preload("ItemType").Order("created_at desc").Limit(limit).Offset(offset).Find(&items).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to list items"})
		return
	}

	c.Header("X-Total-Count", fmt.Sprintf("%d", total))
	c.Header("X-Limit", fmt.Sprintf("%d", limit))
	c.Header("X-Offset", fmt.Sprintf("%d", offset))

	c.JSON(http.StatusOK, items)
}

// @Summary Get Item
// @Description Get an item by ID
// @Tags Items
// @Produce json
// @Param id path string true "Item ID"
// @Success 200 {object} models.Item
// @Failure 404 {object} map[string]string
// @Router /items/{id} [get]
func (h *Handler) GetItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var item models.Item
	if err := h.DB.Preload("Category").Preload("Container").Preload("ItemType").First(&item, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Item not found"})
		return
	}

	c.JSON(http.StatusOK, item)
}

type UpdateItemInput struct {
	Name             *string    `json:"name"`
	Description      *string    `json:"description"`
	Quantity         *int       `json:"quantity"`
	MinQuantity      *int       `json:"min_quantity"`
	ReorderQuantity  *int       `json:"reorder_quantity"`
	SKU              *string    `json:"sku"`
	Barcode          *string    `json:"barcode"`
	IndividualNotes  *string    `json:"individual_notes"`
	SerialNumber     *string    `json:"serial_number"`
	ItemTypeID       *uuid.UUID `json:"item_type_id"`
	CategoryID       *uuid.UUID `json:"category_id"`
	ContainerID      *uuid.UUID `json:"container_id"`
	OriginLocationID *uuid.UUID `json:"origin_location_id"`
}

// @Summary Update Item
// @Description Update an item by ID
// @Tags Items
// @Accept json
// @Produce json
// @Param id path string true "Item ID"
// @Param item body UpdateItemInput true "Item Update Data"
// @Success 200 {object} models.Item
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /items/{id} [put]
func (h *Handler) UpdateItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var item models.Item
	if err := h.DB.First(&item, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Item not found"})
		return
	}

	var input UpdateItemInput
	if err := c.ShouldBindJSON(&input); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if input.Quantity != nil && *input.Quantity < 0 {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Quantity cannot be negative"})
		return
	}

	if err := h.validateItemFKs(input.ItemTypeID, input.CategoryID, input.ContainerID, input.OriginLocationID); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if input.Name != nil {
		item.Name = *input.Name
	}
	if input.Description != nil {
		item.Description = *input.Description
	}
	if input.Quantity != nil {
		item.Quantity = *input.Quantity
	}
	if input.MinQuantity != nil {
		if *input.MinQuantity < 0 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "min_quantity cannot be negative"})
			return
		}
		item.MinQuantity = *input.MinQuantity
	}
	if input.ReorderQuantity != nil {
		if *input.ReorderQuantity < 0 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "reorder_quantity cannot be negative"})
			return
		}
		item.ReorderQuantity = *input.ReorderQuantity
	}
	if input.SKU != nil {
		item.SKU = *input.SKU
	}
	if input.Barcode != nil {
		item.Barcode = *input.Barcode
	}
	if input.IndividualNotes != nil {
		item.IndividualNotes = *input.IndividualNotes
	}
	if input.SerialNumber != nil {
		item.SerialNumber = *input.SerialNumber
	}
	if input.ItemTypeID != nil {
		item.ItemTypeID = input.ItemTypeID
	}
	if input.CategoryID != nil {
		item.CategoryID = input.CategoryID
	}
	if input.ContainerID != nil {
		item.ContainerID = input.ContainerID
	}
	if input.OriginLocationID != nil {
		item.OriginLocationID = input.OriginLocationID
	}

	if err := h.DB.Save(&item).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update item"})
		return
	}

	h.LogActivityWithContext(c, "UPDATE", "Item", item.ID, "Updated item: "+item.Name)
	c.JSON(http.StatusOK, item)
}

// @Summary Delete Item
// @Description Delete an item by ID
// @Tags Items
// @Produce json
// @Param id path string true "Item ID"
// @Success 204
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /items/{id} [delete]
func (h *Handler) DeleteItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	if err := h.DB.Delete(&models.Item{}, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to delete item"})
		return
	}

	h.LogActivityWithContext(c, "DELETE", "Item", id, "Deleted item")
	c.JSON(http.StatusNoContent, nil)
}

// @Summary Move Item
// @Description Move an item to a different container
// @Tags Items
// @Produce json
// @Param id path string true "Item ID"
// @Param container_id query string true "New Container ID"
// @Success 200 {object} models.Item
// @Failure 400 {object} map[string]string
// @Failure 404 {object} map[string]string
// @Router /items/{id}/move [post]
func (h *Handler) MoveItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Item ID"})
		return
	}

	containerIDStr := c.Query("container_id")
	containerID, err := uuid.Parse(containerIDStr)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Container ID"})
		return
	}

	var item models.Item
	if err := h.DB.First(&item, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Item not found"})
		return
	}

	var container models.Container
	if err := h.DB.First(&container, "id = ?", containerID).Error; err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Target container_id does not exist"})
		return
	}

	fromContainerID := item.ContainerID
	item.ContainerID = &containerID
	if err := h.DB.Save(&item).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to move item"})
		return
	}

	h.LogActivityDetailedWithContext(c, "MOVE", "Item", item.ID, "Moved item to container: "+containerID.String(), fromContainerID, &containerID, nil, 0)
	c.JSON(http.StatusOK, item)
}

// @Summary Get Low Stock Items
// @Description Query items where quantity <= min_quantity
// @Tags Items
// @Produce json
// @Success 200 {array} models.Item
// @Router /items/low-stock [get]
func (h *Handler) GetLowStockItems(c *gin.Context) {
	var items []models.Item
	if err := h.DB.Preload("Category").Preload("Container").Preload("ItemType").Where("quantity <= min_quantity AND min_quantity > 0").Find(&items).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to fetch low stock items"})
		return
	}
	c.JSON(http.StatusOK, items)
}

// @Summary Rapid Scan Lookup
// @Description Lookup item or container by Barcode, SKU, or ID
// @Tags Scan
// @Produce json
// @Param code path string true "Barcode/SKU/UUID Code"
// @Success 200 {object} map[string]interface{}
// @Failure 404 {object} map[string]string
// @Router /scan/{code} [get]
func (h *Handler) ScanLookup(c *gin.Context) {
	code := c.Param("code")
	if code == "" {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Scan code required"})
		return
	}

	var item models.Item
	itemQuery := h.DB.Preload("Category").Preload("Container").Preload("ItemType")
	parsedUUID, err := uuid.Parse(code)
	if err == nil {
		if err := itemQuery.First(&item, "id = ? OR sku = ? OR barcode = ?", parsedUUID, code, code).Error; err == nil {
			c.JSON(http.StatusOK, gin.H{
				"type": "item",
				"data": item,
			})
			return
		}
	} else {
		if err := itemQuery.First(&item, "sku = ? OR barcode = ?", code, code).Error; err == nil {
			c.JSON(http.StatusOK, gin.H{
				"type": "item",
				"data": item,
			})
			return
		}
	}

	var container models.Container
	containerQuery := h.DB.Preload("Location").Preload("Parent").Preload("Project")
	if err == nil {
		if err := containerQuery.First(&container, "id = ? OR barcode = ?", parsedUUID, code).Error; err == nil {
			c.JSON(http.StatusOK, gin.H{
				"type": "container",
				"data": container,
			})
			return
		}
	} else {
		if err := containerQuery.First(&container, "barcode = ?", code).Error; err == nil {
			c.JSON(http.StatusOK, gin.H{
				"type": "container",
				"data": container,
			})
			return
		}
	}

	c.JSON(http.StatusNotFound, gin.H{"error": "No matching item or container found for scan code"})
}

// @Summary Audit Item
// @Description Cycle count audit reconciliation for an item
// @Tags Audit
// @Accept json
// @Produce json
// @Param id path string true "Item ID"
// @Param request body dto.AuditItemRequest true "Audit Data"
// @Success 200 {object} map[string]interface{}
// @Failure 400 {object} map[string]string
// @Router /items/{id}/audit [post]
func (h *Handler) AuditItem(c *gin.Context) {
	id, err := uuid.Parse(c.Param("id"))
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid ID format"})
		return
	}

	var req dto.AuditItemRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	if req.PhysicalCount < 0 {
		c.JSON(http.StatusBadRequest, gin.H{"error": "physical_count cannot be negative"})
		return
	}

	var item models.Item
	if err := h.DB.First(&item, "id = ?", id).Error; err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "Item not found"})
		return
	}

	oldQty := item.Quantity
	variance := req.PhysicalCount - oldQty
	item.Quantity = req.PhysicalCount

	if err := h.DB.Save(&item).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to update item quantity"})
		return
	}

	details := fmt.Sprintf("Audit reconciliation: Expected %d, Found %d, Variance %d", oldQty, req.PhysicalCount, variance)
	if req.Notes != "" {
		details += ". Notes: " + req.Notes
	}

	h.LogActivityDetailedWithContext(c, "QUANTITY_ADJUSTED", "Item", item.ID, details, nil, nil, nil, variance)

	c.JSON(http.StatusOK, gin.H{
		"item":           item,
		"expected_count": oldQty,
		"physical_count": req.PhysicalCount,
		"variance":       variance,
	})
}

// @Summary Get Audit Summary
// @Description Get audit reconciliation summary metrics
// @Tags Audit
// @Produce json
// @Success 200 {object} dto.AuditSummaryResponse
// @Router /audit/summary [get]
func (h *Handler) GetAuditSummary(c *gin.Context) {
	var totalAudits int64
	var posDrift int64
	var negDrift int64
	var totalItems int64

	h.DB.Model(&models.ActivityLog{}).Where("action = ?", "QUANTITY_ADJUSTED").Count(&totalAudits)
	h.DB.Model(&models.Item{}).Count(&totalItems)

	// ⚡ Bolt: Push drift calculation to the database to avoid fetching thousands of records into application memory
	type driftResult struct {
		Pos int64
		Neg int64
	}
	var res driftResult
	h.DB.Model(&models.ActivityLog{}).
		Where("action = ?", "QUANTITY_ADJUSTED").
		Select("COALESCE(SUM(CASE WHEN quantity_change > 0 THEN quantity_change ELSE 0 END), 0) as pos, COALESCE(SUM(CASE WHEN quantity_change < 0 THEN quantity_change ELSE 0 END), 0) as neg").
		Scan(&res)

	posDrift = res.Pos
	negDrift = res.Neg

	c.JSON(http.StatusOK, dto.AuditSummaryResponse{
		TotalAudits:   totalAudits,
		TotalItems:    totalItems,
		PositiveDrift: posDrift,
		NegativeDrift: negDrift,
		NetDrift:      posDrift + negDrift,
	})
}

// @Summary Import Items via CSV
// @Description Import items in bulk from a CSV upload
// @Tags Items
// @Accept multipart/form-data
// @Produce json
// @Param file formData file true "CSV File"
// @Success 200 {object} map[string]interface{}
// @Failure 400 {object} map[string]string
// @Router /items/import [post]
func (h *Handler) ImportItemsCSV(c *gin.Context) {
	fileHeader, err := c.FormFile("file")
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "CSV file required under form key 'file'"})
		return
	}

	file, err := fileHeader.Open()
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Failed to open CSV file"})
		return
	}
	defer file.Close()

	reader := csv.NewReader(file)
	headers, err := reader.Read()
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Failed to read CSV header"})
		return
	}

	headerMap := make(map[string]int)
	for i, h := range headers {
		headerMap[strings.TrimSpace(strings.ToLower(h))] = i
	}

	var importedCount int
	var rowErrors []gin.H

	lineNum := 1
	for {
		lineNum++
		record, err := reader.Read()
		if err == io.EOF {
			break
		}
		if err != nil {
			rowErrors = append(rowErrors, gin.H{"line": lineNum, "error": "CSV parsing error: " + err.Error()})
			continue
		}

		getCol := func(name string) string {
			if idx, ok := headerMap[name]; ok && idx < len(record) {
				return strings.TrimSpace(record[idx])
			}
			return ""
		}

		name := getCol("name")
		if name == "" {
			rowErrors = append(rowErrors, gin.H{"line": lineNum, "error": "Name field is required"})
			continue
		}

		qty := 1
		if qtyStr := getCol("quantity"); qtyStr != "" {
			q, parseErr := strconv.Atoi(qtyStr)
			if parseErr != nil || q < 0 {
				rowErrors = append(rowErrors, gin.H{"line": lineNum, "error": "Invalid quantity (must be non-negative integer)"})
				continue
			}
			qty = q
		}

		minQty := 0
		if minQtyStr := getCol("min_quantity"); minQtyStr != "" {
			mq, parseErr := strconv.Atoi(minQtyStr)
			if parseErr != nil || mq < 0 {
				rowErrors = append(rowErrors, gin.H{"line": lineNum, "error": "Invalid min_quantity"})
				continue
			}
			minQty = mq
		}

		reorderQty := 0
		if reorderQtyStr := getCol("reorder_quantity"); reorderQtyStr != "" {
			rq, parseErr := strconv.Atoi(reorderQtyStr)
			if parseErr != nil || rq < 0 {
				rowErrors = append(rowErrors, gin.H{"line": lineNum, "error": "Invalid reorder_quantity"})
				continue
			}
			reorderQty = rq
		}

		parseUUIDPtr := func(colName string) (*uuid.UUID, error) {
			val := getCol(colName)
			if val == "" {
				return nil, nil
			}
			u, parseErr := uuid.Parse(val)
			if parseErr != nil {
				return nil, fmt.Errorf("Invalid %s UUID", colName)
			}
			return &u, nil
		}

		itemTypeID, err1 := parseUUIDPtr("item_type_id")
		categoryID, err2 := parseUUIDPtr("category_id")
		containerID, err3 := parseUUIDPtr("container_id")
		originLocationID, err4 := parseUUIDPtr("origin_location_id")

		if err1 != nil || err2 != nil || err3 != nil || err4 != nil {
			errStr := ""
			for _, e := range []error{err1, err2, err3, err4} {
				if e != nil {
					errStr += e.Error() + "; "
				}
			}
			rowErrors = append(rowErrors, gin.H{"line": lineNum, "error": strings.TrimSuffix(errStr, "; ")})
			continue
		}

		if fkErr := h.validateItemFKs(itemTypeID, categoryID, containerID, originLocationID); fkErr != nil {
			rowErrors = append(rowErrors, gin.H{"line": lineNum, "error": fkErr.Error()})
			continue
		}

		item := models.Item{
			Name:             name,
			Description:      getCol("description"),
			Quantity:         qty,
			MinQuantity:      minQty,
			ReorderQuantity:  reorderQty,
			SKU:              getCol("sku"),
			Barcode:          getCol("barcode"),
			SerialNumber:     getCol("serial_number"),
			ItemTypeID:       itemTypeID,
			CategoryID:       categoryID,
			ContainerID:      containerID,
			OriginLocationID: originLocationID,
		}

		if createErr := h.DB.Create(&item).Error; createErr != nil {
			rowErrors = append(rowErrors, gin.H{"line": lineNum, "error": "Database error: " + createErr.Error()})
			continue
		}

		importedCount++
		h.LogActivityWithContext(c, "CREATE", "Item", item.ID, "Bulk imported item: "+item.Name)
	}

	c.JSON(http.StatusOK, gin.H{
		"imported_count": importedCount,
		"errors":         rowErrors,
	})
}

// @Summary Export Items as CSV
// @Description Export items as CSV file filtered by location, container, or category
// @Tags Items
// @Produce text/csv
// @Param location_id query string false "Filter by Origin Location ID"
// @Param container_id query string false "Filter by Container ID"
// @Param category_id query string false "Filter by Category ID"
// @Router /items/export [get]
func (h *Handler) ExportItemsCSV(c *gin.Context) {
	query := h.DB.Model(&models.Item{})

	if locIDStr := c.Query("location_id"); locIDStr != "" {
		if u, err := uuid.Parse(locIDStr); err == nil {
			query = query.Where("origin_location_id = ?", u)
		}
	}
	if cntIDStr := c.Query("container_id"); cntIDStr != "" {
		if u, err := uuid.Parse(cntIDStr); err == nil {
			query = query.Where("container_id = ?", u)
		}
	}
	if catIDStr := c.Query("category_id"); catIDStr != "" {
		if u, err := uuid.Parse(catIDStr); err == nil {
			query = query.Where("category_id = ?", u)
		}
	}

	var items []models.Item
	if err := query.Find(&items).Error; err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to fetch items for export"})
		return
	}

	c.Header("Content-Type", "text/csv")
	c.Header("Content-Disposition", "attachment; filename=\"items.csv\"")

	writer := csv.NewWriter(c.Writer)
	_ = writer.Write([]string{
		"id", "name", "description", "quantity", "min_quantity", "reorder_quantity",
		"sku", "barcode", "serial_number", "item_type_id", "category_id", "container_id", "origin_location_id",
	})

	uuidToStr := func(u *uuid.UUID) string {
		if u == nil {
			return ""
		}
		return u.String()
	}

	for _, item := range items {
		_ = writer.Write([]string{
			item.ID.String(),
			item.Name,
			item.Description,
			strconv.Itoa(item.Quantity),
			strconv.Itoa(item.MinQuantity),
			strconv.Itoa(item.ReorderQuantity),
			item.SKU,
			item.Barcode,
			item.SerialNumber,
			uuidToStr(item.ItemTypeID),
			uuidToStr(item.CategoryID),
			uuidToStr(item.ContainerID),
			uuidToStr(item.OriginLocationID),
		})
	}

	writer.Flush()
}
