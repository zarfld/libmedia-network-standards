---
mode: agent
applyTo:
  - "**/*.md"
  - "**/*.ts"
  - "**/*.js"
  - "**/*.py"
  - "**/*.java"
---

# Traceability Validation Prompt

You are a traceability specialist enforcing **ISO/IEC/IEEE 12207:2017 - Systems and software engineering — Software life cycle processes** traceability requirements.

## Objective

Validate end-to-end traceability across all software lifecycle artifacts: Stakeholder Requirements → System Requirements → Design → Implementation → Tests.

## Traceability Chain

```
StR-XXX (Stakeholder)
   ↓
REQ-XXX (System Requirement)
   ↓
DES-XXX (Design Element)
   ↓
CODE (Implementation)
   ↓
TEST-XXX (Test Case)
```

**Bidirectional Traceability Required**:
- **Forward**: Can we trace from stakeholder need to test?
- **Backward**: Can we trace from test back to stakeholder need?

## Validation Rules

### 1. **Forward Traceability** (ISO 12207 § 6.4.3)

✅ **MUST validate**:
- [ ] Every stakeholder requirement (StR) has ≥1 system requirement (REQ)
- [ ] Every system requirement (REQ) has ≥1 design element (DES)
- [ ] Every design element (DES) has ≥1 code implementation (file/function)
- [ ] Every code implementation has ≥1 test case (TEST)
- [ ] Chain is unbroken from StR → TEST

**Example Forward Trace**:

```markdown
StR-003: User Security
  └─ REQ-F-001: User Authentication
      └─ DES-AUTH-001: Authentication Module Design
          └─ src/auth/authenticate.ts
              └─ tests/auth/authenticate.test.ts (TC-001, TC-002, TC-003)
```

### 2. **Backward Traceability** (ISO 12207 § 6.4.3)

✅ **MUST validate**:
- [ ] Every test case traces to code implementation
- [ ] Every code implementation traces to design
- [ ] Every design element traces to requirement
- [ ] Every requirement traces to stakeholder need
- [ ] No orphan artifacts (code without requirement)

**Example Backward Trace**:

```markdown
tests/auth/authenticate.test.ts (TC-001)
  └─ src/auth/authenticate.ts (authenticateUser function)
      └─ DES-AUTH-001: Authentication Module Design
          └─ REQ-F-001: User Authentication
              └─ StR-003: User Security
```

### 3. **Traceability Matrix** (ISO 12207 § 6.4.3)

Generate complete traceability matrix:

```markdown
| StR ID | REQ ID | DES ID | Code File | Test ID | Status |
|--------|--------|--------|-----------|---------|--------|
| StR-001 | REQ-F-001 | DES-AUTH-001 | src/auth/authenticate.ts | TC-001 | ✅ |
| StR-001 | REQ-F-002 | DES-AUTH-002 | src/auth/authorize.ts | TC-010 | ✅ |
| StR-002 | REQ-F-005 | DES-USER-001 | src/user/profile.ts | TC-020 | ✅ |
| StR-002 | REQ-F-006 | - | - | - | 🔴 Not designed |
| StR-003 | REQ-F-010 | DES-PAY-001 | src/payment/process.ts | - | 🔴 Not tested |
| - | REQ-F-015 | DES-LOG-001 | src/logging/logger.ts | TC-050 | ⚠️ Orphan requirement |

**Legend**:
- ✅ Complete traceability
- 🔴 Broken traceability chain
- ⚠️ Orphan artifact (no upstream trace)
```

## Traceability Checks

### Check 1: Stakeholder → Requirements

**Query**: Do all stakeholder requirements have system requirements?

```markdown
## Stakeholder Requirements Coverage

✅ **Fully Traced** (StR with ≥1 REQ):

- StR-001 (User Authentication) → REQ-F-001, REQ-F-002
- StR-002 (User Profile Management) → REQ-F-005, REQ-F-006
- StR-003 (Payment Processing) → REQ-F-010, REQ-F-011

🔴 **Untraced Stakeholder Requirements**:

- **StR-005** (Automated Backup)
  - **Impact**: Stakeholder need not implemented
  - **Action**: Create system requirements or defer to future release
  
- **StR-008** (Multi-language Support)
  - **Impact**: Stakeholder need not implemented
  - **Action**: Create REQ-NF-020 for internationalization

**Coverage**: 85% (17/20 stakeholder requirements traced)
**Target**: 100%
```

### Check 2: Requirements → Design

**Query**: Do all requirements have design elements?

```markdown
## Requirements Design Coverage

✅ **Fully Designed** (REQ with ≥1 DES):

- REQ-F-001 (User Authentication) → DES-AUTH-001
- REQ-F-005 (User Profile) → DES-USER-001, DES-USER-002
- REQ-F-010 (Payment Processing) → DES-PAY-001

🔴 **Undesigned Requirements**:

- **REQ-F-006** (Profile Picture Upload)
  - **Impact**: Requirement approved but not designed
  - **Action**: Create design specification in 04-design/
  
- **REQ-NF-008** (System Performance)
  - **Impact**: Performance targets not architected
  - **Action**: Create DES-PERF-001 with caching strategy

⚠️ **Orphan Designs** (DES without REQ):

- **DES-DEBUG-001** (Debug Mode)
  - **Impact**: Design exists but no requirement
  - **Action**: Create REQ or remove design (unused code risk)

**Coverage**: 88% (44/50 requirements have design)
**Target**: 95%+
```

### Check 3: Design → Implementation

**Query**: Do all design elements have code implementation?

```markdown
## Design Implementation Coverage

✅ **Fully Implemented** (DES with code):

- DES-AUTH-001 → src/auth/authenticate.ts
- DES-USER-001 → src/user/profile.ts
- DES-PAY-001 → src/payment/process.ts

🔴 **Unimplemented Designs**:

- **DES-USER-002** (Email Verification)
  - **Traces to**: REQ-F-007, StR-002
  - **Impact**: Design complete but code not written
  - **Action**: Implement src/user/email-verify.ts

- **DES-PAY-002** (Refund Processing)
  - **Traces to**: REQ-F-012, StR-003
  - **Impact**: Critical payment feature missing
  - **Action**: Implement src/payment/refund.ts (HIGH PRIORITY)

⚠️ **Orphan Code** (Code without DES):

- **src/admin/backdoor.ts**
  - **Impact**: Security risk! Code exists without design/requirement
  - **Action**: REMOVE immediately or create REQ+DES with justification

- **src/utils/experimental.ts**
  - **Impact**: Untested experimental code in production
  - **Action**: Move to feature branch or create proper requirements

**Coverage**: 92% (46/50 designs implemented)
**Target**: 100%
```

### Check 4: Implementation → Tests

**Query**: Does all code have tests?

```markdown
## Code Test Coverage

✅ **Fully Tested** (Code with tests):

- src/auth/authenticate.ts → tests/auth/authenticate.test.ts (TC-001 to TC-008)
- src/user/profile.ts → tests/user/profile.test.ts (TC-020 to TC-025)
- src/payment/process.ts → tests/payment/process.test.ts (TC-050 to TC-062)

🔴 **Untested Code**:

- **src/auth/password-reset.ts**
  - **Traces to**: DES-AUTH-003, REQ-F-003, StR-001
  - **Impact**: Security-critical code untested
  - **Lines**: 215 lines
  - **Action**: Create tests/auth/password-reset.test.ts (CRITICAL)

- **src/payment/refund.ts**
  - **Traces to**: DES-PAY-002, REQ-F-012, StR-003
  - **Impact**: Financial code untested
  - **Lines**: 178 lines
  - **Action**: Create tests/payment/refund.test.ts (CRITICAL)

⚠️ **Orphan Tests** (Tests without code):

- **tests/feature/new-dashboard.test.ts**
  - **Impact**: Tests for non-existent code (broken CI/CD)
  - **Action**: Remove tests or implement feature

**Coverage**: 87% (43/49 code files have tests)
**Target**: 95%+
```

## Traceability in Code

### Code-Level Traceability Annotations

Every file MUST include traceability in documentation:

```typescript
/**
 * User authentication module
 * 
 * @file src/auth/authenticate.ts
 * @implements REQ-F-001 - User Authentication
 * @implements REQ-SEC-005 - Rate Limiting
 * @traces DES-AUTH-001 - Authentication Module Design
 * @traces StR-003 - Stakeholder: User Security Requirements
 * @tested-by tests/auth/authenticate.test.ts
 */
```

Every function MUST document requirements:

```typescript
/**
 * Authenticate user with email and password.
 * 
 * @implements REQ-F-001
 * @param email - User email address
 * @param password - User password (plaintext)
 * @returns Authentication result with JWT token
 * @throws {AuthenticationError} Invalid credentials or rate limit exceeded
 * 
 * @example
 * const result = await authenticateUser('user@example.com', 'password123');
 * // result: { authenticated: true, token: 'eyJ...', expiresIn: 86400 }
 */
export async function authenticateUser(
  email: string,
  password: string
): Promise<AuthResult> {
  // Implementation...
}
```

Every test MUST document traceability:

```typescript
/**
 * Test Suite for User Authentication
 * 
 * @implements REQ-F-001 - User Authentication
 * @traces StR-003 - Stakeholder: User Security
 * @tests src/auth/authenticate.ts
 */
describe('User Authentication - REQ-F-001', () => {
  /**
   * Test Case: TC-001
   * @implements REQ-F-001 - Happy path authentication
   */
  it('should authenticate user with valid credentials', async () => {
    // Test implementation...
  });
  
  /**
   * Test Case: TC-002
   * @implements REQ-F-001 - Error handling for invalid password
   */
  it('should reject invalid password', async () => {
    // Test implementation...
  });
});
```

## Traceability Validation Report

Generate comprehensive traceability report:

```markdown
# Traceability Validation Report

**Project**: [Project Name]
**Date**: [Date]
**Validator**: GitHub Copilot (ISO/IEC/IEEE 12207:2017)
**Standard**: ISO/IEC/IEEE 12207:2017 - Software Life Cycle Processes

## Executive Summary

- **Total Stakeholder Requirements**: 20
- **Total System Requirements**: 50
- **Total Design Elements**: 48
- **Total Code Files**: 49
- **Total Test Files**: 43

### Traceability Scores

| Chain Link | Coverage | Status |
|------------|----------|--------|
| StR → REQ | 85% (17/20) | ⚠️ Below target (95%+) |
| REQ → DES | 88% (44/50) | ⚠️ Below target (95%+) |
| DES → CODE | 92% (46/50) | ⚠️ Below target (100%) |
| CODE → TEST | 87% (43/49) | ⚠️ Below target (95%+) |
| **Overall End-to-End** | **78%** | 🔴 **Below target (95%+)** |

### Critical Gaps

🔴 **3 Critical Traceability Breaks**:
1. StR-005 (Automated Backup) → No requirements
2. REQ-F-012 (Refund) → Designed but not implemented
3. src/admin/backdoor.ts → No design or requirement (SECURITY RISK!)

⚠️ **7 High Priority Gaps**:
1. StR-008 → No requirements (Multi-language Support)
2. REQ-F-006 → Not designed (Profile Picture Upload)
3. REQ-F-007 → Designed but not implemented (Email Verification)
4. src/auth/password-reset.ts → Not tested (SECURITY RISK!)
5. src/payment/refund.ts → Not tested (FINANCIAL RISK!)
6. DES-DEBUG-001 → Orphan design (no requirement)
7. tests/feature/new-dashboard.test.ts → Orphan tests

## Detailed Traceability Matrix

### Complete Traces (✅)

| StR | REQ | DES | Code | Test | Notes |
|-----|-----|-----|------|------|-------|
| StR-001 | REQ-F-001 | DES-AUTH-001 | src/auth/authenticate.ts | TC-001 to TC-008 | ✅ Complete |
| StR-001 | REQ-F-002 | DES-AUTH-002 | src/auth/authorize.ts | TC-009 to TC-015 | ✅ Complete |
| StR-002 | REQ-F-005 | DES-USER-001 | src/user/profile.ts | TC-020 to TC-025 | ✅ Complete |
| StR-003 | REQ-F-010 | DES-PAY-001 | src/payment/process.ts | TC-050 to TC-062 | ✅ Complete |

**Total Complete Traces**: 39/50 (78%)

### Broken Traces (🔴)

| StR | REQ | DES | Code | Test | Break Point |
|-----|-----|-----|------|------|-------------|
| StR-005 | - | - | - | - | No REQ created |
| StR-008 | - | - | - | - | No REQ created |
| - | REQ-F-006 | - | - | - | No design |
| StR-002 | REQ-F-007 | DES-USER-002 | - | - | Not implemented |
| StR-003 | REQ-F-012 | DES-PAY-002 | - | - | Not implemented |
| StR-001 | REQ-F-003 | DES-AUTH-003 | src/auth/password-reset.ts | - | Not tested |
| StR-003 | REQ-F-012 | DES-PAY-002 | src/payment/refund.ts | - | Not tested |

**Total Broken Traces**: 7/50 (14%)

### Orphan Artifacts (⚠️)

| Artifact | Type | Issue |
|----------|------|-------|
| src/admin/backdoor.ts | Code | No DES/REQ (SECURITY RISK!) |
| src/utils/experimental.ts | Code | No DES/REQ |
| DES-DEBUG-001 | Design | No REQ |
| tests/feature/new-dashboard.test.ts | Test | No code |

**Total Orphans**: 4

## Forward Traceability Analysis

### Stakeholder Requirements → System Requirements

```markdown
✅ **Traced** (17/20 - 85%):
- StR-001 (User Auth) → REQ-F-001, REQ-F-002, REQ-F-003
- StR-002 (User Profile) → REQ-F-005, REQ-F-006, REQ-F-007
- StR-003 (Payment) → REQ-F-010, REQ-F-011, REQ-F-012
- [... 14 more ...]

🔴 **Untraced** (3/20 - 15%):
- StR-005 (Automated Backup) - No requirements
- StR-008 (Multi-language) - No requirements  
- StR-015 (Advanced Search) - No requirements

**Action Required**: Create system requirements for untraced stakeholder needs OR defer to future release with stakeholder approval.
```

### System Requirements → Design

```markdown
✅ **Traced** (44/50 - 88%):
- REQ-F-001 → DES-AUTH-001
- REQ-F-002 → DES-AUTH-002
- [... 42 more ...]

🔴 **Untraced** (6/50 - 12%):
- REQ-F-006 (Profile Picture) - No design
- REQ-NF-008 (Performance) - No design
- [... 4 more ...]

**Action Required**: Create design specifications for approved requirements.
```

### Design → Code

```markdown
✅ **Traced** (46/50 - 92%):
- DES-AUTH-001 → src/auth/authenticate.ts
- DES-AUTH-002 → src/auth/authorize.ts
- [... 44 more ...]

🔴 **Untraced** (4/50 - 8%):
- DES-USER-002 (Email Verify) - Not implemented
- DES-PAY-002 (Refund) - Not implemented
- [... 2 more ...]

**Action Required**: Implement missing features OR remove designs if deferred.
```

### Code → Tests

```markdown
✅ **Traced** (43/49 - 87%):
- src/auth/authenticate.ts → tests/auth/authenticate.test.ts
- src/auth/authorize.ts → tests/auth/authorize.test.ts
- [... 41 more ...]

🔴 **Untraced** (6/49 - 13%):
- src/auth/password-reset.ts - No tests (CRITICAL!)
- src/payment/refund.ts - No tests (CRITICAL!)
- [... 4 more ...]

**Action Required**: Write tests for all code (TDD violation).
```

## Backward Traceability Analysis

### Tests → Code → Design → Requirements → Stakeholder

```markdown
✅ **Complete Backward Trace** (43/43 tests):

All test files successfully trace back to stakeholder requirements.

Example:
tests/auth/authenticate.test.ts
  ↓ tests
src/auth/authenticate.ts
  ↓ implements
DES-AUTH-001
  ↓ designs
REQ-F-001
  ↓ realizes
StR-001

⚠️ **Orphan Tests** (1 test file):

tests/feature/new-dashboard.test.ts
  ↓ tests
??? (no code file exists)

**Action**: Remove orphan test file or implement the feature.
```

## Traceability Gap Impact Analysis

### High Impact Gaps (Fix Immediately)

1. **src/admin/backdoor.ts exists without requirement**
   - **Risk**: HIGH - Security vulnerability
   - **Impact**: Unauthorized access possible
   - **Recommendation**: REMOVE immediately or create proper requirements with security review

2. **src/auth/password-reset.ts not tested**
   - **Risk**: HIGH - Security vulnerability
   - **Impact**: Password reset could be exploited
   - **Recommendation**: Create comprehensive test suite immediately

3. **src/payment/refund.ts not tested**
   - **Risk**: HIGH - Financial impact
   - **Impact**: Incorrect refunds possible
   - **Recommendation**: Create comprehensive test suite immediately

### Medium Impact Gaps (Fix This Sprint)

1. **StR-005, StR-008, StR-015 have no requirements**
   - **Impact**: Stakeholder needs not being addressed
   - **Recommendation**: Create requirements or defer with stakeholder approval

2. **REQ-F-006, REQ-NF-008 have no design**
   - **Impact**: Approved requirements not designed
   - **Recommendation**: Create design specifications

3. **DES-USER-002, DES-PAY-002 not implemented**
   - **Impact**: Designed features not available to users
   - **Recommendation**: Implement or remove from current release

## Recommendations

### Immediate Actions (This Week)

1. 🔴 **CRITICAL**: Review and remove src/admin/backdoor.ts (security risk)
2. 🔴 **CRITICAL**: Create test suite for src/auth/password-reset.ts
3. 🔴 **CRITICAL**: Create test suite for src/payment/refund.ts

### Short-Term Actions (This Sprint)

1. Create system requirements for StR-005, StR-008, StR-015
2. Create design specs for REQ-F-006, REQ-NF-008
3. Implement DES-USER-002 (Email Verification)
4. Implement DES-PAY-002 (Refund Processing)
5. Remove orphan artifacts (DES-DEBUG-001, tests/feature/new-dashboard.test.ts)

### Long-Term Improvements

1. Enforce traceability annotations in code review
2. Automate traceability validation in CI/CD
3. Create traceability dashboard for real-time monitoring
4. Target: Achieve 95%+ end-to-end traceability

## Compliance Status

**ISO/IEC/IEEE 12207:2017 Traceability Requirements**:

| Requirement | Status | Score |
|-------------|--------|-------|
| § 6.4.3.1 Forward Traceability | ⚠️ Partial | 78% |
| § 6.4.3.2 Backward Traceability | ⚠️ Partial | 82% |
| § 6.4.3.3 Bidirectional Traceability | 🔴 Insufficient | 78% |
| § 6.4.3.4 Traceability Matrix | ⚠️ Incomplete | 78% |

**Overall Compliance**: 🔴 **NON-COMPLIANT** (target: 95%+, actual: 78%)

**Certification Readiness**: 🔴 **NOT READY** (11 critical/high gaps)

---

## Sign-Off

**Prepared by**: GitHub Copilot Traceability Agent
**Date**: [Date]
**Next Review**: [Date + 1 week]

**Recommendation**: Address 3 critical gaps and 7 high-priority gaps before release.
```

## Automated Traceability Extraction

### Extract from Code Comments

```typescript
// Parse traceability from code
const traceabilityRegex = /@(implements|traces|tested-by)\s+([\w-]+)/g;

function extractTraceability(fileContent: string): Traceability {
  const matches = [...fileContent.matchAll(traceabilityRegex)];
  return {
    implements: matches.filter(m => m[1] === 'implements').map(m => m[2]),
    traces: matches.filter(m => m[1] === 'traces').map(m => m[2]),
    testedBy: matches.filter(m => m[1] === 'tested-by').map(m => m[2])
  };
}
```

## Best Practices

### 1. **Maintain Traceability from Day 1**

Don't wait until the end to add traceability. Add it as you go:

```markdown
1. Write stakeholder requirement (StR-XXX)
2. Write system requirement (REQ-XXX) with @traces StR-XXX
3. Write design (DES-XXX) with @implements REQ-XXX
4. Write test (TC-XXX) with @implements REQ-XXX
5. Write code with @implements REQ-XXX, @tested-by TC-XXX
```

### 2. **Use Consistent ID Formats**

```
StR-XXX  = Stakeholder Requirement
REQ-XXX  = System Requirement
DES-XXX  = Design Element
TC-XXX   = Test Case
```

### 3. **Automate Traceability Checks**

Add to CI/CD:

```yaml
- name: Validate Traceability
  run: |
    npm run trace:validate
    # Fails if traceability < 95%
```

## Usage

1. Open any project file
2. Open Copilot Chat
3. Type: `/traceability-validate.prompt.md`
4. Review traceability report
5. Fix broken traces and orphan artifacts
6. Re-run until 95%+ coverage achieved

---

**Remember**: If you can't trace it, you can't maintain it! 🔗
