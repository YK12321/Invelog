#include <gtest/gtest.h>
#include "UUID.h"
#include <set>
#include <unordered_set>
#include <chrono>

// Test UUID generation
TEST(UUIDTest, GenerateUnique) {
    UUID uuid1 = UUID::generate();
    UUID uuid2 = UUID::generate();
    
    // UUIDs should be unique
    EXPECT_NE(uuid1.toString(), uuid2.toString());
}

// Test UUID format (should be 36 characters with hyphens)
TEST(UUIDTest, CorrectFormat) {
    UUID uuid = UUID::generate();
    std::string str = uuid.toString();
    
    // Length should be 36 (32 hex + 4 hyphens)
    EXPECT_EQ(str.length(), 36);
    
    // Check hyphen positions (8-4-4-4-12 format)
    EXPECT_EQ(str[8], '-');
    EXPECT_EQ(str[13], '-');
    EXPECT_EQ(str[18], '-');
    EXPECT_EQ(str[23], '-');
    
    // Check that other characters are hex digits or hyphens
    for (size_t i = 0; i < str.length(); ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            EXPECT_EQ(str[i], '-');
        } else {
            EXPECT_TRUE(isxdigit(str[i])) << "Character at position " << i << " is not a hex digit";
        }
    }
}

// Test UUID version (should be version 4)
TEST(UUIDTest, Version4) {
    UUID uuid = UUID::generate();
    std::string str = uuid.toString();
    
    // Version 4 UUID should have '4' at position 14
    EXPECT_EQ(str[14], '4');
    
    // Variant should be 8, 9, a, or b at position 19
    char variant = str[19];
    EXPECT_TRUE(variant == '8' || variant == '9' || variant == 'a' || variant == 'b');
}

// Test UUID uniqueness with large set
TEST(UUIDTest, MassUniqueness) {
    std::set<std::string> uuids;
    const int count = 10000;
    
    for (int i = 0; i < count; ++i) {
        UUID uuid = UUID::generate();
        uuids.insert(uuid.toString());
    }
    
    // All UUIDs should be unique
    EXPECT_EQ(uuids.size(), count);
}

// Test UUID comparison operators
TEST(UUIDTest, EqualityOperator) {
    UUID uuid1 = UUID::generate();
    UUID uuid2 = uuid1;  // Copy
    UUID uuid3 = UUID::generate();
    
    // Same UUID should be equal
    EXPECT_EQ(uuid1, uuid2);
    
    // Different UUIDs should not be equal
    EXPECT_NE(uuid1, uuid3);
}

// Test UUID inequality operator
TEST(UUIDTest, InequalityOperator) {
    UUID uuid1 = UUID::generate();
    UUID uuid2 = UUID::generate();
    
    EXPECT_TRUE(uuid1 != uuid2);
    EXPECT_FALSE(uuid1 != uuid1);
}

// Test UUID from string
TEST(UUIDTest, FromString) {
    UUID original = UUID::generate();
    std::string str = original.toString();
    
    UUID fromStr = UUID::fromString(str);
    
    EXPECT_EQ(original, fromStr);
    EXPECT_EQ(original.toString(), fromStr.toString());
}

// Test UUID from invalid string
TEST(UUIDTest, FromInvalidString) {
    // Empty string should create nil UUID
    UUID empty = UUID::fromString("");
    EXPECT_EQ(empty.toString(), "00000000-0000-0000-0000-000000000000");
    
    // Invalid format should create nil UUID
    UUID invalid = UUID::fromString("not-a-uuid");
    EXPECT_EQ(invalid.toString(), "00000000-0000-0000-0000-000000000000");
}

// Test UUID nil (zero UUID)
TEST(UUIDTest, NilUUID) {
    UUID nil = UUID::fromString("00000000-0000-0000-0000-000000000000");
    EXPECT_EQ(nil.toString(), "00000000-0000-0000-0000-000000000000");
}

// Test UUID copy constructor
TEST(UUIDTest, CopyConstructor) {
    UUID original = UUID::generate();
    UUID copy(original);
    
    EXPECT_EQ(original, copy);
    EXPECT_EQ(original.toString(), copy.toString());
}

// Test UUID assignment operator
TEST(UUIDTest, AssignmentOperator) {
    UUID uuid1 = UUID::generate();
    UUID uuid2 = UUID::generate();
    
    EXPECT_NE(uuid1, uuid2);
    
    uuid2 = uuid1;
    
    EXPECT_EQ(uuid1, uuid2);
    EXPECT_EQ(uuid1.toString(), uuid2.toString());
}

// Test UUID in STL containers
TEST(UUIDTest, STLContainers) {
    // Test with vector
    std::vector<UUID> uuids;
    UUID uuid1 = UUID::generate();
    UUID uuid2 = UUID::generate();
    UUID uuid3 = uuid1;
    
    uuids.push_back(uuid1);
    uuids.push_back(uuid2);
    uuids.push_back(uuid3);
    
    EXPECT_EQ(uuids.size(), 3);
    EXPECT_EQ(uuids[0], uuid1);
    EXPECT_EQ(uuids[1], uuid2);
    EXPECT_EQ(uuids[2], uuid1);  // Same as first
    
    // Test with map (using UUID as key)
    std::map<std::string, UUID> uuidMap;
    uuidMap["first"] = uuid1;
    uuidMap["second"] = uuid2;
    
    EXPECT_EQ(uuidMap.size(), 2);
    EXPECT_EQ(uuidMap["first"], uuid1);
    EXPECT_EQ(uuidMap["second"], uuid2);
}

// Test UUID toString round-trip
TEST(UUIDTest, ToStringRoundTrip) {
    for (int i = 0; i < 100; ++i) {
        UUID original = UUID::generate();
        std::string str = original.toString();
        UUID restored = UUID::fromString(str);
        
        EXPECT_EQ(original, restored);
        EXPECT_EQ(str, restored.toString());
    }
}

// Test UUID performance (should be reasonably fast)
TEST(UUIDTest, GenerationPerformance) {
    const int count = 10000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < count; ++i) {
        UUID uuid = UUID::generate();
        (void)uuid;  // Suppress unused variable warning
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should be able to generate 10,000 UUIDs in less than 1 second
    EXPECT_LT(duration.count(), 1000) << "UUID generation took " << duration.count() << "ms for " << count << " UUIDs";
}
