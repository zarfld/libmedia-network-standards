# IEEE 1722.1-2021 State Machine Library Design Plan

## Overview

This document outlines the library design strategy for our IEEE 1722.1-2021 state machine implementation. The goal is to provide a foundational library that applications can build upon while maintaining clean separation between:

1. **Core Protocol Implementation** - What we implement internally
2. **Application Integration Points** - What applications must provide
3. **Optional Extension Points** - What applications can override

## Design Philosophy

### Library Responsibilities (What We Implement)
- **Complete IEEE 1722.1-2021 state machine logic**
- **Protocol PDU serialization/deserialization** 
- **Message timing and retransmission**
- **State transitions and validation**
- **Thread-safe operation**
- **Default network interface abstraction**

### Application Responsibilities (What Apps Must Provide)
- **Network transport layer** (raw socket, platform-specific networking)
- **Entity model implementation** (descriptors, entity capabilities)
- **Stream connection callbacks** (actual audio/video streaming)
- **Entity discovery callbacks** (how to handle discovered entities)

### Optional Extension Points (What Apps Can Override)
- **Custom message validation**
- **Custom timing parameters**
- **Custom logging/debugging**
- **Custom entity behaviors**

## Architecture Layers

### Layer 1: Core State Machines (100% Internal)
```cpp
namespace IEEE::_1722_1::_2021::StateMachines {
    // Complete state machine implementations
    class ADPDiscoveryStateMachine;
    class ACMPControllerStateMachine;
    class AECPEntityModelEntityStateMachine;
    // etc.
}
```

### Layer 2: Protocol Interface (Mixed - Some Internal, Some App-Provided)
```cpp
namespace IEEE::_1722_1::_2021::Protocol {
    // We provide PDU serialization/deserialization
    class ADPMessageSerializer;
    class ACMPMessageSerializer;
    class AECPMessageSerializer;
    
    // Application provides network transport
    class INetworkInterface {
        virtual bool sendRawMessage(const uint8_t* data, size_t length, const MacAddress& dest) = 0;
        virtual MacAddress getLocalMacAddress() const = 0;
    };
}
```

### Layer 3: Entity Model Interface (App-Provided)
```cpp
namespace IEEE::_1722_1::_2021::EntityModel {
    // Application implements entity-specific behavior
    class IEntityDelegate {
        virtual void onEntityDiscovered(const DiscoveredEntity& entity) = 0;
        virtual void onStreamConnectionRequest(const StreamConnectionRequest& request, StreamConnectionResponse& response) = 0;
        virtual void onDescriptorReadRequest(DescriptorType type, uint16_t index, DescriptorData& data) = 0;
    };
}
```

### Layer 4: Application API (Public Interface)
```cpp
namespace IEEE::_1722_1::_2021::Library {
    // Main library entry point
    class AVDECCLibrary {
        // Constructor requires app-provided network interface and entity delegate
        AVDECCLibrary(std::unique_ptr<INetworkInterface> network, 
                     std::unique_ptr<IEntityDelegate> entityDelegate);
        
        // Library manages all state machines internally
        void start();
        void stop();
        
        // High-level operations
        void discoverEntities();
        void connectStream(EntityID talker, EntityID listener, uint16_t talkerIndex, uint16_t listenerIndex);
        void readDescriptor(EntityID entity, DescriptorType type, uint16_t index);
    };
}
```

## Implementation Strategy

### Phase 1: Core State Machine Library
- Complete state machine method implementations
- Internal message serialization/deserialization
- Thread-safe manager with proper lifecycle

### Phase 2: Interface Abstraction Layer
- Define clean interfaces for network and entity model
- Provide default implementations where possible
- Create factory pattern for easy instantiation

### Phase 3: Application Integration Layer
- High-level API for common operations
- Example implementations for different use cases
- Documentation and examples

### Phase 4: Advanced Features
- Custom protocol extensions
- Performance optimization hooks
- Advanced debugging/monitoring

## Key Design Decisions

### 1. Delegate Pattern vs Virtual Inheritance
**Decision**: Use delegate pattern with interfaces
**Rationale**: Allows applications to use composition, easier testing, cleaner separation

### 2. Internal vs External Threading
**Decision**: Library manages threading internally
**Rationale**: Simplifies application integration, ensures thread safety, consistent timing

### 3. Message Serialization
**Decision**: Library handles all PDU serialization/deserialization
**Rationale**: Complex IEEE format specifications, reduces application complexity

### 4. Network Abstraction Level
**Decision**: Abstract at raw Ethernet level
**Rationale**: Maintains platform flexibility while handling AVDECC-specific requirements

### 5. Entity Model Flexibility
**Decision**: Application provides entity model through delegates
**Rationale**: Entity models vary widely between applications, maximum flexibility needed

## Example Usage Pattern

```cpp
// Application provides network interface
class MyNetworkInterface : public IEEE::_1722_1::_2021::Protocol::INetworkInterface {
    bool sendRawMessage(const uint8_t* data, size_t length, const MacAddress& dest) override {
        // Platform-specific raw socket send
        return sendEthernetFrame(data, length, dest);
    }
    
    MacAddress getLocalMacAddress() const override {
        return getAdapterMacAddress();
    }
};

// Application provides entity behavior
class MyEntityDelegate : public IEEE::_1722_1::_2021::EntityModel::IEntityDelegate {
    void onEntityDiscovered(const DiscoveredEntity& entity) override {
        // Add to UI, update database, etc.
        addEntityToList(entity);
    }
    
    void onStreamConnectionRequest(const StreamConnectionRequest& request, StreamConnectionResponse& response) override {
        // Check if we can accept this connection
        if (canAcceptStream(request)) {
            response.status = ACMP_Status::SUCCESS;
            response.streamInfo = getStreamInfo(request.listenerUniqueID);
        } else {
            response.status = ACMP_Status::LISTENER_EXCLUSIVE;
        }
    }
};

// Application uses library
int main() {
    auto network = std::make_unique<MyNetworkInterface>();
    auto entityDelegate = std::make_unique<MyEntityDelegate>();
    
    IEEE::_1722_1::_2021::Library::AVDECCLibrary library(std::move(network), std::move(entityDelegate));
    
    library.start();
    library.discoverEntities();
    
    // Library handles all IEEE 1722.1-2021 protocol operations internally
    // Application receives callbacks for entity-specific decisions
    
    return 0;
}
```

## Benefits of This Design

1. **Standards Compliance**: Complete IEEE 1722.1-2021 implementation in library
2. **Application Simplicity**: Apps focus on entity-specific logic, not protocol details
3. **Platform Flexibility**: Network layer abstraction supports different platforms
4. **Testability**: Clear interfaces enable comprehensive testing
5. **Extensibility**: Delegate pattern allows customization without library changes
6. **Performance**: Internal threading and message handling optimized for real-time operation

## Next Steps

1. Refactor current state machine implementations to separate protocol logic from interface dependencies
2. Create clean interface definitions for network and entity model layers
3. Implement protocol message serialization/deserialization
4. Create high-level library API with proper lifecycle management
5. Develop example applications demonstrating different use cases

This design provides a solid foundation for IEEE 1722.1-2021 applications while maintaining clean separation of concerns and maximum flexibility for different use cases.
