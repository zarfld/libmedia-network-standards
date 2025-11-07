/**
 * @file ieee_1588.hpp
 * @brief IEEE::_1588 abstraction: fixed contract + versioned providers
 */

#ifndef IEEE_1588_ABSTRACTION_HPP
#define IEEE_1588_ABSTRACTION_HPP

#include <cstdint>
#include <type_traits>

#include "Common/interfaces/IIEEE1588Contract.hpp"
#include "Common/interfaces/IEEE1588Selector.hpp"

namespace IEEE {
namespace _1588 {

// Stable provider interface type (fixed contract)
using IProvider = ::Standards::Interfaces::IIEEE1588Contract;

// Version enum reflects only implemented versions (compiled based on defines)
enum class Version : uint8_t {
#if defined(IEEE_HAS_1588_2019)
    _2019,
#endif
#if defined(IEEE_HAS_1588_2008)
    _2008,
#endif
#if defined(IEEE_HAS_1588_2002)
    _2002,
#endif
};

// Map abstraction Version to selector Version
inline ::Standards::Interfaces::IEEE1588Selector::Version toSelector(Version v) noexcept {
    using Sel = ::Standards::Interfaces::IEEE1588Selector;
    switch (v) {
#if defined(IEEE_HAS_1588_2019)
        case Version::_2019: return Sel::Version::_2019;
#endif
#if defined(IEEE_HAS_1588_2008)
        case Version::_2008: return Sel::Version::_2008;
#endif
#if defined(IEEE_HAS_1588_2002)
        case Version::_2002: return Sel::Version::_2002;
#endif
        default: return Sel::Version::_2019; // safe fallback; may resolve via Latest mapping
    }
}

// Register provider factory for a specific version
inline bool register_provider(Version v, IProvider* (*create_fn)()) noexcept {
    return ::Standards::Interfaces::IEEE1588Selector::registerProvider(toSelector(v), create_fn);
}

// Create instance for a specific version
inline IProvider* create(Version v) noexcept {
    auto* fn = ::Standards::Interfaces::IEEE1588Selector::getProviderFactory(toSelector(v));
    return fn ? fn() : nullptr;
}

// Create best available (tries most recent downwards)
inline IProvider* create_best_available() noexcept {
    using Sel = ::Standards::Interfaces::IEEE1588Selector;
#if defined(IEEE_HAS_1588_2019)
    if (auto* f = Sel::getProviderFactory(Sel::Version::_2019)) return f();
#endif
#if defined(IEEE_HAS_1588_2008)
    if (auto* f = Sel::getProviderFactory(Sel::Version::_2008)) return f();
#endif
#if defined(IEEE_HAS_1588_2002)
    if (auto* f = Sel::getProviderFactory(Sel::Version::_2002)) return f();
#endif
    // As a final attempt, use whatever is registered as Latest
    if (auto* f = Sel::getProviderFactory(Sel::Version::Latest)) return f();
    return nullptr;
}

// Convenience macro for providers in versioned namespaces
#define IEEE_REGISTER_1588_PROVIDER(version_enum, create_fn) \
    static bool s_register_ieee1588_##version_enum = \
        ::IEEE::_1588::register_provider(::IEEE::_1588::Version::version_enum, (create_fn))

} // namespace _1588
} // namespace IEEE

#endif // IEEE_1588_ABSTRACTION_HPP
