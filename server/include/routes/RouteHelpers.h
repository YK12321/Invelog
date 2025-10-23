#ifndef ROUTE_HELPERS_H
#define ROUTE_HELPERS_H

#include <string>
#include "../../include/UUID.h"

/**
 * @brief Helper functions for route handlers
 */
namespace RouteHelpers {
    /**
     * @brief Extract UUID from URL path
     * @param path The URL path containing the UUID (e.g., "/api/items/uuid-here")
     * @return UUID extracted from path
     * @throws std::invalid_argument if UUID cannot be extracted or parsed
     */
    inline UUID extractUUID(const std::string& path) {
        // Find the last segment of the path (after the last '/')
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash == std::string::npos || lastSlash == path.length() - 1) {
            throw std::invalid_argument("Invalid path format: no ID found");
        }
        
        std::string uuidStr = path.substr(lastSlash + 1);
        
        // Remove query parameters if present
        size_t queryPos = uuidStr.find('?');
        if (queryPos != std::string::npos) {
            uuidStr = uuidStr.substr(0, queryPos);
        }
        
        try {
            return UUID::fromString(uuidStr);
        } catch (const std::exception& e) {
            throw std::invalid_argument("Invalid UUID in path: " + uuidStr);
        }
    }
}

#endif // ROUTE_HELPERS_H
