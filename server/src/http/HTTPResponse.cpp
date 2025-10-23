#include "../include/http/HTTPResponse.h"

HTTPResponse::HTTPResponse() : statusCode(200) {
    headers["Content-Type"] = "application/json";
    headers["Access-Control-Allow-Origin"] = "*";
}

HTTPResponse::HTTPResponse(int status, const std::string& responseBody)
    : statusCode(status), body(responseBody) {
    headers["Content-Type"] = "application/json";
    headers["Access-Control-Allow-Origin"] = "*";
}

void HTTPResponse::setHeader(const std::string& name, const std::string& value) {
    headers[name] = value;
}

void HTTPResponse::setContentType(const std::string& contentType) {
    headers["Content-Type"] = contentType;
}

void HTTPResponse::enableCORS() {
    headers["Access-Control-Allow-Origin"] = "*";
    headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, DELETE, OPTIONS";
    headers["Access-Control-Allow-Headers"] = "Content-Type, X-API-Key, Authorization";
}

HTTPResponse HTTPResponse::ok(const std::string& body) {
    return HTTPResponse(200, body);
}

HTTPResponse HTTPResponse::created(const std::string& body) {
    return HTTPResponse(201, body);
}

HTTPResponse HTTPResponse::noContent() {
    return HTTPResponse(204, "");
}

HTTPResponse HTTPResponse::badRequest(const std::string& message) {
    return HTTPResponse(400, "{\"error\":\"" + message + "\"}");
}

HTTPResponse HTTPResponse::unauthorized(const std::string& message) {
    return HTTPResponse(401, "{\"error\":\"" + message + "\"}");
}

HTTPResponse HTTPResponse::notFound(const std::string& message) {
    return HTTPResponse(404, "{\"error\":\"" + message + "\"}");
}

HTTPResponse HTTPResponse::internalError(const std::string& message) {
    return HTTPResponse(500, "{\"error\":\"" + message + "\"}");
}
