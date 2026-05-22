#ifndef UUID_H
#define UUID_H

// Prevent Windows UUID type conflict
#define WIN32_NO_STATUS
#include <string>
#include <random>
#include <sstream>
#include <iomanip>

class UUID {
public:
    UUID();
    explicit UUID(const std::string& uuid);
    
    std::string toString() const;
    bool operator==(const UUID& other) const;
    bool operator!=(const UUID& other) const;
    
    static UUID generate();
    static UUID fromString(const std::string& uuid);
    
private:
    std::string uuid_;
    static std::string generateUUIDv4();
};

#endif // UUID_H
