#ifndef SECURITY_CRYPTOGRAPHY_H
#define SECURITY_CRYPTOGRAPHY_H

/**
 * @file security_cryptography.h
 * @brief Security and Cryptography Extensions for IEEE 1722-2016
 * 
 * Complete implementation of ECC, AES-SIV, digital signatures, and key management
 * for secure AVTP communications per IEEE 1722-2016 "tutti completo"
 * 
 * @author OpenAvnu IEEE 1722-2016 Compliance Team
 * @date July 2025
 * @version 4.0.0 - Phase 6 Security & Cryptography Implementation
 */

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <array>
#include <map>
#include <chrono>
#include <functional>

namespace avtp_protocol {
namespace security {

// =============================
// AES-SIV (Synthetic Initialization Vector) Implementation
// =============================
namespace aes_siv {

/**
 * @brief AES-SIV Configuration per RFC 5297
 * Provides authenticated encryption with associated data (AEAD)
 */
struct AESSIVConfig {
    std::array<uint8_t, 32> key;        // 256-bit AES key
    std::array<uint8_t, 64> key_s2v;    // S2V subkey (derived from master key)
    std::array<uint8_t, 32> key_ctr;    // CTR subkey (derived from master key)
    uint32_t key_derivation_rounds;     // Number of key derivation rounds
    bool use_hardware_acceleration;     // Use AES-NI if available
};

/**
 * @brief AES-SIV Encrypted Frame for AVTP
 */
struct AESSIVFrame {
    std::array<uint8_t, 16> synthetic_iv;   // Synthetic initialization vector (SIV)
    std::vector<uint8_t> ciphertext;        // Encrypted payload data
    std::vector<uint8_t> associated_data;   // Associated authenticated data (AAD)
    uint64_t timestamp_ns;                  // Nanosecond timestamp
    uint32_t sequence_number;               // Sequence number for replay protection
    
    // Security metadata
    uint8_t encryption_algorithm;           // Encryption algorithm identifier
    uint8_t key_version;                    // Key version for key rotation
    uint32_t security_level;                // Security level indicator
    
    bool verify_integrity() const;          // Verify frame integrity
    std::string to_debug_string() const;
};

class AESSIVCrypto {
public:
    AESSIVCrypto(const AESSIVConfig& config);
    virtual ~AESSIVCrypto();
    
    // Encryption/Decryption
    bool encrypt(const std::vector<uint8_t>& plaintext,
                const std::vector<uint8_t>& associated_data,
                AESSIVFrame& encrypted_frame);
    
    bool decrypt(const AESSIVFrame& encrypted_frame,
                std::vector<uint8_t>& plaintext,
                std::vector<uint8_t>& associated_data);
    
    // Key management
    bool derive_subkeys(const std::array<uint8_t, 32>& master_key);
    bool rotate_keys(const std::array<uint8_t, 32>& new_master_key);
    
    // Security validation
    bool validate_sequence_number(uint32_t sequence_number);
    bool check_replay_protection(uint32_t sequence_number, uint64_t timestamp_ns);
    
private:
    AESSIVConfig config_;
    std::map<uint32_t, uint64_t> sequence_timestamps_; // For replay protection
    
    // Internal AES-SIV implementation
    bool s2v_function(const std::vector<std::vector<uint8_t>>& inputs, 
                     std::array<uint8_t, 16>& output);
    bool aes_ctr_encrypt(const std::vector<uint8_t>& plaintext,
                        const std::array<uint8_t, 16>& iv,
                        std::vector<uint8_t>& ciphertext);
    bool aes_ctr_decrypt(const std::vector<uint8_t>& ciphertext,
                        const std::array<uint8_t, 16>& iv,
                        std::vector<uint8_t>& plaintext);
};

} // namespace aes_siv

// =============================
// Elliptic Curve Cryptography (ECC) Implementation
// =============================
namespace ecc {

/**
 * @brief ECC Curve Parameters per IEEE 1363a
 * Supports NIST P-256, P-384, P-521 curves
 */
enum class ECCCurve : uint8_t {
    NIST_P256 = 0x01,       // secp256r1 (prime256v1)
    NIST_P384 = 0x02,       // secp384r1
    NIST_P521 = 0x03,       // secp521r1
    BRAINPOOL_P256 = 0x04,  // brainpoolP256r1
    BRAINPOOL_P384 = 0x05,  // brainpoolP384r1
    BRAINPOOL_P512 = 0x06   // brainpoolP512r1
};

/**
 * @brief ECC Key Pair Structure
 */
struct ECCKeyPair {
    ECCCurve curve_type;                // Elliptic curve type
    std::vector<uint8_t> private_key;   // Private key (big-endian format)
    std::vector<uint8_t> public_key_x;  // Public key X coordinate
    std::vector<uint8_t> public_key_y;  // Public key Y coordinate
    uint64_t creation_time;             // Key creation timestamp
    uint64_t expiration_time;           // Key expiration timestamp
    uint32_t key_id;                    // Unique key identifier
    
    bool is_valid() const;              // Validate key pair
    size_t get_key_size_bytes() const;  // Get key size based on curve
    std::string to_debug_string() const;
};

/**
 * @brief ECDSA Signature Structure
 */
struct ECDSASignature {
    std::vector<uint8_t> r_component;   // R component of signature
    std::vector<uint8_t> s_component;   // S component of signature
    ECCCurve curve_type;                // Curve used for signature
    uint8_t hash_algorithm;             // Hash algorithm (SHA256, SHA384, SHA512)
    uint64_t signature_timestamp;       // Signature creation timestamp
    uint32_t signer_key_id;             // Signer's key identifier
    
    bool is_valid() const;              // Validate signature format
    size_t get_signature_size_bytes() const; // Get signature size based on curve
    std::string to_debug_string() const;
};

/**
 * @brief ECC Signed Control Frame for AVTP
 */
struct ECCSignedFrame {
    std::vector<uint8_t> payload_data;  // Original frame payload
    ECDSASignature signature;           // ECDSA signature
    uint64_t timestamp_ns;              // Nanosecond timestamp
    uint32_t sequence_number;           // Sequence number
    
    // Security metadata
    uint32_t security_association_id;   // Security association identifier
    uint8_t signature_algorithm;        // Signature algorithm identifier
    uint8_t trust_level;                // Trust level of signer
    
    bool verify_signature(const ECCKeyPair& public_key) const;
    std::string to_debug_string() const;
};

class ECCCryptography {
public:
    ECCCryptography(ECCCurve default_curve = ECCCurve::NIST_P256);
    virtual ~ECCCryptography();
    
    // Key management
    bool generate_key_pair(ECCCurve curve, ECCKeyPair& key_pair);
    bool import_key_pair(const std::vector<uint8_t>& private_key_der,
                        const std::vector<uint8_t>& public_key_der,
                        ECCKeyPair& key_pair);
    bool export_key_pair(const ECCKeyPair& key_pair,
                        std::vector<uint8_t>& private_key_der,
                        std::vector<uint8_t>& public_key_der);
    
    // Digital signatures
    bool sign_data(const std::vector<uint8_t>& data,
                  const ECCKeyPair& private_key,
                  ECDSASignature& signature);
    
    bool verify_signature(const std::vector<uint8_t>& data,
                         const ECDSASignature& signature,
                         const ECCKeyPair& public_key);
    
    // ECDH key exchange
    bool ecdh_key_exchange(const ECCKeyPair& local_private_key,
                          const ECCKeyPair& remote_public_key,
                          std::vector<uint8_t>& shared_secret);
    
    // Certificate support
    bool create_self_signed_certificate(const ECCKeyPair& key_pair,
                                       const std::string& subject_name,
                                       uint64_t validity_period_days,
                                       std::vector<uint8_t>& certificate);
    
    bool verify_certificate_chain(const std::vector<std::vector<uint8_t>>& certificate_chain,
                                  const ECCKeyPair& root_ca_key);
    
private:
    ECCCurve default_curve_;
    std::map<uint32_t, ECCKeyPair> trusted_keys_;
    
    // Internal ECC operations
    bool point_multiply(const std::vector<uint8_t>& scalar,
                       const std::vector<uint8_t>& point_x,
                       const std::vector<uint8_t>& point_y,
                       std::vector<uint8_t>& result_x,
                       std::vector<uint8_t>& result_y);
    
    bool hash_data(const std::vector<uint8_t>& data,
                  uint8_t hash_algorithm,
                  std::vector<uint8_t>& hash);
    
    size_t get_curve_key_size(ECCCurve curve) const;
    size_t get_curve_coordinate_size(ECCCurve curve) const;
};

} // namespace ecc

// =============================
// Key Management System
// =============================
namespace key_management {

/**
 * @brief Security Association for AVTP Streams
 */
struct SecurityAssociation {
    uint32_t sa_id;                     // Security association identifier
    uint64_t stream_id;                 // Associated stream identifier
    
    // Cryptographic parameters
    uint8_t encryption_algorithm;       // Encryption algorithm (AES-SIV, etc.)
    uint8_t signature_algorithm;        // Signature algorithm (ECDSA, etc.)
    ecc::ECCCurve curve_type;           // ECC curve for signatures
    
    // Key material
    std::array<uint8_t, 32> encryption_key;    // Encryption key
    ecc::ECCKeyPair signing_key_pair;           // Signing key pair
    std::vector<uint8_t> shared_secret;         // Shared secret for key derivation
    
    // Lifecycle management
    uint64_t creation_time;             // SA creation timestamp
    uint64_t expiration_time;           // SA expiration timestamp
    uint32_t key_rotation_interval;     // Key rotation interval (seconds)
    uint64_t last_key_rotation;         // Last key rotation timestamp
    
    // Security policies
    bool replay_protection_enabled;     // Enable replay protection
    uint32_t max_sequence_gap;          // Maximum allowed sequence number gap
    uint8_t minimum_trust_level;        // Minimum required trust level
    
    bool is_valid() const;              // Check if SA is still valid
    bool needs_key_rotation() const;    // Check if key rotation is needed
    std::string to_debug_string() const;
};

/**
 * @brief Trust Management for Public Keys
 */
struct TrustAnchor {
    uint32_t trust_anchor_id;           // Trust anchor identifier
    ecc::ECCKeyPair root_ca_key;        // Root CA public key
    std::string ca_name;                // Certificate authority name
    uint8_t trust_level;                // Trust level (0-255)
    uint64_t validity_start;            // Validity start time
    uint64_t validity_end;              // Validity end time
    
    // Certificate revocation
    std::vector<uint32_t> revoked_keys; // List of revoked key IDs
    uint64_t last_crl_update;           // Last CRL update timestamp
    
    bool is_key_revoked(uint32_t key_id) const;
    std::string to_debug_string() const;
};

class KeyManagementSystem {
public:
    KeyManagementSystem();
    virtual ~KeyManagementSystem();
    
    // Security Association management
    bool create_security_association(uint64_t stream_id,
                                   uint8_t encryption_algorithm,
                                   uint8_t signature_algorithm,
                                   SecurityAssociation& sa);
    
    bool get_security_association(uint32_t sa_id, SecurityAssociation& sa);
    bool update_security_association(const SecurityAssociation& sa);
    bool delete_security_association(uint32_t sa_id);
    
    // Key rotation
    bool rotate_encryption_key(uint32_t sa_id);
    bool rotate_signing_key(uint32_t sa_id);
    bool perform_scheduled_key_rotation();
    
    // Trust management
    bool add_trust_anchor(const TrustAnchor& trust_anchor);
    bool revoke_key(uint32_t trust_anchor_id, uint32_t key_id);
    bool is_key_trusted(uint32_t key_id, uint8_t required_trust_level);
    
    // Certificate operations
    bool import_certificate_chain(const std::vector<std::vector<uint8_t>>& certificate_chain,
                                 uint32_t& trust_anchor_id);
    bool verify_certificate_validity(uint32_t key_id);
    
    // Security event logging
    struct SecurityEvent {
        uint64_t timestamp;
        std::string event_type;
        uint32_t sa_id;
        std::string description;
        uint8_t severity_level;
    };
    
    void log_security_event(const SecurityEvent& event);
    std::vector<SecurityEvent> get_security_events(uint64_t since_timestamp) const;
    
private:
    std::map<uint32_t, SecurityAssociation> security_associations_;
    std::map<uint32_t, TrustAnchor> trust_anchors_;
    std::vector<SecurityEvent> security_log_;
    
    uint32_t next_sa_id_;
    
    // Internal helper methods
    bool derive_key_from_shared_secret(const std::vector<uint8_t>& shared_secret,
                                      const std::string& context,
                                      size_t key_length,
                                      std::vector<uint8_t>& derived_key);
    
    uint32_t generate_sa_id();
    bool cleanup_expired_sas();
};

} // namespace key_management

// =============================
// Secure AVTP Protocol Handler
// =============================

/**
 * @brief Unified Secure AVTP Protocol Handler
 * Integrates AES-SIV encryption, ECC signatures, and key management
 */
class SecureAVTPHandler {
public:
    SecureAVTPHandler();
    virtual ~SecureAVTPHandler();
    
    // Initialization
    bool initialize(const std::string& security_policy_file);
    bool configure_security_association(uint64_t stream_id,
                                      uint8_t encryption_algorithm,
                                      uint8_t signature_algorithm);
    
    // Secure frame processing
    bool encrypt_and_sign_frame(uint64_t stream_id,
                               const std::vector<uint8_t>& plaintext_payload,
                               std::vector<uint8_t>& secure_avtp_frame);
    
    bool decrypt_and_verify_frame(const std::vector<uint8_t>& secure_avtp_frame,
                                 uint64_t& stream_id,
                                 std::vector<uint8_t>& plaintext_payload);
    
    // Security policy management
    bool update_security_policy(uint64_t stream_id, const std::string& policy_parameters);
    bool enforce_security_policy(uint64_t stream_id, const std::vector<uint8_t>& frame);
    
    // Security monitoring
    struct SecurityStatistics {
        uint64_t frames_encrypted;
        uint64_t frames_decrypted;
        uint64_t signature_verifications_passed;
        uint64_t signature_verifications_failed;
        uint64_t replay_attacks_detected;
        uint64_t authentication_failures;
        uint64_t key_rotations_performed;
        double average_encryption_time_us;
        double average_decryption_time_us;
    };
    
    SecurityStatistics get_security_statistics() const;
    bool reset_security_statistics();
    
    // Event handling
    using SecurityEventCallback = std::function<void(const key_management::KeyManagementSystem::SecurityEvent&)>;
    void set_security_event_callback(SecurityEventCallback callback);
    
private:
    std::unique_ptr<key_management::KeyManagementSystem> key_manager_;
    std::unique_ptr<aes_siv::AESSIVCrypto> aes_siv_crypto_;
    std::unique_ptr<ecc::ECCCryptography> ecc_crypto_;
    
    SecurityStatistics statistics_;
    SecurityEventCallback security_event_callback_;
    
    // Internal processing
    bool process_encrypted_frame(const std::vector<uint8_t>& frame,
                               std::vector<uint8_t>& processed_frame);
    bool process_signed_frame(const std::vector<uint8_t>& frame,
                            std::vector<uint8_t>& processed_frame);
    
    bool validate_frame_security_headers(const std::vector<uint8_t>& frame);
    bool extract_security_metadata(const std::vector<uint8_t>& frame,
                                  uint32_t& sa_id,
                                  uint8_t& algorithm_type);
    
    void update_statistics(const std::string& operation, bool success, double time_us);
};

} // namespace security
} // namespace avtp_protocol

#endif // SECURITY_CRYPTOGRAPHY_H
