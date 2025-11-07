/**
 * @file IEEE1588Selector.hpp
 * @brief Lightweight selection and registration helpers for IEEE 1588 providers
 *
 * Provides a stable mechanism for dependents to request a specific IEEE 1588
 * implementation version (e.g., latest, 2019, 2008, 2002) while consuming only
 * the fixed IIEEE1588Contract interface.
 *
 * Notes:
 * - Header-only, no linking required
 * - Simple function-pointer registry to avoid static init order pitfalls
 * - No exceptions; trivial thread-safety via function-local statics
 */

#ifndef STANDARDS_INTERFACES_IEEE1588_SELECTOR_HPP
#define STANDARDS_INTERFACES_IEEE1588_SELECTOR_HPP

#include <cstdint>
#include <cstddef>
#include <utility>
#include <atomic>

#include "IIEEE1588Contract.hpp"

namespace Standards {
namespace Interfaces {

class IEEE1588Selector {
public:
    enum class Version : uint8_t {
        Latest = 0,
        _2019  = 1,
        _2008  = 2,
        _2002  = 3
    };

    using CreateFn = IIEEE1588Contract* (*)();

    // Returns currently selected version (does not imply availability)
    static Version selectedVersion() noexcept {
        return s_selected();
    }

    // Parse common version strings into Version enum
    static Version parse(const char* v) noexcept {
        if (!v) return Version::Latest;
        // Accept case-insensitive common tokens
        if (equals(v, "latest")) return Version::Latest;
        if (equals(v, "2019"))  return Version::_2019;
        if (equals(v, "2008"))  return Version::_2008;
        if (equals(v, "2002"))  return Version::_2002;
        return Version::Latest;
    }

    // Allow host to set a desired version policy (optional)
    static void setSelectedVersion(Version v) noexcept { s_selected() = v; }

    // Provider registration for a specific version
    static bool registerProvider(Version v, CreateFn fn) noexcept {
        auto& slot = slotFor(v);
        slot.store(fn, std::memory_order_release);
        return true;
    }

    // Get a provider factory for a specific version; falls back to Latest, then null
    static CreateFn getProviderFactory(Version v) noexcept {
        // Exact match
        if (auto fn = slotFor(v).load(std::memory_order_acquire)) return fn;
        // Fallback to Latest if not already asking for Latest
        if (v != Version::Latest) {
            if (auto f2 = slotFor(Version::Latest).load(std::memory_order_acquire)) return f2;
        }
        return nullptr;
    }

    // Convenience: create instance using selected version policy
    static IIEEE1588Contract* create() noexcept {
        if (auto fn = getProviderFactory(selectedVersion())) return fn();
        return nullptr;
    }

private:
    static bool equals(const char* a, const char* b) noexcept {
        // simple case-insensitive compare for ASCII tokens
        while (*a && *b) {
            char ca = *a, cb = *b;
            if (ca >= 'A' && ca <= 'Z') ca = static_cast<char>(ca - 'A' + 'a');
            if (cb >= 'A' && cb <= 'Z') cb = static_cast<char>(cb - 'A' + 'a');
            if (ca != cb) return false;
            ++a; ++b;
        }
        return *a == *b;
    }

    static std::atomic<CreateFn>& slotFor(Version v) noexcept {
        switch (v) {
            case Version::Latest: return s_latest();
            case Version::_2019:  return s_2019();
            case Version::_2008:  return s_2008();
            case Version::_2002:  return s_2002();
            default:              return s_latest();
        }
    }

    static std::atomic<CreateFn>& s_latest() noexcept { static std::atomic<CreateFn> x{nullptr}; return x; }
    static std::atomic<CreateFn>& s_2019()  noexcept { static std::atomic<CreateFn> x{nullptr}; return x; }
    static std::atomic<CreateFn>& s_2008()  noexcept { static std::atomic<CreateFn> x{nullptr}; return x; }
    static std::atomic<CreateFn>& s_2002()  noexcept { static std::atomic<CreateFn> x{nullptr}; return x; }
    static Version& s_selected() noexcept { static Version v = Version::Latest; return v; }
};

// Helper macro for adapters to register their factory easily
#define STANDARDS_REGISTER_IEEE1588_PROVIDER(version_enum, create_fn) \
    static bool s_register_ieee1588_##version_enum = \
        ::Standards::Interfaces::IEEE1588Selector::registerProvider( \
            ::Standards::Interfaces::IEEE1588Selector::Version::version_enum, \
            (create_fn));

} // namespace Interfaces
} // namespace Standards

#endif // STANDARDS_INTERFACES_IEEE1588_SELECTOR_HPP
