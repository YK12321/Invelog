#ifndef ROUTE_HANDLER_H
#define ROUTE_HANDLER_H

#include <functional>
#include "HTTPRequest.h"
#include "HTTPResponse.h"

/**
 * @brief Route handler function type
 * 
 * A route handler takes an HTTPRequest and returns an HTTPResponse.
 */
using RouteHandler = std::function<HTTPResponse(const HTTPRequest&)>;

#endif // ROUTE_HANDLER_H
