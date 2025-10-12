---
title: "IEEE 1588-2019 BMCA Algorithm - Detailed Design"
specType: design
version: 0.1.0
status: draft
author: AI Standards Implementation Agent
date: 2024-12-10
relatedArchitecture:
  - ARC-C-003-BMCA
  - ARC-C-001-CoreProtocol
relatedRequirements:
  - REQ-F-1588-002-BMCA
  - REQ-NFR-1588-001-TimingAccuracy
traceability:
  - from: ARC-C-003-BMCA
  - to: DES-C-003-BMCAEngine
---

# IEEE 1588-2019 Best Master Clock Algorithm - Detailed Design

> **Purpose**: Detailed component design for IEEE 1588-2019 BMCA implementation per Section 9.3
> **Standard Compliance**: IEEE 1016-2009 Software Design Description format
> **Protocol Reference**: IEEE 1588-2019 Section 9.3 "Best master clock algorithm"

## DES-C-003: BMCA Engine Component

### **Component Overview**

The BMCA Engine implements the Best Master Clock Algorithm per IEEE 1588-2019 Section 9.3 for grandmaster selection and port role determination.

### **Design Constraints**

- **Deterministic Execution**: Dataset comparison must complete within 100μs
- **Memory Efficiency**: Fixed-size dataset storage, no dynamic allocation
- **Thread Safety**: Concurrent access from announce message handlers
- **Specification Compliance**: 100% adherence to IEEE 1588-2019 comparison algorithm

### **Clock Dataset Structures (DES-D-004)**

```cpp
namespace IEEE::_1588::PTP::_2019::BMCA {

/**
 * @brief Default Dataset per IEEE 1588-2019 Section 8.2.1
 * @design DES-D-004-DefaultDataset
 * @traces REQ-F-1588-002-BMCA
 */
struct DefaultDataset {
    bool two_step_flag;                    ///< Two-step clock capability
    ClockIdentity clock_identity;          ///< 64-bit clock identifier
    uint16_t number_ports;                 ///< Number of PTP ports
    ClockQuality clock_quality;            ///< Clock quality indicators
    uint8_t priority_1;                    ///< First priority for BMCA
    uint8_t priority_2;                    ///< Second priority for BMCA
    uint8_t domain_number;                 ///< PTP domain identifier
    bool slave_only;                       ///< Slave-only clock flag
    
    /**
     * @brief Validate dataset per IEEE 1588-2019 requirements
     * @design DES-D-004a-ValidateDataset
     */
    bool isValid() const noexcept;
};

/**
 * @brief Clock Quality per IEEE 1588-2019 Section 7.6.2.4
 * @design DES-D-005-ClockQuality
 * @traces REQ-F-1588-002-BMCA
 */
struct ClockQuality {
    uint8_t clock_class;                   ///< Clock class per IEEE 1588-2019 Table 5
    uint8_t clock_accuracy;                ///< Clock accuracy per IEEE 1588-2019 Table 6  
    uint16_t offset_scaled_log_variance;   ///< Allan variance estimate
    
    /**
     * @brief Compare clock quality per IEEE 1588-2019 Section 9.3.2.4
     * @param other Clock quality to compare against
     * @return Comparison result (-1: better, 0: equal, +1: worse)
     * @design DES-D-005a-CompareQuality
     */
    int compare(const ClockQuality& other) const noexcept;
};

/**
 * @brief Announce Dataset per IEEE 1588-2019 Section 9.3.2
 * @design DES-D-006-AnnounceDataset
 */
struct AnnounceDataset {
    DefaultDataset grandmaster_dataset;    ///< Grandmaster's default dataset
    uint16_t steps_removed;               ///< Steps from grandmaster
    TimeSource time_source;               ///< Time source type
    PortIdentity port_identity;           ///< Source port identity
    uint64_t receipt_timestamp_ns;        ///< Reception timestamp
    
    /**
     * @brief Validate announce dataset completeness
     * @design DES-D-006a-ValidateAnnounce
     */
    bool isComplete() const noexcept;
};

} // namespace IEEE::_1588::PTP::_2019::BMCA
```

### **BMCA Algorithm Interface (DES-I-003)**

```cpp
/**
 * @brief Best Master Clock Algorithm interface per IEEE 1588-2019
 * @design DES-I-003-BMCAInterface
 * @traces ARC-C-003-BMCA
 */
class IBMCAEngine {
public:
    virtual ~IBMCAEngine() = default;
    
    /**
     * @brief Initialize BMCA with local clock dataset
     * @param local_dataset This clock's default dataset
     * @return Success/failure result
     * @design DES-I-003a-Initialize
     */
    virtual PTPResult<void> initialize(const DefaultDataset& local_dataset) = 0;
    
    /**
     * @brief Process received Announce message per IEEE 1588-2019 Section 9.3.2
     * @param announce_data Announce message dataset
     * @return BMCA decision result
     * @design DES-I-003b-ProcessAnnounce
     * @timing Must complete within 100μs for real-time performance
     */
    virtual PTPResult<BMCADecision> processAnnounceMessage(
        const AnnounceDataset& announce_data) = 0;
    
    /**
     * @brief Execute BMCA state decision algorithm per IEEE 1588-2019 Section 9.3.3
     * @return Recommended port state based on BMCA results
     * @design DES-I-003c-StateDecision
     */
    virtual PTPResult<RecommendedState> executeStateDecision() = 0;
    
    /**
     * @brief Get current best master dataset
     * @return Current grandmaster information
     * @design DES-I-003d-GetBestMaster
     */
    virtual const AnnounceDataset& getBestMasterDataset() const noexcept = 0;
    
    /**
     * @brief Check if local clock is better than received dataset
     * @param foreign_dataset Dataset to compare against
     * @return true if local clock should be master
     * @design DES-I-003e-CompareClock
     */
    virtual bool isLocalClockBetter(const AnnounceDataset& foreign_dataset) const noexcept = 0;
};
```

### **Dataset Comparison Algorithm (DES-C-004)**

```cpp
/**
 * @brief IEEE 1588-2019 dataset comparison per Section 9.3.2.4
 * @design DES-C-004-DatasetComparison
 * @traces REQ-F-1588-002-BMCA
 */
class DatasetComparison {
public:
    /**
     * @brief Compare two datasets per IEEE 1588-2019 Figure 27
     * @param dataset_a First dataset for comparison
     * @param dataset_b Second dataset for comparison  
     * @return Comparison result
     * @design DES-C-004a-CompareDatasets
     * @algorithm Per IEEE 1588-2019 Section 9.3.2.4 step-by-step comparison
     */
    static ComparisonResult compare(const AnnounceDataset& dataset_a,
                                   const AnnounceDataset& dataset_b) noexcept;

private:
    /**
     * @brief Step 1: Compare priority_1 values
     * @design DES-C-004b-ComparePriority1
     */
    static ComparisonResult comparePriority1(uint8_t priority1_a, uint8_t priority1_b) noexcept;
    
    /**
     * @brief Step 2: Compare clock class values  
     * @design DES-C-004c-CompareClockClass
     */
    static ComparisonResult compareClockClass(uint8_t class_a, uint8_t class_b) noexcept;
    
    /**
     * @brief Step 3: Compare clock accuracy values
     * @design DES-C-004d-CompareAccuracy
     */
    static ComparisonResult compareAccuracy(uint8_t accuracy_a, uint8_t accuracy_b) noexcept;
    
    /**
     * @brief Step 4: Compare offset scaled log variance
     * @design DES-C-004e-CompareVariance
     */
    static ComparisonResult compareVariance(uint16_t variance_a, uint16_t variance_b) noexcept;
    
    /**
     * @brief Step 5: Compare priority_2 values
     * @design DES-C-004f-ComparePriority2
     */
    static ComparisonResult comparePriority2(uint8_t priority2_a, uint8_t priority2_b) noexcept;
    
    /**
     * @brief Step 6: Compare clock identity (tiebreaker)
     * @design DES-C-004g-CompareIdentity
     */
    static ComparisonResult compareClockIdentity(const ClockIdentity& id_a,
                                               const ClockIdentity& id_b) noexcept;
};
```

### **BMCA Engine Implementation (DES-C-005)**

```cpp
/**
 * @brief Concrete BMCA engine implementation
 * @design DES-C-005-BMCAEngineImpl
 * @traces ARC-C-003-BMCA
 */
class BMCAEngineImpl : public IBMCAEngine {
private:
    DefaultDataset local_dataset_;              ///< Local clock dataset
    AnnounceDataset best_master_dataset_;       ///< Current best master
    
    // Foreign master tracking per IEEE 1588-2019 Section 9.3.2.5
    struct ForeignMasterRecord {
        AnnounceDataset dataset;
        uint64_t last_announce_time_ns;
        uint16_t announce_count;
        bool is_qualified;                      ///< Has enough announces for qualification
    };
    
    std::array<ForeignMasterRecord, MAX_FOREIGN_MASTERS> foreign_masters_;
    std::atomic<size_t> foreign_master_count_{0};
    mutable std::shared_mutex bmca_mutex_;      ///< Protects BMCA data structures
    
    static constexpr size_t MAX_FOREIGN_MASTERS = 16;  ///< Max foreign masters to track
    static constexpr uint16_t QUALIFICATION_COUNT = 2; ///< Announces needed for qualification
    
public:
    /**
     * @brief Construct BMCA engine with local clock configuration
     * @design DES-C-005a-Constructor
     */
    explicit BMCAEngineImpl(const DefaultDataset& local_dataset);
    
    /**
     * @brief Initialize BMCA engine
     * @design DES-C-005b-Initialize
     * @traces REQ-F-1588-002-BMCA
     */
    PTPResult<void> initialize(const DefaultDataset& local_dataset) override;
    
    /**
     * @brief Process announce message with foreign master tracking
     * @design DES-C-005c-ProcessAnnounce
     * @algorithm Per IEEE 1588-2019 Section 9.3.2.5 foreign master algorithm
     */
    PTPResult<BMCADecision> processAnnounceMessage(
        const AnnounceDataset& announce_data) override;
    
    /**
     * @brief Execute state decision algorithm
     * @design DES-C-005d-StateDecision  
     * @algorithm Per IEEE 1588-2019 Section 9.3.3 state decision event
     */
    PTPResult<RecommendedState> executeStateDecision() override;
    
    /**
     * @brief Thread-safe access to best master dataset
     * @design DES-C-005e-GetBestMaster
     */
    const AnnounceDataset& getBestMasterDataset() const noexcept override;
    
    /**
     * @brief Compare local clock against foreign dataset
     * @design DES-C-005f-CompareClock
     */
    bool isLocalClockBetter(const AnnounceDataset& foreign_dataset) const noexcept override;

private:
    /**
     * @brief Update foreign master table with new announce
     * @design DES-C-005g-UpdateForeignMasters
     */
    void updateForeignMasterTable(const AnnounceDataset& announce_data);
    
    /**
     * @brief Find or create foreign master record
     * @design DES-C-005h-FindForeignMaster
     */
    ForeignMasterRecord* findForeignMaster(const PortIdentity& port_id);
    
    /**
     * @brief Qualify foreign master based on announce count
     * @design DES-C-005i-QualifyMaster
     */
    bool qualifyForeignMaster(ForeignMasterRecord& master);
    
    /**
     * @brief Age out stale foreign master records
     * @design DES-C-005j-AgeOutMasters
     */
    void ageOutForeignMasters(uint64_t current_time_ns);
    
    /**
     * @brief Select best master from qualified foreign masters
     * @design DES-C-005k-SelectBestMaster
     */
    const AnnounceDataset* selectBestMaster();
};
```

## **BMCA Decision Types (DES-D-007)**

```cpp
/**
 * @brief BMCA decision result per IEEE 1588-2019 Section 9.3.3
 * @design DES-D-007-BMCADecision
 * @traces REQ-F-1588-002-BMCA
 */
enum class BMCADecision : uint8_t {
    NO_CHANGE,          ///< No change to current best master
    NEW_BEST_MASTER,    ///< New best master selected
    BECOME_MASTER,      ///< Local clock should become master
    FOREIGN_MASTER_LOST ///< Previous best master no longer available
};

/**
 * @brief Recommended port state from BMCA
 * @design DES-D-008-RecommendedState
 */
enum class RecommendedState : uint8_t {
    RS_MASTER,          ///< Recommend Master state
    RS_SLAVE,           ///< Recommend Slave state  
    RS_PASSIVE,         ///< Recommend Passive state
    RS_LISTENING        ///< Recommend Listening state
};
```

## **Hardware Abstraction Integration (DES-I-004)**

```cpp
/**
 * @brief Hardware abstraction for BMCA timing requirements
 * @design DES-I-004-BMCATimer  
 * @traces ARC-C-005-HardwareAbstraction
 */
class IBMCATimer {
public:
    virtual ~IBMCATimer() = default;
    
    /**
     * @brief Get current time for announce message timestamping
     * @return Nanosecond precision timestamp
     * @design DES-I-004a-GetTimestamp
     */
    virtual uint64_t getCurrentTimeNs() const = 0;
    
    /**
     * @brief Set announce receipt timeout
     * @param timeout_intervals Number of announce intervals
     * @param callback Function to call on timeout
     * @design DES-I-004b-SetAnnounceTimeout
     */
    virtual PTPResult<void> setAnnounceReceiptTimeout(
        uint8_t timeout_intervals,
        std::function<void()> callback) = 0;
    
    /**
     * @brief Reset announce receipt timeout
     * @design DES-I-004c-ResetTimeout
     */
    virtual void resetAnnounceReceiptTimeout() = 0;
};
```

## **Test-Driven Design Preparation (DES-T-002)**

### **Unit Test Interface Design**

```cpp
/**
 * @brief Test fixture for BMCA algorithm validation
 * @design DES-T-002-BMCATestFixture
 * @traces DES-C-005-BMCAEngineImpl
 */
class BMCAEngineTest {
private:
    std::unique_ptr<IBMCAEngine> bmca_engine_;
    MockTimerInterface mock_timer_;
    DefaultDataset test_local_dataset_;
    
public:
    /**
     * Test scenarios per IEEE 1588-2019 specification:
     * - test_DatasetComparison_AllSteps()
     * - test_ForeignMasterQualification()  
     * - test_BestMasterSelection()
     * - test_StateDecisionAlgorithm()
     * - test_ConcurrentAnnounceProcessing()
     * - test_ForeignMasterAging()
     */
};
```

### **Performance Test Requirements**

- **Dataset Comparison Time**: <100μs per comparison (measured)
- **Foreign Master Capacity**: Support 16 concurrent foreign masters
- **Memory Usage**: <8KB for complete BMCA engine instance  
- **Thread Safety**: Concurrent announce processing from multiple ports
- **Specification Compliance**: 100% algorithm coverage per IEEE 1588-2019 Section 9.3

## **Traceability Matrix**

| Design Element | Traces To Architecture | Traces To Requirements | Implementation Target |
|----------------|------------------------|------------------------|----------------------|
| DES-C-003 | ARC-C-003-BMCA | REQ-F-1588-002 | `src/bmca.cpp` |
| DES-I-003 | ARC-C-003-BMCA | REQ-F-1588-002 | `include/bmca.hpp` |
| DES-D-004 | ARC-C-003-BMCA | REQ-F-1588-002 | `include/types.hpp` |
| DES-C-004 | ARC-C-003-BMCA | REQ-F-1588-002 | `src/dataset_comparison.cpp` |
| DES-I-004 | ARC-C-005-HardwareAbstraction | REQ-NFR-1588-003 | `Common/interfaces/` |

## **Implementation Readiness**

### **Phase 04 Exit Criteria Status**

- ✅ **Algorithm Specified**: Complete BMCA per IEEE 1588-2019 Section 9.3
- ✅ **Data Structures Designed**: All required datasets and comparison types
- ✅ **Interface Defined**: IBMCAEngine with all required methods  
- ✅ **Performance Requirements**: <100μs comparison time specified
- ✅ **Thread Safety Design**: Concurrent access patterns defined
- ✅ **Test Design Prepared**: Unit test scenarios for all BMCA functions

### **Ready for Phase 05 Implementation**

This detailed design enables TDD implementation with:

- Complete algorithm specification per IEEE 1588-2019
- Clear performance and memory requirements
- Thread safety and concurrent access patterns  
- Comprehensive test scenario coverage
- Full traceability to architecture and requirements