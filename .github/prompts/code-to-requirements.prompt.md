---
mode: agent
applyTo:
  - "**/src/**/*.js"
  - "**/src/**/*.ts"
  - "**/src/**/*.py"
  - "**/src/**/*.java"
  - "**/src/**/*.go"
  - "**/src/**/*.cs"
  - "**/lib/**/*"
  - "**/app/**/*"
  - "**/tests/**/*"
  - "**/test/**/*"
---

# Code to Requirements Reverse Engineering Prompt

You are a **Requirements Engineer** and **Static Code Analysis Expert** following **ISO/IEC/IEEE 29148:2018**.

## 🎯 Objective

Reverse engineer formal requirements specifications from existing code:
1. **Analyze code behavior** to infer functional requirements
1.1 **Review existing documentation** in order to understand context
1.2 **Identify Copilot-generated artifacts** containing implementation rationale
1.3 **Cross-validate Copilot intentions** against current implementation
1.4 **Review Standards which were base for implementation** use MarkItDown MCP server
2. **Extract non-functional requirements** from implementation patterns
3. **Generate acceptance criteria** from test cases
4. **Create traceability links** between code and requirements
5. **Produce ISO 29148 compliant** requirements specification

## Known Standards which were base for implementation
- in folder D:\SyncDrive\SynologyDrive\MCP\Standards\IEEE\
  - IEEE 1588-2019-en.pdf
  - ISO-IEC-IEEE 8802-1BA-2016-en.pdf
  - ISO-IEC-IEEE 8802-1Q-2020-en.pdf
  - ISO-IEC-IEEE 8802-1AS-2021-en.pdf
  - IEEE 1722.1-2021-en.pdf
  - IEEE 1722.1-2013-en.pdf
  - IEEE 1722-2016-en.pdf
  - 8021AS-2020.pdf
  - IEEE 1733-2011-en.pdf
  - 8021AX-2020.pdf
  - 8021Q-2022.pdf
  - 802-2014.pdf
  - 8021AB-2016.pdf
  - 8021AS-2020.pdf
  - 8021Q-2018.pdf
  - 8021Q-2022.pdf
  - 8021AX-2020.pdf
  - 8021Q-2022.pdf
  - 8021X-2020.pdf
  - 8023-2022.pdf
  - 29148-2018-ISOIECIEEE.pdf
  - IEEE 1588-2019-en.pdf
  - IEEE 1722-2016-en.pdf
  - IEEE 1722.1-2021-en.pdf
  - ieee-1016-2009.pdf
  - iso12207-2017-pdf-free.pdf
  - iso-iec-ieee-42010-2011.pdf
- in folder "D:\SyncDrive\SynologyDrive\MCP\Standards\AES"
  - AES67-2018.pdf
  - AES-70-1-2018-en.pdf
  - AES-70-2-2018-en.pdf
  - AES-70-3-2018-en.pdf
  - AES-70-3-2015-en.pdf
  - AES-X192-2020.pdf
  - AES67-2013.pdf
  - AES70-2015.pdf
  - AES-X192-2017.pdf
  - AES 11-2009 (R2014)-en.pdf
  - AES 5-2018-en.pdf
  - AES 3-1-2009 (R2014)-en.pdf
  - AES 3-2-2009 (R2014)-en.pdf
  - AES 3-3-2009 (R2014)-en.pdf
  - AES 3-4-2009 (R2014)-en.pdf
in folder "D:\SyncDrive\SynologyDrive\MCP\Standards\AVnu"
 - Auto-Ethernet-AVB-Func-Interop-Spec_v1.6.pdf
 - Automotive-Ethernet-AVB-Func-Interop-Spec-v1.5-Public.pdf
 - Avnu_Base-and-ProAV_Functional_Interop_Specification_1.1.pdf
 - AVnu_SWAPIs-v1.3-SWAVI-Updates_Final_Approved_Clean.pdf
 - Avnu-ProAV-Bridge-Specification_v1.0.pdf
 - Milan_Specification_Consolidated_v1.2_Final_Approved-20231130.pdf
 - Milan-Baseline-Interoperability-Specification-2.0a.pdf




## � AI Agent Guardrails
**CRITICAL**: Prevent common AI pitfalls during requirements recovery:
- ❌ **No implementation-based assumptions**: Never assume current code represents correct requirements
- ❌ **Prefer incremental modification during maintenance**: Extend existing patterns unless discrepancies detected
- ✅ **Validate legacy assumptions**: Always question existing business rules until verified
- ✅ **Reimplementation legitimate when**: Discrepancies/bugs detected after requirements validation
- ✅ **Follow proper validation path**: Requirements → Architecture → Implementation decision

**Validation Questions**:
1. Have I validated legacy assumptions against business requirements?
2. Am I extending existing patterns appropriately during maintenance?
3. Have I identified when reimplementation is legitimately needed?

## �🔍 Code Analysis Framework

### Step 1: Code Structure Analysis

**Identify system components**:

#### **Frontend Analysis**
```bash
# Identify UI components and pages
find src/ -name "*.jsx" -o -name "*.tsx" -o -name "*.vue" -o -name "*.html"

# Extract user-facing features
grep -r "onClick\|onSubmit\|useEffect\|useState" src/components/
```

#### **Backend Analysis** 
```bash
# Identify API endpoints
grep -r "app.get\|app.post\|@GetMapping\|@PostMapping\|def.*get\|def.*post" src/

# Extract business logic
find src/ -name "*service*" -o -name "*controller*" -o -name "*handler*"
```

#### **Database Analysis**
```bash
# Find data models and schemas
find src/ -name "*model*" -o -name "*schema*" -o -name "*.sql"

# Extract data operations
grep -r "SELECT\|INSERT\|UPDATE\|DELETE\|CREATE TABLE" src/
```

### Step 2: Functional Requirements Extraction

**Extract requirements from code patterns**:

#### **API Endpoints → Functional Requirements**

**Code Pattern**:
```javascript
// Express.js example
app.post('/api/users', async (req, res) => {
  const { email, password, name } = req.body;
  
  // Validation
  if (!email || !isValidEmail(email)) {
    return res.status(400).json({ error: 'Valid email required' });
  }
  if (!password || password.length < 8) {
    return res.status(400).json({ error: 'Password must be at least 8 characters' });
  }
  
  // Business logic
  const existingUser = await User.findByEmail(email);
  if (existingUser) {
    return res.status(409).json({ error: 'User already exists' });
  }
  
  const hashedPassword = await bcrypt.hash(password, 12);
  const user = await User.create({
    email,
    password: hashedPassword,
    name,
    createdAt: new Date()
  });
  
  res.status(201).json({ id: user.id, email: user.email, name: user.name });
});
```

**Generated Requirement**:
```markdown
## REQ-F-USER-001: User Registration

**ID**: REQ-F-USER-001
**Priority**: High (P1)
**Source**: Derived from POST /api/users endpoint implementation
**Status**: Implemented

### Description
The system shall allow new users to register by providing email, password, and name.

### Functional Requirements

#### REQ-F-USER-001.1: Registration Data Collection
The system shall accept user registration requests with:
- Email address (required, must be valid email format)
- Password (required, minimum 8 characters)
- Full name (required)

#### REQ-F-USER-001.2: Email Uniqueness Validation
The system shall ensure email addresses are unique across all users.

#### REQ-F-USER-001.3: Password Security
The system shall hash passwords using bcrypt with cost factor 12 before storage.

#### REQ-F-USER-001.4: User Account Creation
The system shall create user account with generated unique ID and timestamp.

### Acceptance Criteria

```gherkin
Scenario: Successful user registration
  Given no user exists with email "john@example.com"
  When user submits registration with:
    | email | john@example.com |
    | password | SecurePass123 |
    | name | John Smith |
  Then user account is created successfully
  And response contains user ID, email, and name
  And password is hashed with bcrypt
  And HTTP status is 201 Created

Scenario: Registration with existing email
  Given user exists with email "john@example.com"
  When user submits registration with email "john@example.com"
  Then registration fails with "User already exists"
  And HTTP status is 409 Conflict
  And no duplicate user is created

Scenario: Registration with invalid email
  Given user submits registration with email "invalid-email"
  Then registration fails with "Valid email required"
  And HTTP status is 400 Bad Request

Scenario: Registration with weak password
  Given user submits registration with password "123"
  Then registration fails with "Password must be at least 8 characters"
  And HTTP status is 400 Bad Request
```

### Traceability
- **Implemented in**: src/controllers/userController.js (POST /api/users)
- **Tests**: tests/user.registration.test.js
- **Database**: users table (email, password_hash, name, created_at)
```

#### **Business Logic → Requirements**

**Code Pattern**:
```python
# Python example - Order processing
def process_order(order_data):
    order = Order(**order_data)
    
    # Inventory check
    for item in order.items:
        if not inventory_service.is_available(item.product_id, item.quantity):
            raise InsufficientInventoryError(f"Not enough {item.product_id}")
    
    # Calculate pricing
    subtotal = sum(item.price * item.quantity for item in order.items)
    tax = calculate_tax(subtotal, order.shipping_address.state)
    shipping = calculate_shipping(order.weight, order.shipping_address)
    total = subtotal + tax + shipping
    
    # Apply discount
    if order.discount_code:
        discount = apply_discount(order.discount_code, subtotal)
        total = total - discount
    
    # Process payment
    payment_result = payment_service.charge(order.payment_method, total)
    if not payment_result.success:
        raise PaymentFailedError(payment_result.error_message)
    
    # Update inventory
    for item in order.items:
        inventory_service.reserve(item.product_id, item.quantity)
    
    # Create order record
    order.status = 'confirmed'
    order.total_amount = total
    order.payment_id = payment_result.transaction_id
    order.save()
    
    # Send confirmation
    email_service.send_order_confirmation(order)
    
    return order
```

**Generated Requirements**:
```markdown
## REQ-F-ORDER-002: Order Processing

**ID**: REQ-F-ORDER-002
**Priority**: Critical (P0)
**Source**: Derived from process_order() function implementation

### Description
The system shall process customer orders including inventory validation, pricing calculation, payment processing, and order confirmation.

### Sub-Requirements

#### REQ-F-ORDER-002.1: Inventory Validation
The system shall verify sufficient inventory exists for all ordered items before processing.

#### REQ-F-ORDER-002.2: Price Calculation
The system shall calculate order total including:
- Item subtotal (price × quantity for each item)
- Tax calculation based on shipping address
- Shipping cost based on weight and destination
- Discount application (if valid discount code provided)

#### REQ-F-ORDER-002.3: Payment Processing
The system shall process payment using provided payment method and handle payment failures gracefully.

#### REQ-F-ORDER-002.4: Inventory Reservation
The system shall reserve inventory for confirmed orders to prevent overselling.

#### REQ-F-ORDER-002.5: Order Confirmation
The system shall create order record with confirmed status and send confirmation email to customer.

### Error Handling

#### REQ-F-ORDER-002.E1: Insufficient Inventory
When inventory is insufficient, system shall return error "Not enough [product_id]" without processing order.

#### REQ-F-ORDER-002.E2: Payment Failure
When payment fails, system shall return payment error message without updating inventory or creating order.

### Acceptance Criteria

```gherkin
Scenario: Successful order processing
  Given inventory has sufficient quantities for all items
  And customer has valid payment method
  When order is submitted for processing
  Then inventory is validated for all items
  And total price is calculated (subtotal + tax + shipping - discount)
  And payment is processed successfully
  And inventory is reserved for ordered items
  And order record is created with 'confirmed' status
  And confirmation email is sent to customer

Scenario: Order with insufficient inventory
  Given inventory has insufficient quantity for item "PROD-123"
  When order containing "PROD-123" is processed
  Then system throws InsufficientInventoryError
  And error message includes "Not enough PROD-123"
  And no payment is processed
  And no inventory is reserved

Scenario: Order with payment failure
  Given order has valid inventory
  But payment processing fails with "Card declined"
  When order is processed
  Then system throws PaymentFailedError
  And error message is "Card declined"
  And no inventory is reserved
  And no order record is created
```
```

### Step 3: Non-Functional Requirements Extraction

**Extract NFRs from code patterns**:

#### **Performance Requirements**

**Code Patterns to Analyze**:
```javascript
// Caching patterns
const cache = new Redis();
app.get('/api/products', async (req, res) => {
  const cacheKey = `products:${req.query.page}:${req.query.limit}`;
  let products = await cache.get(cacheKey);
  
  if (!products) {
    products = await Product.findAll({
      limit: req.query.limit || 20,
      offset: (req.query.page - 1) * (req.query.limit || 20)
    });
    await cache.setex(cacheKey, 300, JSON.stringify(products)); // 5 min cache
  }
  
  res.json(products);
});

// Database indexing
CREATE INDEX idx_products_category ON products(category_id);
CREATE INDEX idx_orders_user_date ON orders(user_id, created_at);

// Pagination limits
const MAX_PAGE_SIZE = 100;
const DEFAULT_PAGE_SIZE = 20;
```

**Generated NFR**:
```markdown
## REQ-NF-PERF-001: Product Listing Performance

**ID**: REQ-NF-PERF-001
**Priority**: High (P1)
**Source**: Derived from caching and pagination implementation

### Description
The system shall provide fast product listing with caching and pagination for optimal performance.

### Performance Requirements

#### REQ-NF-PERF-001.1: Response Time
Product listing API shall respond within 200ms for 95% of requests under normal load.

#### REQ-NF-PERF-001.2: Caching Strategy
System shall cache product listing results for 5 minutes to reduce database load.

#### REQ-NF-PERF-001.3: Pagination Limits
System shall support pagination with:
- Default page size: 20 items
- Maximum page size: 100 items
- Efficient offset-based pagination

#### REQ-NF-PERF-001.4: Database Optimization
System shall use database indexes on frequently queried columns (category_id, created_at).

### Acceptance Criteria

```gherkin
Scenario: Fast product listing response
  Given system has cached product data
  When client requests product listing
  Then response is returned within 200ms
  And response contains requested page of products
  And cache-control headers indicate 5-minute cache

Scenario: Efficient pagination
  Given client requests page 5 with 50 items per page
  When system processes request
  Then database query uses LIMIT 50 OFFSET 200
  And response includes pagination metadata
  And database indexes are utilized for performance
```
```

#### **Security Requirements**

**Code Patterns**:
```javascript
// Authentication middleware
const jwt = require('jsonwebtoken');
const authenticate = (req, res, next) => {
  const token = req.headers.authorization?.split(' ')[1];
  if (!token) {
    return res.status(401).json({ error: 'Authentication required' });
  }
  
  try {
    const decoded = jwt.verify(token, process.env.JWT_SECRET);
    req.user = decoded;
    next();
  } catch (error) {
    return res.status(401).json({ error: 'Invalid token' });
  }
};

// Input validation
const validateUserInput = (req, res, next) => {
  const { email, password } = req.body;
  
  // SQL injection prevention
  if (typeof email !== 'string' || typeof password !== 'string') {
    return res.status(400).json({ error: 'Invalid input type' });
  }
  
  // XSS prevention
  req.body.email = xss(email);
  req.body.name = xss(req.body.name);
  
  next();
};

// Rate limiting
const rateLimit = require('express-rate-limit');
const loginLimiter = rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 5, // 5 attempts per window
  message: 'Too many login attempts, please try again later'
});
```

**Generated NFR**:
```markdown
## REQ-NF-SEC-001: Authentication and Authorization

**ID**: REQ-NF-SEC-001
**Priority**: Critical (P0)
**Source**: Derived from authentication middleware implementation

### Description
The system shall implement secure authentication and authorization using JWT tokens.

### Security Requirements

#### REQ-NF-SEC-001.1: JWT Authentication
System shall use JWT tokens for user authentication with:
- Token-based stateless authentication
- Configurable token expiration
- Secure token verification

#### REQ-NF-SEC-001.2: Input Validation and Sanitization
System shall validate and sanitize all user inputs to prevent:
- SQL injection attacks (type checking)
- XSS attacks (input sanitization)
- Invalid data type submissions

#### REQ-NF-SEC-001.3: Rate Limiting
System shall implement rate limiting for authentication endpoints:
- Maximum 5 login attempts per 15-minute window
- Clear error messages for rate limit violations
- IP-based rate limiting

### Acceptance Criteria

```gherkin
Scenario: Valid JWT authentication
  Given user has valid JWT token
  When user accesses protected endpoint
  Then request is processed successfully
  And user context is available in request

Scenario: Invalid JWT authentication
  Given user provides invalid or expired JWT token
  When user accesses protected endpoint
  Then request is rejected with 401 Unauthorized
  And error message is "Invalid token"

Scenario: Rate limiting on login attempts
  Given user has made 5 failed login attempts in 15 minutes
  When user attempts to login again
  Then request is rejected with 429 Too Many Requests
  And error message is "Too many login attempts, please try again later"
```
```

### Step 4: Copilot Artifact Analysis & Intent Recovery

**Extract requirements from Copilot-generated documentation and session logs**:

#### **🤖 Copilot-Generated Artifact Detection**

**Identify Copilot-generated documentation that contains implementation rationale**:

```bash
# Find potential Copilot-generated markdown files
find . -name "*.md" -exec grep -l "copilot\|generated\|agent\|assistant" {} \;
find . -name "*copilot*" -o -name "*session*" -o -name "*log*" -type f
find . -name "*.md" -exec grep -l "I will\|I'll\|Let me\|I'm going to" {} \;

# Look for common Copilot documentation patterns
grep -r "implementation plan\|step-by-step\|approach\|rationale" docs/ --include="*.md"
grep -r "## Analysis\|## Implementation\|## Approach\|## Solution" docs/ --include="*.md"

# Find session logs or development notes
find . -name "*session*.md" -o -name "*notes*.md" -o -name "*development*.md"
find . -name "*.md" -exec grep -l "Session\|TODO\|Plan\|Implementation" {} \;
```

#### **📋 Copilot Artifact Types & Analysis Patterns**

**1. Session Logs & Development Reports**:
```markdown
# Example: copilot_session_2024-03-15.md
## Session Summary
I implemented user authentication with the following approach:

### Requirements Analysis
- Need secure user login system
- JWT tokens for stateless authentication  
- Rate limiting to prevent brute force attacks
- Password hashing with bcrypt

### Implementation Decisions
1. **JWT Token Expiry**: Set to 24 hours based on security vs UX balance
2. **Rate Limiting**: 5 attempts per 15 minutes to prevent abuse
3. **Password Requirements**: 8+ characters with special chars for security
4. **Error Messages**: Generic messages to prevent user enumeration

### Code Changes
- Created authController.js with login/register endpoints
- Added JWT middleware for protected routes
- Implemented rate limiting middleware
- Added password validation utilities
```

**Generated Requirements from Copilot Session**:
```markdown
## REQ-F-AUTH-001: User Authentication System
**Source**: Copilot Session 2024-03-15, Authentication Implementation
**Priority**: High (P1)
**Rationale**: "Need secure user login system" - Copilot analysis

### Description
The system shall provide secure user authentication using JWT tokens with rate limiting protection.

### Sub-Requirements

#### REQ-F-AUTH-001.1: JWT Token Authentication  
**Source**: Copilot decision - "JWT tokens for stateless authentication"
**Implementation**: authController.js, JWT middleware
The system shall use JWT tokens for stateless user authentication.

#### REQ-F-AUTH-001.2: Session Duration
**Source**: Copilot decision - "24 hours based on security vs UX balance"  
**Implementation**: JWT expiry configuration
The system shall set JWT token expiry to 24 hours to balance security and user experience.

#### REQ-F-AUTH-001.3: Brute Force Protection
**Source**: Copilot decision - "Rate limiting to prevent brute force attacks"
**Implementation**: Rate limiting middleware  
The system shall limit login attempts to 5 per 15-minute window to prevent brute force attacks.

#### REQ-F-AUTH-001.4: Password Security
**Source**: Copilot decision - "Password hashing with bcrypt"
**Implementation**: Password validation utilities
The system shall hash passwords using bcrypt and require 8+ characters with special characters.

#### REQ-F-AUTH-001.5: Security Through Obscurity  
**Source**: Copilot decision - "Generic messages to prevent user enumeration"
**Implementation**: Error handling in authController.js
The system shall return generic error messages to prevent user enumeration attacks.

### Validation Status
- [ ] **Intent Verification**: Confirm security requirements still applicable
- [ ] **Implementation Alignment**: Verify code matches documented decisions  
- [ ] **Currency Check**: Validate 24-hour expiry still appropriate
- [ ] **Rate Limit Validation**: Confirm 5/15min limit meets current needs
```

**2. Architecture Decision Documents**:
```markdown
# Example: architecture_decisions.md
## Database Choice: PostgreSQL vs MongoDB

### Analysis by Copilot
After analyzing the requirements, I recommended PostgreSQL because:

1. **ACID Compliance**: Financial transactions require strong consistency
2. **Complex Queries**: Reporting needs require complex joins and aggregations  
3. **Data Integrity**: User and order relationships need referential integrity
4. **Mature Ecosystem**: Well-established tools and expertise available

### Implementation
- Set up PostgreSQL with connection pooling
- Created normalized schema for users, orders, products
- Added indexes for common query patterns
- Implemented transaction boundaries for order processing
```

**Generated Architecture Requirements**:
```markdown
## REQ-NF-DATA-001: Database Technology Choice
**Source**: Copilot Architecture Decision, Database Analysis
**Priority**: Critical (P0)
**Rationale**: "Financial transactions require strong consistency" - Copilot

### Description  
The system shall use PostgreSQL as primary database for ACID compliance and complex query support.

### Non-Functional Requirements

#### REQ-NF-DATA-001.1: ACID Compliance
**Source**: Copilot rationale - "Financial transactions require strong consistency"
**Validation Needed**: ✅ Still critical for financial operations
The system shall ensure ACID properties for all financial transactions.

#### REQ-NF-DATA-001.2: Complex Query Support
**Source**: Copilot rationale - "Reporting needs require complex joins"  
**Validation Needed**: ❓ Confirm current reporting requirements
The system shall support complex queries with joins and aggregations for reporting.

#### REQ-NF-DATA-001.3: Data Integrity  
**Source**: Copilot rationale - "User and order relationships need referential integrity"
**Validation Needed**: ✅ Still applicable for data consistency
The system shall maintain referential integrity between related entities.
```

#### **🔍 Code-Documentation Alignment Validation**

**Cross-validate Copilot intentions against current implementation**:

```javascript
// Copilot Documentation Validation Framework
class CopilotArtifactValidator {
  
  async validateIntentionAlignment(copilotDoc, codebase) {
    const validationResults = [];
    
    // Extract stated intentions from Copilot docs
    const intentions = this.extractIntentions(copilotDoc);
    
    for (const intention of intentions) {
      const alignment = await this.checkImplementationAlignment(intention, codebase);
      validationResults.push({
        intention: intention.description,
        source: intention.source,
        currentImplementation: alignment.actualImplementation,
        alignment: alignment.matches ? '✅ ALIGNED' : '❌ MISALIGNED',
        discrepancy: alignment.discrepancy,
        staleness: this.checkStaleness(intention, codebase),
        userValidationNeeded: !alignment.matches
      });
    }
    
    return validationResults;
  }
  
  extractIntentions(document) {
    // Parse Copilot markdown for stated intentions
    const patterns = [
      /I will implement (.*)/g,
      /The approach is to (.*)/g,  
      /I decided to (.*) because (.*)/g,
      /Requirements?: (.*)/g,
      /Implementation plan: (.*)/g
    ];
    
    return this.parseIntentions(document, patterns);
  }
}
```

**Example Validation Results**:
```markdown
## Copilot Intention Validation Report

### ALIGNED Intentions ✅
1. **JWT Authentication Implementation**
   - **Copilot Intent**: "Use JWT tokens for stateless authentication"
   - **Current Code**: JWT middleware implemented in auth/middleware.js
   - **Status**: ✅ ALIGNED - Implementation matches intention

2. **Password Hashing**  
   - **Copilot Intent**: "Hash passwords with bcrypt for security"
   - **Current Code**: bcrypt.hash() used in userController.js
   - **Status**: ✅ ALIGNED - Implementation matches intention

### MISALIGNED Intentions ❌
1. **Rate Limiting Configuration**
   - **Copilot Intent**: "5 attempts per 15 minutes to prevent brute force"
   - **Current Code**: 10 attempts per 30 minutes in rateLimiter.js  
   - **Status**: ❌ MISALIGNED - Configuration changed post-implementation
   - **User Validation Needed**: ❓ Which rate limit is correct for current needs?

2. **JWT Token Expiry**
   - **Copilot Intent**: "24 hours based on security vs UX balance" 
   - **Current Code**: 12 hours in jwt.config.js
   - **Status**: ❌ MISALIGNED - Expiry shortened after implementation
   - **User Validation Needed**: ❓ Was 12-hour change intentional or error?

### OUTDATED Intentions ⚠️
1. **Database Choice Rationale**
   - **Copilot Intent**: "PostgreSQL for complex reporting queries"
   - **Current Code**: Still PostgreSQL but no complex reporting implemented
   - **Status**: ⚠️ OUTDATED - Reporting requirements may have changed
   - **User Validation Needed**: ❓ Are complex reporting features still planned?
```

#### **🗣️ User Interaction Framework for Discrepancy Resolution**

**Structured user interaction to resolve discrepancies and validate intentions**:

```markdown
### User Validation Questions for Discrepancies

#### Discrepancy Type: MISALIGNED Implementation

**DISC-001: Rate Limiting Configuration Mismatch**
**Copilot Intent**: 5 attempts per 15 minutes  
**Current Code**: 10 attempts per 30 minutes
**File**: src/middleware/rateLimiter.js:12

**Questions for User**:
1. ❓ **Which rate limit is correct for your current security needs?**
   - A) Use original Copilot recommendation (5/15min) - more secure
   - B) Keep current implementation (10/30min) - more user-friendly  
   - C) Different configuration based on new requirements

2. ❓ **Why was the rate limit changed from the original design?**
   - A) Security requirements changed
   - B) User experience feedback  
   - C) Accidental modification
   - D) Don't know - needs investigation

3. ❓ **Should we document this as a new requirement?**
   - A) Yes - update requirement with current rationale
   - B) No - revert to original Copilot design
   - C) Need security team review first

**Recommended Actions Based on Response**:
- **If A1+A2**: Document new security requirement with updated rationale
- **If B1+C2**: Revert to original configuration and document reason
- **If C1+D2**: Schedule security review to determine correct configuration

#### Discrepancy Type: OUTDATED Rationale

**DISC-002: Database Rationale No Longer Applicable**  
**Copilot Intent**: "PostgreSQL for complex reporting queries"
**Current State**: No complex reporting features implemented
**Status**: Intent may be outdated

**Questions for User**:
1. ❓ **Are complex reporting features still planned?**
   - A) Yes - still required, just not implemented yet
   - B) No - requirements changed, simple queries sufficient
   - C) Uncertain - need product owner clarification

2. ❓ **If reporting not needed, is PostgreSQL still the right choice?**
   - A) Yes - other benefits (ACID compliance, team expertise) still apply
   - B) No - could simplify to simpler database  
   - C) Need architecture review

3. ❓ **Should we update the requirement documentation?**
   - A) Yes - remove reporting rationale, keep other benefits
   - B) Yes - document that reporting is deferred/cancelled
   - C) No - keep original rationale in case requirements return

### User Interaction Workflow

#### Phase 1: Automated Discrepancy Detection
```bash
# Run discrepancy detection
/code-to-requirements.prompt.md --copilot-validation

# Output: List of misaligned/outdated intentions requiring user input
```

#### Phase 2: Interactive Validation Session
```bash
# Present discrepancies one by one for user resolution
DISC-001: Rate limiting mismatch detected
Current: 10 attempts/30min
Copilot Intent: 5 attempts/15min  

Which should we document as the requirement?
[1] Current implementation (10/30)
[2] Original Copilot design (5/15)  
[3] Different configuration
[4] Need team discussion

User Input: 1
Rationale: User feedback indicated original was too restrictive

# Generate updated requirement
REQ-NF-SEC-001.3: Rate Limiting (UPDATED)
Original: 5 attempts per 15 minutes (Copilot recommendation)
Current: 10 attempts per 30 minutes 
Rationale: Adjusted based on user feedback - original too restrictive
Change Date: [Current date]
Change Reason: User experience optimization
```

#### Phase 3: Documentation Update
```markdown
# Updated requirement with change history
## REQ-NF-SEC-001.3: Authentication Rate Limiting

**Current Requirement**: System shall limit login attempts to 10 per 30-minute window.

**Implementation**: Rate limiting middleware (rateLimiter.js)

**Change History**:
- **v1.0** (Copilot): 5 attempts per 15 minutes - "prevent brute force attacks"
- **v1.1** (User Update): 10 attempts per 30 minutes - "user feedback: original too restrictive"

**Validation Status**: ✅ User confirmed - requirement current as of [date]
```

#### **📊 Copilot Artifact Timeline Analysis**

**Track evolution of Copilot intentions over time**:

```javascript
// Timeline analysis to detect stale documentation
class CopilotTimelineAnalyzer {
  
  analyzeArtifactFreshness(copilotDocs, gitHistory) {
    const timeline = [];
    
    copilotDocs.forEach(doc => {
      const intentions = this.extractIntentions(doc);
      const lastModified = this.getLastModified(doc.path);
      const relatedCodeChanges = this.findRelatedCodeChanges(intentions, gitHistory);
      
      timeline.push({
        document: doc.path,
        intentions: intentions.length,
        lastModified: lastModified,
        relatedCodeChanges: relatedCodeChanges,
        staleness: this.calculateStaleness(lastModified, relatedCodeChanges),
        freshness: this.calculateFreshness(doc, relatedCodeChanges)
      });
    });
    
    return timeline.sort((a, b) => b.staleness - a.staleness);
  }
  
  calculateStaleness(docModified, codeChanges) {
    const lastCodeChange = Math.max(...codeChanges.map(c => c.timestamp));
    const daysSinceDocUpdate = (Date.now() - docModified) / (1000 * 60 * 60 * 24);
    const daysSinceCodeChange = (Date.now() - lastCodeChange) / (1000 * 60 * 60 * 24);
    
    // Staleness increases when code changes after doc was written
    return Math.max(0, daysSinceCodeChange - daysSinceDocUpdate);
  }
}
```

**Timeline Analysis Report**:
```markdown
## Copilot Artifact Freshness Analysis

### STALE Documentation (High Priority Review) ⚠️
1. **architecture_decisions.md** (Modified: 3 months ago)
   - **Related Code Changes**: 15 changes in last month  
   - **Staleness Score**: 87/100 (very stale)
   - **Risk**: Architecture decisions may no longer reflect current code

2. **authentication_implementation.md** (Modified: 6 weeks ago)
   - **Related Code Changes**: 8 changes in auth/ directory
   - **Staleness Score**: 65/100 (moderately stale)
   - **Risk**: Security implementations may have evolved

### FRESH Documentation (Low Priority) ✅  
1. **database_schema.md** (Modified: 1 week ago)
   - **Related Code Changes**: 2 minor schema updates
   - **Staleness Score**: 15/100 (fresh)
   - **Status**: Documentation likely current

### ORPHANED Documentation (Review Required) 🗑️
1. **payment_integration.md** (Modified: 8 months ago)
   - **Related Code**: No payment code found in current codebase
   - **Status**: Feature may have been removed or never implemented
   - **Action**: Verify if payment features still planned
```

### Step 5: Test-Driven Requirements

**Extract acceptance criteria from existing tests**:

#### **Unit Tests → Acceptance Criteria**

**Test Code**:
```javascript
describe('User Registration', () => {
  test('should create user with valid data', async () => {
    const userData = {
      email: 'test@example.com',
      password: 'SecurePassword123',
      name: 'Test User'
    };
    
    const response = await request(app)
      .post('/api/users')
      .send(userData)
      .expect(201);
      
    expect(response.body).toHaveProperty('id');
    expect(response.body.email).toBe(userData.email);
    expect(response.body.name).toBe(userData.name);
    expect(response.body).not.toHaveProperty('password');
  });
  
  test('should reject duplicate email', async () => {
    await User.create({
      email: 'existing@example.com',
      password: 'hashedpassword',
      name: 'Existing User'
    });
    
    const response = await request(app)
      .post('/api/users')
      .send({
        email: 'existing@example.com',
        password: 'NewPassword123',
        name: 'New User'
      })
      .expect(409);
      
    expect(response.body.error).toBe('User already exists');
  });
});
```

**Generated Acceptance Criteria**:
```gherkin
Scenario: Create user with valid data
  Given no user exists with email "test@example.com"
  When POST request to "/api/users" with:
    | email | test@example.com |
    | password | SecurePassword123 |
    | name | Test User |
  Then response status is 201 Created
  And response body contains user ID
  And response body contains email "test@example.com"
  And response body contains name "Test User"
  And response body does not contain password

Scenario: Reject duplicate email registration
  Given user exists with email "existing@example.com"
  When POST request to "/api/users" with:
    | email | existing@example.com |
    | password | NewPassword123 |
    | name | New User |
  Then response status is 409 Conflict
  And response body contains error "User already exists"
```

### Step 5: Requirements Specification Generation

**Complete ISO 29148 Requirements Document**:

```markdown
# System Requirements Specification

**Document Version**: 1.0
**Date**: [Generated Date]
**Source**: Reverse engineered from existing codebase
**Status**: Draft for Review

## 1. Introduction

### 1.1 Purpose
This document specifies the system requirements derived from analysis of the existing codebase implementation.

### 1.2 Scope
This specification covers functional and non-functional requirements extracted from:
- API endpoint implementations
- Business logic functions  
- Database schemas and operations
- Test cases and validation logic
- Security and performance patterns

### 1.3 Definitions and Acronyms
| Term | Definition |
|------|------------|
| API | Application Programming Interface |
| JWT | JSON Web Token |
| NFR | Non-Functional Requirement |
| TDD | Test-Driven Development |

## 2. System Overview

### 2.1 System Purpose
[Inferred from code analysis - what the system does]

### 2.2 System Architecture
[High-level architecture derived from code structure]

### 2.3 Key Components
[Main modules/services identified in codebase]

## 3. Functional Requirements

### 3.1 User Management (REQ-F-USER-xxx)
[Requirements derived from user-related code]

### 3.2 Order Processing (REQ-F-ORDER-xxx)
[Requirements derived from order processing logic]

### 3.3 Product Catalog (REQ-F-PRODUCT-xxx)
[Requirements derived from product management code]

[Continue for all functional areas...]

## 4. Non-Functional Requirements

### 4.1 Performance Requirements (REQ-NF-PERF-xxx)
[Requirements derived from caching, pagination, optimization patterns]

### 4.2 Security Requirements (REQ-NF-SEC-xxx)
[Requirements derived from authentication, validation, rate limiting]

### 4.3 Reliability Requirements (REQ-NF-REL-xxx)
[Requirements derived from error handling, logging patterns]

### 4.4 Usability Requirements (REQ-NF-USE-xxx)
[Requirements derived from UI/UX implementation]

## 5. System Interfaces

### 5.1 External APIs
[Interfaces derived from external service integrations]

### 5.2 Database Interfaces
[Data models and schemas found in code]

### 5.3 User Interfaces
[UI components and pages identified]

## 6. Data Requirements

### 6.1 Data Models
[Entity relationships derived from code]

### 6.2 Data Storage
[Database structure inferred from models and migrations]

### 6.3 Data Processing
[Data transformations found in business logic]

## 7. Traceability Matrix

| Requirement ID | Source Code | Test Cases | Database | UI Component |
|----------------|-------------|------------|----------|--------------|
| REQ-F-USER-001 | userController.js | user.test.js | users table | RegisterForm.jsx |
| REQ-F-ORDER-002 | orderService.py | order.test.py | orders table | OrderForm.vue |
[Continue for all requirements...]

## 8. Specification Validation & Gap Detection Framework

### 8.1 Requirements Validation Against Original Specifications

#### **⚠️ CRITICAL: Prevent Codifying Bugs and Missing Context**

**The Challenge**: Code may have been implemented based on:
- Missing or lost specifications  
- Incomplete understanding of business rules
- Temporary workarounds that became permanent
- Bugs that were never identified or fixed
- Domain knowledge that was never documented

**Simply reverse-engineering from code can perpetuate these issues.**

#### **8.1.1 Missing Specification Detection**

**Identify areas where original specifications may be missing**:

```bash
# Look for suspicious code patterns that suggest missing specs
grep -r "TODO\|FIXME\|HACK\|WORKAROUND\|TEMP" src/
grep -r "magic number\|hardcoded\|assume" src/ 
find src/ -name "*.js" -exec grep -l "// ?" {} \;  # Comments indicating uncertainty
```

**Red Flags in Code**:
```javascript
// Examples of code that suggests missing specifications
const TAX_RATE = 0.08;  // MISSING: Which jurisdiction? What conditions?
if (user.age >= 21) {   // MISSING: Legal requirement? Business rule? Why 21?
  // Alcohol purchase logic
}

function calculateDiscount(customer) {
  // FIXME: Not sure about VIP discount rules
  if (customer.type === 'VIP') {
    return 0.15;  // MISSING: Where did 15% come from?
  }
}

// Hardcoded business rules without explanation
const MAX_RETRY_ATTEMPTS = 3;  // MISSING: SLA requirement? Technical limit?
const SESSION_TIMEOUT = 1800000;  // MISSING: Security policy? UX requirement?
```

**Generated Specification Gaps**:
```markdown
## SPEC-GAP-001: Tax Calculation Rules
**Missing Specification**: Tax rate hardcoded as 8%
**Required Information**: 
- Which tax jurisdiction applies?
- Are there different rates for different product categories?
- How are tax-exempt purchases handled?
- What are the rules for international customers?

**Validation Required**: 
- Consult tax regulations/legal team
- Review original business requirements
- Verify against accounting system rules

## SPEC-GAP-002: Age Verification Business Rules  
**Missing Specification**: Age verification logic assumes 21 as threshold
**Required Information**:
- Legal requirements for age verification (jurisdiction-specific)
- Product categories requiring age verification
- Acceptable forms of age verification
- Consequences of failed verification

**Validation Required**:
- Legal compliance review
- Product manager confirmation
- Regulatory requirements check
```

#### **8.1.2 Business Domain Validation**

**Cross-reference against domain standards and regulations**:

```markdown
### Domain-Specific Validation Checklist

#### Financial/Payment Processing
- [ ] **PCI DSS Compliance**: Does payment handling meet PCI requirements?
- [ ] **GDPR/Data Protection**: Are financial data retention rules followed?
- [ ] **Financial Regulations**: Do calculations comply with local financial laws?
- [ ] **Currency Handling**: Are currency conversions and rounding correct?

#### Healthcare/Medical
- [ ] **HIPAA Compliance**: Is PHI handled according to regulations?
- [ ] **Medical Standards**: Do calculations follow medical protocols?
- [ ] **Drug Interactions**: Are contraindication rules complete?
- [ ] **Emergency Procedures**: Are critical path requirements documented?

#### E-commerce/Retail
- [ ] **Consumer Protection**: Do refund/return policies meet legal requirements?
- [ ] **Inventory Management**: Are stock handling rules complete?
- [ ] **Pricing Rules**: Are discount/promotion calculations accurate?
- [ ] **Tax Compliance**: Are sales tax rules jurisdiction-appropriate?

#### Automotive/Manufacturing
- [ ] **Safety Standards**: Do safety checks meet industry requirements?
- [ ] **Quality Control**: Are QC processes properly specified?
- [ ] **Regulatory Compliance**: Do processes meet DOT/EPA/etc. requirements?
- [ ] **Material Standards**: Are material specifications complete?
```

#### **8.1.3 Inconsistency and Bug Detection**

**Identify potential bugs in extracted requirements**:

**Logic Inconsistency Detection**:
```python
# Example: Inconsistent validation rules across endpoints
# Endpoint 1: User registration
def validate_password(password):
    return len(password) >= 8 and has_special_char(password)

# Endpoint 2: Password reset  
def validate_new_password(password):
    return len(password) >= 6  # INCONSISTENT! Missing special char requirement

# Generated Bug Report:
# BUG-REQ-001: Inconsistent password validation rules
# - Registration requires 8+ chars + special char
# - Reset only requires 6+ chars
# - VALIDATION NEEDED: Which rule is correct?
```

**Edge Case Analysis**:
```javascript
// Example: Missing edge case handling
function calculateAge(birthDate) {
  const today = new Date();
  const birth = new Date(birthDate);
  return today.getFullYear() - birth.getFullYear();
}

// MISSING EDGE CASES:
// - What if birth date is in the future?
// - What about leap years and exact birth dates?
// - How are time zones handled?
// - What's the behavior for invalid dates?

// Generated Gap Analysis:
// GAP-REQ-002: Age calculation missing edge case specifications
// - Future birth dates: Should return error or 0?
// - Timezone handling: Use UTC or local time?
// - Invalid dates: Should throw exception or return null?
```

**Data Consistency Validation**:
```sql
-- Check for data inconsistencies that suggest missing business rules
SELECT user_id, COUNT(*) as order_count, SUM(total) as total_spent
FROM orders 
WHERE total < 0  -- SUSPICIOUS: Negative order totals
GROUP BY user_id;

SELECT * FROM users 
WHERE created_at > NOW()  -- SUSPICIOUS: Future creation dates
OR age < 0 OR age > 150;  -- SUSPICIOUS: Invalid ages

-- Generated Validation Questions:
-- VAL-Q-001: Are negative order totals valid (refunds/credits)?
-- VAL-Q-002: Should age have upper/lower bounds? What are business rules?
```

#### **8.1.4 Stakeholder Validation Framework**

**Process to validate extracted requirements with domain experts**:

```markdown
### Stakeholder Validation Process

#### Phase 1: Expert Review Sessions
**Schedule validation sessions with**:
- **Product Owners**: Business logic and user workflow validation
- **Domain Experts**: Industry-specific rules and compliance requirements
- **Original Developers**: Historical context and design decisions
- **Customer Support**: Real-world usage patterns and edge cases
- **Legal/Compliance**: Regulatory requirements and constraints

#### Phase 2: Validation Questions Template
For each extracted requirement:

**REQ-F-[ID]: [Requirement Title]**
✅ **Business Justification**: Why does this requirement exist?
✅ **Completeness**: Are there missing scenarios or edge cases?
✅ **Accuracy**: Does the implementation match intended behavior?
✅ **Currency**: Are the business rules still valid today?
✅ **Compliance**: Does this meet all regulatory/legal requirements?

**Validation Examples**:
```
REQ-F-USER-001: User Registration with Email Validation

Validation Questions:
1. **Business Justification**: Why is email validation required?
   - Answer: User verification and password recovery capability
   
2. **Completeness**: Are all email validation scenarios covered?
   - Current: Basic format validation
   - Missing: Domain blacklists, disposable email detection, international domains
   
3. **Accuracy**: Does current implementation meet business intent?
   - Issue Found: Code allows emails without TLD (.com, .org, etc.)
   - Business Intent: Should accept all valid international domains
   
4. **Currency**: Are current rules still applicable?
   - Update Needed: GDPR requires explicit consent for email storage
   - New Requirement: Email preferences and unsubscribe capability
   
5. **Compliance**: Does this meet current regulations?
   - Gap Found: Missing GDPR consent tracking
   - Gap Found: No email retention policy implementation
```

#### Phase 3: External Validation Sources
**Cross-reference requirements against external sources**:

```markdown
### External Validation Checklist

#### Industry Standards
- [ ] **ISO Standards**: Relevant ISO standards for the domain
- [ ] **IEEE Standards**: Technical implementation standards  
- [ ] **W3C Standards**: Web accessibility and standards compliance
- [ ] **OWASP Guidelines**: Security best practices
- [ ] **Industry Best Practices**: Domain-specific best practices

#### Regulatory Sources
- [ ] **Government Regulations**: Applicable local/federal laws
- [ ] **Industry Regulations**: Sector-specific compliance requirements
- [ ] **International Standards**: Cross-border compliance requirements
- [ ] **Professional Standards**: Industry association guidelines

#### Business Context
- [ ] **Company Policies**: Internal business rules and policies
- [ ] **SLA Requirements**: Service level agreements and contracts
- [ ] **Competitive Analysis**: Industry standard practices
- [ ] **Customer Contracts**: Specific customer requirements
```

#### **8.1.5 Automated Validation Checks**

**Set up automated validation to catch specification issues**:

```javascript
// Automated business rule validation
class RequirementValidator {
  validateBusinessRules(extractedRequirements) {
    const issues = [];
    
    // Check for hardcoded values without business context
    const hardcodedValues = this.findHardcodedValues();
    hardcodedValues.forEach(value => {
      issues.push({
        type: 'MISSING_SPECIFICATION',
        severity: 'HIGH',
        description: `Hardcoded value ${value.value} in ${value.location}`,
        validation_needed: 'Business justification for this value'
      });
    });
    
    // Check for inconsistent validation rules
    const inconsistencies = this.findValidationInconsistencies();
    inconsistencies.forEach(inconsistency => {
      issues.push({
        type: 'INCONSISTENT_LOGIC', 
        severity: 'CRITICAL',
        description: `Inconsistent validation: ${inconsistency.description}`,
        validation_needed: 'Clarify which validation rule is correct'
      });
    });
    
    return issues;
  }
}
```

#### **8.1.6 Specification Gap Report Template**

```markdown
# Specification Validation Report

## Executive Summary
- **Total Requirements Extracted**: [N]
- **Validation Gaps Identified**: [N] 
- **Critical Missing Specifications**: [N]
- **Stakeholder Review Required**: [N] requirements

## Critical Specification Gaps

### SPEC-GAP-001: [Title]
**Severity**: CRITICAL/HIGH/MEDIUM/LOW
**Category**: Business Logic/Compliance/Security/Performance
**Description**: [What specification is missing]
**Impact**: [Potential consequences of missing specification]
**Current Implementation**: [What the code currently does]
**Validation Required**: [What needs to be verified/researched]
**Stakeholder**: [Who should validate this]
**Timeline**: [When validation is needed]

## Requirements Requiring External Validation

### REQ-F-[ID]: [Title]
**Current Status**: EXTRACTED - NEEDS VALIDATION
**Validation Type**: 
- [ ] Business Logic Review
- [ ] Compliance Check  
- [ ] Technical Standards Review
- [ ] Stakeholder Confirmation

**Validation Questions**:
1. [Specific question about business rule]
2. [Specific question about edge cases]
3. [Specific question about compliance]

**Assigned Validator**: [Name/Role]
**Due Date**: [Date]

## Automated Validation Results

### Inconsistency Alerts
- [List of logical inconsistencies found]
- [List of hardcoded values needing justification]
- [List of missing edge case handling]

### Compliance Checks
- [Industry standard compliance status]
- [Regulatory requirement coverage]
- [Security standard adherence]

## Next Steps
1. **Immediate**: Address critical specification gaps
2. **This Week**: Complete stakeholder validation sessions
3. **This Sprint**: Resolve inconsistencies and missing edge cases
4. **This Quarter**: Implement missing compliance requirements

## Validation Sign-offs
- [ ] **Product Owner**: Business logic validated
- [ ] **Domain Expert**: Industry rules confirmed  
- [ ] **Compliance Officer**: Regulatory requirements verified
- [ ] **Technical Lead**: Implementation feasibility confirmed
```

## 9. Assumptions and Dependencies

### 9.1 Assumptions
[Assumptions made during reverse engineering process]

### 9.2 Dependencies
[External dependencies identified in code]

## 10. Change Management

### 10.1 Requirements Baseline
This specification represents the current implemented functionality as of [date].

### 10.2 Future Changes
New requirements should follow the spec-driven development process using elicitation and refinement prompts.
```

## 🚀 Usage

### Full Codebase Analysis:
```bash
/code-to-requirements.prompt.md Analyze the entire codebase and generate a complete requirements specification.

Focus on:
- All API endpoints and business logic
- Security and performance patterns
- Database operations and data models
- Test cases for acceptance criteria
- Error handling and validation logic

Generate ISO 29148 compliant requirements document.
```

### Specific Component Analysis:
```bash
# Analyze specific module
/code-to-requirements.prompt.md Analyze the user authentication module and generate security requirements.

Files to analyze:
- src/auth/authController.js
- src/middleware/authentication.js  
- tests/auth.test.js

# Analyze API endpoints
/code-to-requirements.prompt.md Extract functional requirements from all REST API endpoints in the order management service.
```

### Test-Driven Requirements:
```bash
/code-to-requirements.prompt.md Generate acceptance criteria based on existing test cases.

Analyze test files:
- tests/user.test.js
- tests/order.test.js
- tests/payment.test.js

Create Gherkin scenarios matching test assertions.
```

## 📊 Analysis Patterns

### **Code Pattern Recognition**:

1. **CRUD Operations** → Functional requirements
2. **Validation Logic** → Business rules and constraints  
3. **Error Handling** → Exception scenarios
4. **Caching/Optimization** → Performance requirements
5. **Authentication/Authorization** → Security requirements
6. **Rate Limiting** → Scalability requirements
7. **Logging/Monitoring** → Observability requirements
8. **Test Assertions** → Acceptance criteria

### **Traceability Establishment**:

- **API Endpoint** ↔ **Functional Requirement**
- **Test Case** ↔ **Acceptance Criteria**
- **Database Model** ↔ **Data Requirement**
- **Validation Logic** ↔ **Business Rule**
- **Error Handler** ↔ **Exception Scenario**

---

**Transform code into compliant requirements!** 🔄