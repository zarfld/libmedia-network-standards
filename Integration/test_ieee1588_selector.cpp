/**
 * @file test_ieee1588_selector.cpp
 * @brief Minimal test validating runtime selection across IEEE 1588 providers
 */

#include <cassert>
#include <cstdio>
#include <cstring>

#include "Common/interfaces/IIEEE1588Contract.hpp"
#include "Common/interfaces/IEEE1588Selector.hpp"
#include "Common/interfaces/IEEE1588Runtime.hpp"

using namespace Standards::Interfaces;

// ---------------- Mock Providers ----------------

class MockPTP2019 : public IIEEE1588Contract {
public:
    const char* getVersion() const noexcept override { return "2019"; }
    bool hasCapability(uint32_t cap) const noexcept override {
        const uint32_t all = CAP_IEEE1588_2019;
        return (cap & ~all) == 0;
    }
    int getCurrentTime(Timestamp& ts) const noexcept override { ts = Timestamp(1, 2); return SUCCESS; }
    int getClockIdentity(ClockIdentity& id) const noexcept override { (void)id; return SUCCESS; }
    int getPortIdentity(uint16_t, PortIdentity& id) const noexcept override { (void)id; return SUCCESS; }
    int getClockQuality(ClockQuality& q) const noexcept override { (void)q; return SUCCESS; }
    uint8_t getPriority1() const noexcept override { return 128; }
    uint8_t getPriority2() const noexcept override { return 128; }
    SyncState getSyncState(uint16_t) const noexcept override { return SYNC_STATE_MASTER; }
    int64_t getOffsetFromMaster(uint16_t) const noexcept override { return 0; }
    uint64_t getMeanPathDelay(uint16_t) const noexcept override { return 0; }
};

class MockPTP2008 : public IIEEE1588Contract {
public:
    const char* getVersion() const noexcept override { return "2008"; }
    bool hasCapability(uint32_t cap) const noexcept override {
        const uint32_t all = CAP_IEEE1588_2008;
        return (cap & ~all) == 0;
    }
    int getCurrentTime(Timestamp& ts) const noexcept override { ts = Timestamp(3, 4); return SUCCESS; }
    int getClockIdentity(ClockIdentity& id) const noexcept override { (void)id; return SUCCESS; }
    int getPortIdentity(uint16_t, PortIdentity& id) const noexcept override { (void)id; return SUCCESS; }
    int getClockQuality(ClockQuality& q) const noexcept override { (void)q; return SUCCESS; }
    uint8_t getPriority1() const noexcept override { return 129; }
    uint8_t getPriority2() const noexcept override { return 129; }
    SyncState getSyncState(uint16_t) const noexcept override { return SYNC_STATE_SLAVE; }
    int64_t getOffsetFromMaster(uint16_t) const noexcept override { return 100; }
    uint64_t getMeanPathDelay(uint16_t) const noexcept override { return 50; }
};

static IIEEE1588Contract* Create2019() { static MockPTP2019 impl; return &impl; }
static IIEEE1588Contract* Create2008() { static MockPTP2008 impl; return &impl; }

// Register providers
STANDARDS_REGISTER_IEEE1588_PROVIDER(_2019, &Create2019)
STANDARDS_REGISTER_IEEE1588_PROVIDER(_2008, &Create2008)
// Also treat Latest as 2019 for this test
STANDARDS_REGISTER_IEEE1588_PROVIDER(Latest, &Create2019)

int main() {
    // Explicit property selection
    IEEE1588Selector::setSelectedVersion(IEEE1588Selector::Version::_2019);
    IIEEE1588Contract* p = IEEE1588Selector::create();
    assert(p && std::strcmp(p->getVersion(), "2019") == 0);
    assert(p->hasCapability(IIEEE1588Contract::CAP_SECURITY));

    // Switch to 2008 at runtime
    IEEE1588Selector::setSelectedVersion(IEEE1588Selector::Version::_2008);
    p = IEEE1588Selector::create();
    assert(p && std::strcmp(p->getVersion(), "2008") == 0);
    assert(!p->hasCapability(IIEEE1588Contract::CAP_SECURITY));
    assert(p->hasCapability(IIEEE1588Contract::CAP_PEER_DELAY));

    // Request unavailable version (2002) -> fallback to Latest (2019)
    IEEE1588Selector::setSelectedVersion(IEEE1588Selector::Version::_2002);
    p = IEEE1588Selector::create();
    assert(p && std::strcmp(p->getVersion(), "2019") == 0);

    // Env parsing helper (does not rely on process env in this test)
    auto v = IEEE1588Selector::parse("2008");
    assert(v == IEEE1588Selector::Version::_2008);

    std::puts("test_ieee1588_selector: PASS");
    return 0;
}
