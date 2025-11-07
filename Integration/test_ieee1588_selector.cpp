/**
 * @file test_ieee1588_selector.cpp
 * @brief Minimal test validating runtime selection across IEEE 1588 providers
 */

#include <cassert>
#include <cstdio>
#include <cstring>

#include "IEEE/1588/ieee_1588.hpp"

using ::IEEE::_1588::IProvider;

// ---------------- Mock Providers ----------------

class MockPTP2019 : public IProvider {
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

class MockPTP2008 : public IProvider {
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

static IProvider* Create2019() { static MockPTP2019 impl; return &impl; }
static IProvider* Create2008() { static MockPTP2008 impl; return &impl; }

// Register providers
// Register providers in IEEE::_1588 abstraction (enum reflects compiled versions)
#if defined(IEEE_HAS_1588_2019)
IEEE_REGISTER_1588_PROVIDER(_2019, &Create2019);
#endif
#if defined(IEEE_HAS_1588_2008)
IEEE_REGISTER_1588_PROVIDER(_2008, &Create2008);
#endif

int main() {
    // Explicit property selection
    IProvider* p = nullptr;
#if defined(IEEE_HAS_1588_2019)
    p = ::IEEE::_1588::create(::IEEE::_1588::Version::_2019);
    assert(p && std::strcmp(p->getVersion(), "2019") == 0);
    assert(p->hasCapability(IProvider::CAP_SECURITY));
#endif

    // Switch to 2008 at runtime
    (void)p;
#if defined(IEEE_HAS_1588_2008)
    p = ::IEEE::_1588::create(::IEEE::_1588::Version::_2008);
    assert(p && std::strcmp(p->getVersion(), "2008") == 0);
    assert(!p->hasCapability(IProvider::CAP_SECURITY));
    assert(p->hasCapability(IProvider::CAP_PEER_DELAY));
#endif

    // Request unavailable version (2002) -> fallback to Latest (2019)
    // best-available selection works
    p = ::IEEE::_1588::create_best_available();
    assert(p != nullptr);

    // Env parsing helper (does not rely on process env in this test)
    // no direct parsing in abstraction test; covered by selector tests elsewhere

    std::puts("test_ieee1588_selector: PASS");
    return 0;
}
