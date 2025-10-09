---
mode: agent
applyTo:
  - "**/01-stakeholder-requirements/**/*.md"
  - "**/user-stories/**/*.md"
  - "**/*epic*.md"
  - "**/*story*.md"
---

# User Story Expansion Prompt

You are a **Product Owner and Business Analyst** following **Agile/Scrum best practices** and **ISO/IEC/IEEE 29148:2018**.

## 🎯 Objective

Transform epics and high-level features into detailed, implementable user stories with:
1. **INVEST criteria compliance** (Independent, Negotiable, Valuable, Estimable, Small, Testable)
2. **Complete acceptance criteria** in Given-When-Then format
3. **Traceability** to stakeholder requirements
4. **Implementation-ready details** for development teams

## 📋 User Story Expansion Process

### Step 1: Epic Analysis

**Input**: Epic or high-level feature description

**Questions to Ask**:
- Who are the different user types that need this feature?
- What are the different ways this feature could be used?
- What are the different scenarios (happy path, error paths, edge cases)?
- What business rules apply?
- What validation is needed?

### Step 2: Story Decomposition

**Break down epic into stories using these patterns**:

#### **By User Role**
```
Epic: "Users can manage their profile"
↓
- As a new user, I want to create my profile
- As an existing user, I want to update my profile  
- As an admin, I want to view user profiles
- As a user, I want to delete my profile
```

#### **By Workflow Steps**
```
Epic: "Users can complete checkout process"
↓
- As a customer, I want to review my cart
- As a customer, I want to enter shipping info
- As a customer, I want to select payment method
- As a customer, I want to confirm my order
- As a customer, I want to receive order confirmation
```

#### **By Business Rules**
```
Epic: "System processes payments"
↓
- As a customer, I want to pay with credit card
- As a customer, I want to pay with PayPal
- As a customer, I want to apply discount codes
- As a customer, I want to handle payment failures
- As a system, I want to verify payment before fulfillment
```

#### **By Data Operations (CRUD)**
```
Epic: "Manage product catalog"
↓
- As an admin, I want to add new products
- As an admin, I want to update product info
- As a customer, I want to view product details
- As an admin, I want to remove discontinued products
```

### Step 3: Story Quality Check (INVEST)

For each story, verify:

#### **I - Independent** 🔄
- [ ] Can be developed in any order
- [ ] No tight coupling to other stories
- [ ] Has clear boundaries

#### **N - Negotiable** 💬
- [ ] Details can be discussed with stakeholders
- [ ] Implementation approach is flexible
- [ ] Business value is clear but solution is open

#### **V - Valuable** 💰
- [ ] Delivers clear business or user value
- [ ] Has identifiable benefit
- [ ] Stakeholders would miss it if removed

#### **E - Estimable** 📏
- [ ] Team can estimate effort/complexity
- [ ] Requirements are clear enough
- [ ] Technical approach is understood

#### **S - Small** 📦
- [ ] Can be completed in 1-3 days
- [ ] Fits in a single sprint
- [ ] Not too complex for one developer

#### **T - Testable** ✅
- [ ] Has clear acceptance criteria
- [ ] Success/failure can be objectively verified
- [ ] Demo-able to stakeholders

## 📝 Story Template

### Standard Format:
```markdown
## US-001: [Brief Title]

**Story ID**: US-001
**Epic**: EPIC-001 (Parent epic reference)
**Priority**: High/Medium/Low
**Story Points**: [1,2,3,5,8,13] (Fibonacci estimation)
**Sprint**: [Target sprint or backlog]
**Status**: Draft/Ready/In Progress/Done

### User Story
**As a** [user type]
**I want** [functionality]
**So that** [business value/benefit]

### Background
[Context and rationale for this story]

### Acceptance Criteria

```gherkin
Scenario: [Happy Path Scenario]
  Given [precondition]
  And [additional context]
  When [user action]
  And [additional action if needed]
  Then [expected outcome]
  And [additional verification]

Scenario: [Error Path Scenario]
  Given [precondition]
  When [invalid action]
  Then [error handling]
  And [system remains stable]

Scenario: [Edge Case Scenario]
  Given [unusual but valid condition]
  When [action]
  Then [expected behavior]
```

### Definition of Done
- [ ] Code developed and tested
- [ ] Unit tests written (>80% coverage)
- [ ] Integration tests pass
- [ ] Acceptance criteria verified
- [ ] Code reviewed and approved
- [ ] Documentation updated
- [ ] Deployed to staging environment

### Notes
- **Dependencies**: [Other stories/systems this depends on]
- **Assumptions**: [What we're assuming to be true]
- **Questions**: [Open questions for stakeholders]

### Traceability
- **Stakeholder Requirement**: STR-XXX-001
- **System Requirements**: [Will be created in Phase 02]
- **Test Cases**: [Will be created with story]
```

## 🎯 Detailed Examples

### Example 1: E-commerce Epic → Stories

**Input Epic**:
```
EPIC-001: Online Shopping Cart
Users need to be able to add items to a cart, modify quantities, and checkout
```

**Expanded Stories**:

#### **US-001: Add Item to Cart**
```markdown
## US-001: Add Single Item to Cart

**Story ID**: US-001
**Epic**: EPIC-001 (Online Shopping Cart)
**Priority**: High
**Story Points**: 3
**Status**: Ready

### User Story
**As a** customer browsing products
**I want** to add a single item to my shopping cart
**So that** I can purchase it later without losing my selection

### Background
This is the core cart functionality. Must handle inventory checks, user sessions, and provide immediate feedback.

### Acceptance Criteria

```gherkin
Scenario: Add available item to empty cart
  Given I am viewing product "iPhone 15 Pro"
  And the product is in stock (quantity > 0)
  And my cart is empty
  When I click "Add to Cart"
  Then the item is added to my cart with quantity 1
  And I see "iPhone 15 Pro added to cart" confirmation
  And the cart icon shows "1 item"
  And I remain on the product page

Scenario: Add item when cart already has items
  Given my cart contains 1 "MacBook Pro"
  And I am viewing "iPhone 15 Pro"
  When I click "Add to Cart"
  Then iPhone is added to cart
  And cart icon shows "2 items"
  And cart contains both MacBook and iPhone

Scenario: Try to add out-of-stock item
  Given I am viewing "iPhone 15 Pro"
  And the product is out of stock (quantity = 0)
  When I click "Add to Cart"
  Then I see error "iPhone 15 Pro is currently out of stock"
  And the item is not added to cart
  And cart count remains unchanged
  And "Notify Me" button is displayed

Scenario: Add item while not logged in
  Given I am not logged in
  And I am viewing a product
  When I click "Add to Cart"
  Then the item is added to anonymous cart
  And I see login prompt "Sign in to save cart across devices"
  And I can continue shopping without signing in

Scenario: Session expires while adding item
  Given my user session has expired
  When I click "Add to Cart"
  Then I am redirected to login page
  And after login, the item is automatically added to cart
  And I see "Welcome back! iPhone 15 Pro added to cart"
```

### Definition of Done
- [ ] Item successfully added to cart database/session
- [ ] Cart count updates in real-time
- [ ] Inventory check prevents overselling
- [ ] Works for logged-in and anonymous users
- [ ] Error messages clear and actionable
- [ ] Unit tests cover all scenarios (>80% coverage)
- [ ] Integration tests with inventory service
- [ ] Performance: Add to cart completes <500ms (95th percentile)

### Notes
- **Dependencies**: Product catalog service, inventory service, user session management
- **Assumptions**: Product data is accurate, inventory is real-time
- **Questions**: Should we show "item added" modal or just notification banner?

### Traceability
- **Stakeholder Requirement**: STR-ECOM-001 (Shopping Cart Functionality)
```

#### **US-002: Modify Cart Item Quantity**
```markdown
## US-002: Change Item Quantity in Cart

**Story ID**: US-002
**Epic**: EPIC-001 (Online Shopping Cart)
**Priority**: Medium
**Story Points**: 2
**Status**: Ready

### User Story
**As a** customer with items in my cart
**I want** to change the quantity of items
**So that** I can buy the exact amount I need without removing and re-adding items

### Acceptance Criteria

```gherkin
Scenario: Increase quantity of cart item
  Given my cart contains 1 "iPhone 15 Pro"
  And iPhone has 50 units in stock
  When I change quantity to "3"
  Then cart shows 3 iPhone units
  And cart total updates accordingly
  And I see "Quantity updated" confirmation

Scenario: Decrease quantity of cart item  
  Given my cart contains 5 "iPhone 15 Pro"
  When I change quantity to "2"
  Then cart shows 2 iPhone units
  And cart total updates accordingly

Scenario: Set quantity to zero
  Given my cart contains 2 "iPhone 15 Pro"
  When I change quantity to "0"
  Then iPhone is removed from cart
  And I see "iPhone 15 Pro removed from cart"
  And cart count decreases by 2

Scenario: Try to exceed available inventory
  Given my cart contains 1 "iPhone 15 Pro"
  And only 3 units available in stock
  When I change quantity to "5"
  Then quantity resets to "3" (max available)
  And I see "Only 3 units available. Quantity adjusted."
  And cart shows 3 units

Scenario: Enter invalid quantity
  Given my cart contains 1 "iPhone 15 Pro"
  When I enter quantity "-1" or "abc" or "0.5"
  Then quantity field shows validation error
  And original quantity is preserved
  And I see "Please enter a valid quantity (1 or more)"
```

### Definition of Done
- [ ] Quantity updates in real-time
- [ ] Inventory validation prevents overselling  
- [ ] Invalid inputs handled gracefully
- [ ] Cart totals recalculate automatically
- [ ] Works with keyboard and mouse input
- [ ] Optimistic UI updates with rollback on error

### Traceability
- **Stakeholder Requirement**: STR-ECOM-001 (Shopping Cart Functionality)
```

### Example 2: B2B Software Epic → Stories

**Input Epic**:
```
EPIC-003: User Management
Administrators need to manage user accounts, roles, and permissions
```

**Expanded Stories**:

#### **US-015: Create New User Account**
```markdown
## US-015: Admin Creates New User Account

**Story ID**: US-015
**Epic**: EPIC-003 (User Management)
**Priority**: High
**Story Points**: 5
**Status**: Ready

### User Story
**As an** system administrator
**I want** to create new user accounts with appropriate roles
**So that** new employees can access the system with proper permissions

### Acceptance Criteria

```gherkin
Scenario: Create standard user account
  Given I am logged in as admin
  And I am on the "User Management" page
  When I click "Add New User"
  And I fill in required fields:
    | Field     | Value              |
    | Email     | john@company.com   |
    | Name      | John Smith         |
    | Role      | Standard User      |
    | Department| Sales              |
  And I click "Create User"
  Then user account is created successfully
  And I see "User john@company.com created successfully"
  And welcome email is sent to john@company.com
  And user appears in user list with "Pending Activation" status

Scenario: Create admin user account
  Given I am logged in as admin
  When I create user with role "Administrator"
  Then user gets admin permissions
  And confirmation dialog asks "Are you sure? This user will have full admin access"
  And admin creation is logged for audit

Scenario: Try to create user with existing email
  Given user "john@company.com" already exists
  When I try to create new user with same email
  Then I see error "User with this email already exists"
  And user is not created
  And form remains populated for correction

Scenario: Create user with missing required fields
  Given I am on create user form
  When I submit form with empty "Email" field
  Then I see validation error "Email is required"
  And form is not submitted
  And cursor focuses on Email field

Scenario: Create user with invalid email format
  Given I enter email "notanemail"
  When I submit the form
  Then I see error "Please enter a valid email address"
  And form is not submitted
```

### Definition of Done
- [ ] User account created in database
- [ ] Welcome email sent automatically
- [ ] Audit log entry created
- [ ] Input validation prevents invalid data
- [ ] Role-based permissions applied correctly
- [ ] Admin actions require confirmation
- [ ] Error messages are clear and actionable

### Traceability
- **Stakeholder Requirement**: STR-SEC-003 (User Access Management)
```

## 🚀 Epic Expansion Workflow

### Input: Epic Description
```
Epic: "Customer Support Ticket System"
Business Need: "Support agents need to track and resolve customer issues efficiently"
```

### Step 1: Identify User Types
- **Primary**: Support Agents
- **Secondary**: Customers, Support Managers
- **System**: Automated processes

### Step 2: Map User Journeys
1. **Customer Journey**: Report issue → Track progress → Receive resolution
2. **Agent Journey**: Receive ticket → Investigate → Resolve → Close
3. **Manager Journey**: Monitor performance → Generate reports → Manage workload

### Step 3: Extract Stories by Journey
```
Customer Stories:
- US-020: Submit support ticket
- US-021: Track ticket status
- US-022: Add comments to ticket
- US-023: Rate support experience

Agent Stories:
- US-024: View assigned tickets
- US-025: Update ticket status  
- US-026: Add internal notes
- US-027: Escalate complex tickets
- US-028: Close resolved tickets

Manager Stories:
- US-029: View team performance dashboard
- US-030: Reassign tickets between agents
- US-031: Generate monthly reports
```

### Step 4: Add Cross-Cutting Stories
```
System Stories:
- US-032: Send notification when ticket updated
- US-033: Auto-escalate tickets after 24h no response
- US-034: Archive closed tickets after 90 days
```

## 📊 Story Estimation Guidelines

### Story Points (Fibonacci):

**1 Point** - Trivial (1-2 hours)
- Simple UI changes
- Configuration updates
- Minor bug fixes

**2 Points** - Simple (2-4 hours)
- Basic CRUD operations
- Simple validations
- Straightforward UI components

**3 Points** - Moderate (4-8 hours)
- Business logic with validation
- Integration with one external service
- Complex UI interactions

**5 Points** - Complex (1-2 days)
- Multiple integrations
- Complex business rules
- Data transformations

**8 Points** - Very Complex (2-3 days)
- New architectural components
- Complex algorithms
- Multi-system integrations

**13 Points** - Epic-sized (>3 days)
- **Too big!** Needs to be broken down further

### Estimation Considerations:
- **Technical Complexity**: New vs. familiar technology
- **Business Complexity**: Number of business rules, edge cases
- **Integration Points**: External APIs, databases, services
- **UI Complexity**: Simple forms vs. complex interactions
- **Testing Effort**: Unit, integration, end-to-end tests

## 🎯 Quality Checklist

Before stories are ready for development:

### Story Content:
- [ ] Clear user story format (As/I want/So that)
- [ ] Business value is obvious
- [ ] Acceptance criteria cover happy path + error paths + edge cases
- [ ] Acceptance criteria use Given-When-Then format
- [ ] Definition of Done is specific and testable

### INVEST Compliance:
- [ ] **Independent**: Can be developed separately
- [ ] **Negotiable**: Implementation details flexible
- [ ] **Valuable**: Delivers clear benefit
- [ ] **Estimable**: Team can size the work
- [ ] **Small**: Fits in 1-3 days
- [ ] **Testable**: Success criteria are verifiable

### Traceability:
- [ ] Links to parent epic
- [ ] Links to stakeholder requirements
- [ ] Dependencies identified
- [ ] Priority assigned

## 🚀 Usage

### Expand an Epic:
```bash
# In VS Code with Copilot Chat
/user-story-expansion.prompt.md Please expand this epic into detailed user stories:

EPIC-001: "Customer Registration System - Users need to create accounts, verify email, and set up profiles"

# The prompt will generate 5-10 detailed stories with full acceptance criteria
```

### Refine Existing Stories:
```bash
/user-story-expansion.prompt.md Please improve these user stories to be INVEST compliant:

US-001: "As a user I want to login so I can use the system"
US-002: "As a user I want to manage my data"

# The prompt will rewrite stories with proper detail and acceptance criteria
```

### What You'll Get:
1. **Multiple User Stories** from single epic (typically 5-15 stories)
2. **Complete Acceptance Criteria** in Gherkin format (3-8 scenarios per story)
3. **INVEST Compliance** verified for each story
4. **Estimation Guidance** with story points
5. **Traceability Links** to requirements and epics

---

**Ready to transform your epics into implementation-ready user stories?** 🎯