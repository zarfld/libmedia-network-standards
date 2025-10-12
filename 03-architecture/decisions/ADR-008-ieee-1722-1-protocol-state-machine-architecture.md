---
specType: architecture
standard: "42010"
phase: "03-architecture"
version: "1.0.0"
author: "Architecture Team"
date: "2025-10-12"
status: "approved"
traceability:
  requirements:
    - "REQ-F-001"
    - "REQ-F-003"
    - "REQ-NF-001"
---

# ADR-008: IEEE 1722.1 Protocol State Machine Architecture

## Status

Accepted

## Context

IEEE 1722.1-2021 AVDECC defines three core protocols (ADP, AECP, ACMP) that must operate concurrently with complex state management requirements. Each protocol has distinct state machines with timing requirements and interactions between protocols.

### Protocol State Machine Requirements

- **ADP State Machine**: Entity discovery and advertisement with periodic behavior
- **AECP State Machine**: Command/response processing with timeout and retry logic
- **ACMP State Machine**: Connection management with complex multi-entity coordination
- **Concurrent Operation**: All protocols must operate simultaneously without interference
- **Timing Precision**: Protocol timers must meet IEEE 1722.1-2021 specification requirements

### Technical Challenges

- **Protocol Interactions**: State changes in one protocol can affect others (e.g., ACMP depends on ADP)
- **Timing Requirements**: Multiple concurrent timers with microsecond precision requirements
- **Error Handling**: Protocol failures must be isolated and not affect other protocols  
- **Resource Management**: Shared resources (network, entity database) must be coordinated
- **Real-time Constraints**: State machine processing must not block protocol message handling

## Stakeholder Concerns

- **Audio Engineers**: Require reliable device control and connection management for professional workflows
- **Network Engineers**: Need predictable protocol behavior and efficient network resource usage
- **Software Developers**: Require maintainable state machine implementation with clear debugging capabilities
- **Equipment Manufacturers**: Need robust protocol implementation supporting diverse device architectures

## Architectural Viewpoints

- **Concurrency Viewpoint**: Safe concurrent operation of multiple protocol state machines
- **Timing Viewpoint**: Precise timer management and protocol timing requirements
- **Reliability Viewpoint**: Fault isolation and error recovery across protocol boundaries
- **Maintainability Viewpoint**: Clear state machine structure enabling debugging and testing

## Decision

We will implement an **Event-Driven State Machine Architecture** with protocol isolation:

### 1. Protocol State Machine Framework

```cpp
namespace IEEE::_1722_1::_2021::protocols {

// Base state machine interface
class ProtocolStateMachine {
public:
    virtual ~ProtocolStateMachine() = default;
    virtual void processEvent(const ProtocolEvent& event) = 0;
    virtual void onTimer(TimerId timer_id) = 0;
    virtual bool isHealthy() const = 0;
    virtual std::string getStateName() const = 0;
    
protected:
    virtual void enterState(StateId new_state) = 0;
    virtual void exitState(StateId current_state) = 0;
    virtual TimerId startTimer(std::chrono::milliseconds duration, TimerType type) = 0;
    virtual void cancelTimer(TimerId timer_id) = 0;
};

// Protocol event base class  
class ProtocolEvent {
public:
    enum class Type {
        PACKET_RECEIVED,
        TIMER_EXPIRED,
        ENTITY_STATE_CHANGED,
        NETWORK_INTERFACE_CHANGED,
        PROTOCOL_ERROR
    };
    
    Type getType() const { return type_; }
    std::chrono::steady_clock::time_point getTimestamp() const { return timestamp_; }
    
protected:
    ProtocolEvent(Type type) : type_(type), timestamp_(std::chrono::steady_clock::now()) {}
    
private:
    Type type_;
    std::chrono::steady_clock::time_point timestamp_;
};

} // namespace IEEE::_1722_1::_2021::protocols
```

### 2. ADP State Machine Implementation

```cpp
namespace IEEE::_1722_1::_2021::adp {

class ADPStateMachine : public ProtocolStateMachine {
public:
    enum class State {
        INITIAL,
        WAITING,
        ADVERTISE,
        DEPARTING
    };
    
    ADPStateMachine(const EntityModel& entity_model, NetworkInterface& network);
    
    void processEvent(const ProtocolEvent& event) override;
    void onTimer(TimerId timer_id) override;
    bool isHealthy() const override;
    
    // ADP-specific operations
    void startAdvertising();
    void stopAdvertising();
    void sendEntityDiscover();
    
private:
    State current_state_ = State::INITIAL;
    const EntityModel& entity_model_;
    NetworkInterface& network_interface_;
    
    // ADP timers per IEEE 1722.1-2021
    TimerId advertise_timer_;
    TimerId discover_timer_;
    std::chrono::milliseconds advertise_interval_{2000}; // 2-62 seconds configurable
    
    // State transition handlers
    void handleWaitingState(const ProtocolEvent& event);
    void handleAdvertiseState(const ProtocolEvent& event);
    void handleDepartingState(const ProtocolEvent& event);
    
    // Protocol message handlers
    void processEntityAvailable(const EntityAvailableMessage& msg);
    void processEntityDiscover(const EntityDiscoverMessage& msg);
    void processEntityDeparting(const EntityDepartingMessage& msg);
    
    void sendEntityAvailable();
    void sendEntityDeparting();
};

} // namespace IEEE::_1722_1::_2021::adp
```

### 3. AECP State Machine Implementation

```cpp
namespace IEEE::_1722_1::_2021::aecp {

class AECPStateMachine : public ProtocolStateMachine {
public:
    enum class State {
        WAITING_FOR_COMMAND,
        PROCESSING_COMMAND,
        WAITING_FOR_RESPONSE,
        ACQUIRED
    };
    
    AECPStateMachine(EntityModel& entity_model, NetworkInterface& network);
    
    void processEvent(const ProtocolEvent& event) override;
    void onTimer(TimerId timer_id) override;
    
    // AECP command processing
    void sendCommand(const AEMCommand& command, const MacAddress& target);
    void processResponse(const AEMResponse& response);
    
private:
    State current_state_ = State::WAITING_FOR_COMMAND;
    EntityModel& entity_model_;
    NetworkInterface& network_interface_;
    
    // Command tracking for timeout/retry
    struct PendingCommand {
        uint16_t sequence_number;
        AEMCommand command;
        MacAddress target_address;
        std::chrono::steady_clock::time_point send_time;
        uint8_t retry_count;
        CommandCallback callback;
    };
    
    std::unordered_map<uint16_t, PendingCommand> pending_commands_;
    TimerId command_timeout_timer_;
    uint16_t next_sequence_number_ = 0;
    
    // State handlers
    void handleWaitingForCommand(const ProtocolEvent& event);
    void handleProcessingCommand(const ProtocolEvent& event);
    void handleWaitingForResponse(const ProtocolEvent& event);
    void handleAcquired(const ProtocolEvent& event);
    
    // AEM command processors
    void processReadDescriptor(const AEMCommand& command);
    void processAcquireEntity(const AEMCommand& command);
    void processSetConfiguration(const AEMCommand& command);
    void processMilanCommand(const AEMCommand& command);
    
    void sendResponse(uint16_t sequence_number, AEMStatusCode status, 
                     const std::vector<uint8_t>& payload);
    void handleCommandTimeout(uint16_t sequence_number);
};

} // namespace IEEE::_1722_1::_2021::aecp
```

### 4. ACMP State Machine Implementation

```cpp
namespace IEEE::_1722_1::_2021::acmp {

class ACMPStateMachine : public ProtocolStateMachine {
public:
    enum class State {
        WAITING,
        CONNECTING,
        CONNECTED,
        DISCONNECTING,
        FAST_CONNECT
    };
    
    ACMPStateMachine(EntityModel& entity_model, NetworkInterface& network);
    
    void processEvent(const ProtocolEvent& event) override;
    void onTimer(TimerId timer_id) override;
    
    // Stream connection management
    void connectStream(const StreamConnectionRequest& request);
    void disconnectStream(const StreamID& stream_id);
    ConnectionState getConnectionState(const StreamID& stream_id) const;
    
private:
    State current_state_ = State::WAITING;
    EntityModel& entity_model_;
    NetworkInterface& network_interface_;
    
    // Connection database
    std::unordered_map<StreamID, StreamConnection> active_connections_;
    std::unordered_map<uint16_t, PendingConnection> pending_connections_;
    
    TimerId connection_timeout_timer_;
    uint16_t next_sequence_number_ = 0;
    
    // State handlers
    void handleWaiting(const ProtocolEvent& event);
    void handleConnecting(const ProtocolEvent& event);
    void handleConnected(const ProtocolEvent& event);
    void handleDisconnecting(const ProtocolEvent& event);
    
    // ACMP message processors
    void processConnectTXCommand(const ACMPMessage& message);
    void processConnectRXCommand(const ACMPMessage& message);
    void processDisconnectTXCommand(const ACMPMessage& message);
    void processGetTXStateCommand(const ACMPMessage& message);
    
    // Connection validation and management
    bool validateStreamConnection(const StreamConnectionRequest& request) const;
    void establishConnection(const StreamConnection& connection);
    void teardownConnection(const StreamID& stream_id);
};

} // namespace IEEE::_1722_1::_2021::acmp
```

### 5. Protocol Coordinator

```cpp
class AVDECCProtocolCoordinator {
public:
    AVDECCProtocolCoordinator(EntityModel& entity_model, NetworkInterface& network);
    
    void start();
    void stop();
    
    // Protocol access
    ADPStateMachine& getADP() { return adp_state_machine_; }
    AECPStateMachine& getAECP() { return aecp_state_machine_; }
    ACMPStateMachine& getACMP() { return acmp_state_machine_; }
    
    // Event distribution
    void dispatchEvent(const ProtocolEvent& event);
    
private:
    EntityModel& entity_model_;
    NetworkInterface& network_interface_;
    
    // Protocol state machines
    ADPStateMachine adp_state_machine_;
    AECPStateMachine aecp_state_machine_;
    ACMPStateMachine acmp_state_machine_;
    
    // Event processing
    EventQueue event_queue_;
    std::thread event_processor_thread_;
    std::atomic<bool> running_{false};
    
    // Timer management
    TimerManager timer_manager_;
    
    void eventProcessorLoop();
    void handleInterProtocolCommunication(const ProtocolEvent& event);
};
```

## Rationale

### **Event-Driven Architecture Benefits**

- **Protocol Isolation**: Each protocol state machine operates independently
- **Scalable Concurrency**: Event queue handles concurrent protocol operations
- **Testability**: State machines can be tested independently with synthetic events
- **Debugging**: Event history provides clear audit trail for protocol behavior

### **Timing Precision**

- **Dedicated Timer Manager**: High-precision timers for protocol timing requirements  
- **Timer Coalescing**: Multiple timers combined for efficiency while maintaining precision
- **Priority Scheduling**: Critical protocol timers have priority over non-critical operations
- **Jitter Reduction**: Timer implementation minimizes timing jitter for AVDECC compliance

### **Error Isolation**

- **Exception Boundaries**: Protocol failures contained within individual state machines
- **Health Monitoring**: State machines report health status for system diagnostics
- **Recovery Mechanisms**: Automatic recovery from transient failures with exponential backoff
- **Graceful Degradation**: System continues operation when individual protocols fail

## Consequences

### **Positive Impacts**

- **IEEE 1722.1 Compliance**: State machine architecture directly follows IEEE specification
- **Concurrent Operation**: Protocols operate efficiently without blocking each other
- **Real-time Performance**: Event-driven architecture meets professional audio timing requirements
- **Maintainability**: Clear state separation enables focused development and debugging
- **Scalability**: Architecture supports devices with hundreds of streams and connections

### **Negative Impacts**

- **Memory Overhead**: Event queues and state tracking require additional memory  
- **Implementation Complexity**: Event-driven architecture has higher development complexity
- **Debugging Complexity**: Concurrent state machines can be challenging to debug
- **Event Latency**: Event queue processing adds small latency to protocol operations

### **Risk Mitigation**

- **Comprehensive Testing**: State machine testing with IEEE 1722.1 compliance test vectors
- **Performance Monitoring**: Real-time monitoring of event queue depth and processing latency
- **Event Tracing**: Detailed event logging for debugging complex protocol interactions
- **Fallback Mechanisms**: Simplified protocol behavior when event queue overloads

## Implementation Requirements

- Event-driven state machine framework with microsecond-precision timers
- Complete implementation of ADP, AECP, and ACMP state machines per IEEE 1722.1-2021
- Thread-safe event queue with priority scheduling for critical protocol events
- Comprehensive error handling with protocol isolation and recovery mechanisms
- Performance monitoring and diagnostics for protocol health and timing compliance
- Integration with hardware abstraction layer for network interface access

## Verification Criteria

- All protocol state machines pass IEEE 1722.1-2021 compliance testing
- Protocol timing requirements met with <1ms jitter for critical operations
- Concurrent protocol operation with no deadlocks or race conditions
- State machine recovery from all defined error conditions within specification timeouts
- Event processing latency <100μs for 99.9% of events under normal load
- Memory usage scales predictably with number of active connections and commands

**References**: IEEE 1722.1-2021, ADR-007 (Entity Model Architecture), ADR-001 (Hardware Abstraction)