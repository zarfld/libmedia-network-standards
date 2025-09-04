#pragma once

#include <cstdint>
#include <cstring>

// Ensure proper Windows header ordering to prevent template linkage conflicts
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    
    // Include C++ STL headers first to establish proper linkage
    #include <type_traits>
    #include <vector>
    #include <memory>
    
    // Now include Windows headers with proper C++ linkage
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <intrin.h>  // For _byteswap_uint64
    #pragma comment(lib, "ws2_32.lib")
    
    #ifndef htobe64
        #define htobe64(x) _byteswap_uint64(x)
        #define be64toh(x) _byteswap_uint64(x)
        #define htonll(x) _byteswap_uint64(x)
        #define ntohll(x) _byteswap_uint64(x)
    #endif
#else
    #include <arpa/inet.h>
    #include <endian.h>
#endif

namespace IEEE {
namespace _1722_1 {
namespace _2021 {

// Basic IEEE 1722.1-2021 working implementation
// This provides the minimal working foundation

// Common AVDECC Constants
const uint8_t AVDECC_MULTICAST_MAC[6] = {0x91, 0xE0, 0xF0, 0x01, 0x00, 0x00};
const uint16_t AVDECC_ETHERTYPE = 0x22F0;

// Entity ID type
using EntityID = uint64_t;

// Basic AVDECC Entity class that actually compiles
class AVDECCEntity {
private:
    EntityID entity_id;
    EntityID entity_model_id;
    uint32_t entity_capabilities;
    uint16_t talker_stream_sources;
    uint16_t talker_capabilities;
    uint16_t listener_stream_sinks;
    uint16_t listener_capabilities;
    uint32_t controller_capabilities;
    uint32_t available_index;
    EntityID association_id;

public:
    // Constructor
    AVDECCEntity(EntityID id = 0, EntityID model_id = 0);
    
    // Basic accessors
    EntityID get_entity_id() const { return entity_id; }
    void set_entity_id(EntityID id) { entity_id = id; }
    
    EntityID get_entity_model_id() const { return entity_model_id; }
    void set_entity_model_id(EntityID model_id) { entity_model_id = model_id; }
    
    uint32_t get_available_index() const { return available_index; }
    void increment_available_index() { available_index++; }
    
    // Capability checks
    bool supports_aem() const;
    bool has_talker_capabilities() const;
    bool has_listener_capabilities() const;
    
    // Name management
    void set_entity_name(const char* name);
    void set_firmware_version(const char* version);
    void set_group_name(const char* name);
    void set_serial_number(const char* serial);
};

// Basic AEM Command structure that compiles
class AEMCommand {
public:
    enum CommandType : uint16_t {
        READ_DESCRIPTOR = 0x0004,
        WRITE_DESCRIPTOR = 0x0005,
        SET_CONFIGURATION = 0x0008,
        GET_CONFIGURATION = 0x0009
    };
    
private:
    CommandType command_type;
    EntityID target_entity_id;
    uint16_t sequence_id;
    bool unsolicited;
    bool controller_request;

public:
    AEMCommand(CommandType cmd_type = READ_DESCRIPTOR);
    AEMCommand(CommandType cmd_type, EntityID target_id, uint16_t seq_id);
    
    // Serialization
    size_t serialize(uint8_t* buffer, size_t buffer_size) const;
    bool deserialize(const uint8_t* buffer, size_t buffer_size);
    
    // Accessors
    CommandType get_command_type() const { return command_type; }
    EntityID get_target_entity_id() const { return target_entity_id; }
    uint16_t get_sequence_id() const { return sequence_id; }
};

// Basic Entity Descriptor that compiles
class EntityDescriptor {
public:
    enum DescriptorType : uint16_t {
        ENTITY = 0x0000,
        CONFIGURATION = 0x0001,
        AUDIO_UNIT = 0x0002,
        STREAM_INPUT = 0x0005,
        STREAM_OUTPUT = 0x0006
    };
    
private:
    DescriptorType descriptor_type;
    uint16_t descriptor_index;
    EntityID entity_id;
    EntityID entity_model_id;

public:
    EntityDescriptor();
    
    // Serialization
    size_t serialize(uint8_t* buffer, size_t buffer_size) const;
    bool deserialize(const uint8_t* buffer, size_t buffer_size);
    
    // Accessors
    DescriptorType get_descriptor_type() const { return descriptor_type; }
    void set_descriptor_type(DescriptorType type) { descriptor_type = type; }
    
    uint16_t get_descriptor_index() const { return descriptor_index; }
    void set_descriptor_index(uint16_t index) { descriptor_index = index; }
};

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
