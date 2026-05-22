#include "UUID.h"
#include <chrono>

UUID::UUID() {
    uuid_ = generateUUIDv4();
}

UUID::UUID(const std::string& uuid) : uuid_(uuid) {}

std::string UUID::toString() const {
    return uuid_;
}

bool UUID::operator==(const UUID& other) const {
    return uuid_ == other.uuid_;
}

bool UUID::operator!=(const UUID& other) const {
    return !(*this == other);
}

UUID UUID::generate() {
    return UUID();
}

UUID UUID::fromString(const std::string& uuid) {
    // Validate UUID format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    if (uuid.length() != 36) {
        return UUID("00000000-0000-0000-0000-000000000000");
    }
    
    // Check hyphen positions
    if (uuid[8] != '-' || uuid[13] != '-' || uuid[18] != '-' || uuid[23] != '-') {
        return UUID("00000000-0000-0000-0000-000000000000");
    }
    
    // Check that all other characters are hex digits
    for (size_t i = 0; i < uuid.length(); ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            continue; // Skip hyphens
        }
        if (!isxdigit(uuid[i])) {
            return UUID("00000000-0000-0000-0000-000000000000");
        }
    }
    
    return UUID(uuid);
}

std::string UUID::generateUUIDv4() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    // Generate UUID v4 format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
    uint64_t part1 = dis(gen);
    uint64_t part2 = dis(gen);
    
    ss << std::setw(8) << (part1 >> 32);
    ss << "-";
    ss << std::setw(4) << ((part1 >> 16) & 0xFFFF);
    ss << "-";
    ss << std::setw(4) << (0x4000 | ((part1 & 0x0FFF)));
    ss << "-";
    ss << std::setw(4) << (0x8000 | ((part2 >> 48) & 0x3FFF));
    ss << "-";
    ss << std::setw(12) << (part2 & 0xFFFFFFFFFFFF);
    
    return ss.str();
}
