---
mode: agent
applyTo:
  - "**/src/**/*"
  - "**/lib/**/*"
  - "**/app/**/*"
  - "**/tests/**/*"
  - "**/test/**/*"
  - "**/__tests__/**/*"
---

# Test Gap Filler Prompt

You are a **Test Engineer** following **TDD principles** and **IEEE 1012-2016** Verification & Validation standards.

## 🎯 Objective

Identify and fill test coverage gaps to achieve >80% test coverage:
1. **Analyze current test coverage** and identify untested code paths
2. **Generate missing unit tests** with AAA pattern (Arrange, Act, Assert)
3. **Create integration tests** for component interactions
4. **Add edge case tests** for boundary conditions and error scenarios
5. **Ensure requirement traceability** for all generated tests

## 📊 Test Coverage Analysis Framework

### Step 1: Coverage Assessment

**Generate coverage reports for different languages**:

#### **JavaScript/TypeScript (Jest)**
```bash
npm run test:coverage
# or
npx jest --coverage --coverageReporters=text-lcov --coverageReporters=html
```

#### **Python (pytest)**
```bash
pytest --cov=src --cov-report=html --cov-report=term
```

#### **Java (JaCoCo)**
```bash
mvn test jacoco:report
```

#### **Go**
```bash
go test -coverprofile=coverage.out ./...
go tool cover -html=coverage.out
```

### **Coverage Analysis Template**

```markdown
# Test Coverage Analysis

## Overall Coverage: [X]%
**Target**: 80% minimum
**Gap**: [Y]% points to target

## Coverage by Category
- **Statements**: [X]% ([covered]/[total])
- **Branches**: [Y]% ([covered]/[total])
- **Functions**: [Z]% ([covered]/[total])
- **Lines**: [W]% ([covered]/[total])

## Coverage by Component

| Component | Lines | Coverage | Gap | Priority |
|-----------|-------|----------|-----|----------|
| userController.js | 234 | 92% | ✅ Target met | Maintain |
| orderService.py | 445 | 65% | 🔴 15% gap | HIGH |
| paymentGateway.js | 189 | 45% | 🔴 35% gap | CRITICAL |
| utilityHelpers.js | 78 | 23% | 🔴 57% gap | MEDIUM |

## Critical Untested Code Paths
1. **paymentGateway.js** - Lines 45-67 (error handling)
2. **orderService.py** - Lines 123-156 (discount calculation)
3. **userController.js** - Lines 89-92 (email validation edge cases)
```

### Step 2: Untested Code Identification

**Identify specific untested code paths**:

#### **JavaScript Example Analysis**

**Uncovered Code**:
```javascript
// paymentGateway.js - Lines 45-67 (0% coverage)
async function processRefund(transactionId, amount, reason) {
  try {
    // Validate refund amount
    if (amount <= 0) {
      throw new Error('Refund amount must be positive');
    }
    
    // Get original transaction
    const transaction = await Transaction.findById(transactionId);
    if (!transaction) {
      throw new Error('Transaction not found');
    }
    
    if (transaction.amount < amount) {
      throw new Error('Refund amount exceeds original transaction');
    }
    
    // Process refund with payment provider
    const refundResult = await paymentProvider.refund({
      transactionId,
      amount,
      reason
    });
    
    if (!refundResult.success) {
      throw new Error(`Refund failed: ${refundResult.error}`);
    }
    
    // Update transaction record
    await Transaction.update(transactionId, {
      status: 'refunded',
      refundAmount: amount,
      refundReason: reason,
      refundedAt: new Date()
    });
    
    return refundResult;
  } catch (error) {
    logger.error('Refund processing failed', { transactionId, amount, error });
    throw error;
  }
}
```

**Generated Unit Tests**:
```javascript
// tests/paymentGateway.test.js
describe('processRefund', () => {
  
  beforeEach(() => {
    jest.clearAllMocks();
  });

  // TEST-PAY-003-01: Happy path refund processing
  test('should process valid refund successfully', async () => {
    // Arrange
    const transactionId = 'txn-123';
    const amount = 50.00;
    const reason = 'Customer request';
    
    const mockTransaction = {
      id: transactionId,
      amount: 100.00,
      status: 'completed'
    };
    
    const mockRefundResult = {
      success: true,
      refundId: 'ref-456'
    };
    
    Transaction.findById.mockResolvedValue(mockTransaction);
    paymentProvider.refund.mockResolvedValue(mockRefundResult);
    Transaction.update.mockResolvedValue(true);
    
    // Act
    const result = await processRefund(transactionId, amount, reason);
    
    // Assert
    expect(result).toEqual(mockRefundResult);
    expect(Transaction.findById).toHaveBeenCalledWith(transactionId);
    expect(paymentProvider.refund).toHaveBeenCalledWith({
      transactionId,
      amount,
      reason
    });
    expect(Transaction.update).toHaveBeenCalledWith(transactionId, {
      status: 'refunded',
      refundAmount: amount,
      refundReason: reason,
      refundedAt: expect.any(Date)
    });
  });

  // TEST-PAY-003-02: Invalid refund amount (negative)
  test('should reject negative refund amount', async () => {
    // Arrange
    const transactionId = 'txn-123';
    const amount = -10.00;
    const reason = 'Invalid test';
    
    // Act & Assert
    await expect(processRefund(transactionId, amount, reason))
      .rejects.toThrow('Refund amount must be positive');
    
    expect(Transaction.findById).not.toHaveBeenCalled();
  });

  // TEST-PAY-003-03: Invalid refund amount (zero)
  test('should reject zero refund amount', async () => {
    // Arrange
    const transactionId = 'txn-123';
    const amount = 0;
    const reason = 'Invalid test';
    
    // Act & Assert
    await expect(processRefund(transactionId, amount, reason))
      .rejects.toThrow('Refund amount must be positive');
  });

  // TEST-PAY-003-04: Transaction not found
  test('should handle transaction not found', async () => {
    // Arrange
    const transactionId = 'nonexistent';
    const amount = 50.00;
    const reason = 'Customer request';
    
    Transaction.findById.mockResolvedValue(null);
    
    // Act & Assert
    await expect(processRefund(transactionId, amount, reason))
      .rejects.toThrow('Transaction not found');
    
    expect(paymentProvider.refund).not.toHaveBeenCalled();
  });

  // TEST-PAY-003-05: Refund amount exceeds original
  test('should reject refund amount exceeding original transaction', async () => {
    // Arrange
    const transactionId = 'txn-123';
    const amount = 150.00; // More than original 100.00
    const reason = 'Customer request';
    
    const mockTransaction = {
      id: transactionId,
      amount: 100.00,
      status: 'completed'
    };
    
    Transaction.findById.mockResolvedValue(mockTransaction);
    
    // Act & Assert
    await expect(processRefund(transactionId, amount, reason))
      .rejects.toThrow('Refund amount exceeds original transaction');
    
    expect(paymentProvider.refund).not.toHaveBeenCalled();
  });

  // TEST-PAY-003-06: Payment provider refund failure
  test('should handle payment provider refund failure', async () => {
    // Arrange
    const transactionId = 'txn-123';
    const amount = 50.00;
    const reason = 'Customer request';
    
    const mockTransaction = {
      id: transactionId,
      amount: 100.00,
      status: 'completed'
    };
    
    const mockRefundResult = {
      success: false,
      error: 'Insufficient funds in merchant account'
    };
    
    Transaction.findById.mockResolvedValue(mockTransaction);
    paymentProvider.refund.mockResolvedValue(mockRefundResult);
    
    // Act & Assert
    await expect(processRefund(transactionId, amount, reason))
      .rejects.toThrow('Refund failed: Insufficient funds in merchant account');
    
    expect(Transaction.update).not.toHaveBeenCalled();
  });

  // TEST-PAY-003-07: Database update failure
  test('should handle database update failure', async () => {
    // Arrange
    const transactionId = 'txn-123';
    const amount = 50.00;
    const reason = 'Customer request';
    
    const mockTransaction = {
      id: transactionId,
      amount: 100.00,
      status: 'completed'
    };
    
    const mockRefundResult = {
      success: true,
      refundId: 'ref-456'
    };
    
    Transaction.findById.mockResolvedValue(mockTransaction);
    paymentProvider.refund.mockResolvedValue(mockRefundResult);
    Transaction.update.mockRejectedValue(new Error('Database connection failed'));
    
    // Act & Assert
    await expect(processRefund(transactionId, amount, reason))
      .rejects.toThrow('Database connection failed');
    
    expect(logger.error).toHaveBeenCalledWith(
      'Refund processing failed',
      expect.objectContaining({
        transactionId,
        amount,
        error: expect.any(Error)
      })
    );
  });

  // TEST-PAY-003-08: Edge case - exact refund amount
  test('should process refund for exact transaction amount', async () => {
    // Arrange
    const transactionId = 'txn-123';
    const amount = 100.00; // Exact match
    const reason = 'Full refund';
    
    const mockTransaction = {
      id: transactionId,
      amount: 100.00,
      status: 'completed'
    };
    
    const mockRefundResult = {
      success: true,
      refundId: 'ref-456'
    };
    
    Transaction.findById.mockResolvedValue(mockTransaction);
    paymentProvider.refund.mockResolvedValue(mockRefundResult);
    Transaction.update.mockResolvedValue(true);
    
    // Act
    const result = await processRefund(transactionId, amount, reason);
    
    // Assert
    expect(result).toEqual(mockRefundResult);
    expect(Transaction.update).toHaveBeenCalledWith(
      transactionId,
      expect.objectContaining({
        status: 'refunded',
        refundAmount: 100.00
      })
    );
  });
});
```

#### **Python Example Analysis**

**Uncovered Code**:
```python
# orderService.py - Lines 123-156 (0% coverage)
def calculate_discount(subtotal, discount_code, customer_tier):
    """Calculate discount amount based on code and customer tier."""
    
    if not discount_code:
        return 0.0
    
    # Get discount configuration
    discount = DiscountCode.get_by_code(discount_code)
    if not discount:
        raise InvalidDiscountError(f"Invalid discount code: {discount_code}")
    
    if not discount.is_active():
        raise ExpiredDiscountError(f"Discount code expired: {discount_code}")
    
    # Check minimum order amount
    if subtotal < discount.minimum_order:
        raise InsufficientOrderError(
            f"Minimum order ${discount.minimum_order} required for {discount_code}"
        )
    
    # Calculate base discount
    if discount.type == 'percentage':
        discount_amount = subtotal * (discount.value / 100)
    elif discount.type == 'fixed':
        discount_amount = discount.value
    else:
        raise InvalidDiscountError(f"Unknown discount type: {discount.type}")
    
    # Apply customer tier multiplier
    tier_multipliers = {
        'bronze': 1.0,
        'silver': 1.1,
        'gold': 1.2,
        'platinum': 1.5
    }
    
    multiplier = tier_multipliers.get(customer_tier, 1.0)
    discount_amount *= multiplier
    
    # Apply maximum discount limit
    if discount.max_discount and discount_amount > discount.max_discount:
        discount_amount = discount.max_discount
    
    # Ensure discount doesn't exceed subtotal
    if discount_amount > subtotal:
        discount_amount = subtotal
    
    return round(discount_amount, 2)
```

**Generated Unit Tests**:
```python
# tests/test_order_service.py
import pytest
from unittest.mock import Mock, patch
from order_service import calculate_discount
from exceptions import InvalidDiscountError, ExpiredDiscountError, InsufficientOrderError

class TestCalculateDiscount:
    
    def setup_method(self):
        """Set up test fixtures."""
        self.mock_discount = Mock()
        self.mock_discount.minimum_order = 50.0
        self.mock_discount.max_discount = None
        
    # TEST-ORD-004-01: No discount code provided
    def test_calculate_discount_no_code(self):
        """Should return 0 when no discount code provided."""
        # Act
        result = calculate_discount(100.0, None, 'bronze')
        
        # Assert
        assert result == 0.0
        
    def test_calculate_discount_empty_code(self):
        """Should return 0 when empty discount code provided."""
        # Act
        result = calculate_discount(100.0, '', 'bronze')
        
        # Assert
        assert result == 0.0

    # TEST-ORD-004-02: Invalid discount code
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_invalid_code(self, mock_get_code):
        """Should raise error for invalid discount code."""
        # Arrange
        mock_get_code.return_value = None
        
        # Act & Assert
        with pytest.raises(InvalidDiscountError) as exc_info:
            calculate_discount(100.0, 'INVALID', 'bronze')
        
        assert "Invalid discount code: INVALID" in str(exc_info.value)

    # TEST-ORD-004-03: Expired discount code
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_expired_code(self, mock_get_code):
        """Should raise error for expired discount code."""
        # Arrange
        self.mock_discount.is_active.return_value = False
        mock_get_code.return_value = self.mock_discount
        
        # Act & Assert
        with pytest.raises(ExpiredDiscountError) as exc_info:
            calculate_discount(100.0, 'EXPIRED', 'bronze')
        
        assert "Discount code expired: EXPIRED" in str(exc_info.value)

    # TEST-ORD-004-04: Insufficient order amount
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_insufficient_order(self, mock_get_code):
        """Should raise error when order below minimum."""
        # Arrange
        self.mock_discount.is_active.return_value = True
        self.mock_discount.minimum_order = 100.0
        mock_get_code.return_value = self.mock_discount
        
        # Act & Assert
        with pytest.raises(InsufficientOrderError) as exc_info:
            calculate_discount(50.0, 'SAVE10', 'bronze')
        
        assert "Minimum order $100.0 required" in str(exc_info.value)

    # TEST-ORD-004-05: Percentage discount calculation
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_percentage(self, mock_get_code):
        """Should calculate percentage discount correctly."""
        # Arrange
        self.mock_discount.is_active.return_value = True
        self.mock_discount.type = 'percentage'
        self.mock_discount.value = 10.0  # 10%
        mock_get_code.return_value = self.mock_discount
        
        # Act
        result = calculate_discount(100.0, 'SAVE10', 'bronze')
        
        # Assert
        assert result == 10.0  # 10% of $100

    # TEST-ORD-004-06: Fixed discount calculation
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_fixed(self, mock_get_code):
        """Should calculate fixed discount correctly."""
        # Arrange
        self.mock_discount.is_active.return_value = True
        self.mock_discount.type = 'fixed'
        self.mock_discount.value = 15.0  # $15 off
        mock_get_code.return_value = self.mock_discount
        
        # Act
        result = calculate_discount(100.0, 'SAVE15', 'bronze')
        
        # Assert
        assert result == 15.0

    # TEST-ORD-004-07: Unknown discount type
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_unknown_type(self, mock_get_code):
        """Should raise error for unknown discount type."""
        # Arrange
        self.mock_discount.is_active.return_value = True
        self.mock_discount.type = 'unknown'
        mock_get_code.return_value = self.mock_discount
        
        # Act & Assert
        with pytest.raises(InvalidDiscountError) as exc_info:
            calculate_discount(100.0, 'UNKNOWN', 'bronze')
        
        assert "Unknown discount type: unknown" in str(exc_info.value)

    # TEST-ORD-004-08: Customer tier multipliers
    @pytest.mark.parametrize("tier,multiplier,expected", [
        ('bronze', 1.0, 10.0),
        ('silver', 1.1, 11.0),
        ('gold', 1.2, 12.0),
        ('platinum', 1.5, 15.0),
        ('unknown', 1.0, 10.0)  # Default to 1.0
    ])
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_tier_multipliers(self, mock_get_code, tier, multiplier, expected):
        """Should apply correct tier multiplier."""
        # Arrange
        self.mock_discount.is_active.return_value = True
        self.mock_discount.type = 'percentage'  
        self.mock_discount.value = 10.0  # 10%
        mock_get_code.return_value = self.mock_discount
        
        # Act
        result = calculate_discount(100.0, 'SAVE10', tier)
        
        # Assert
        assert result == expected

    # TEST-ORD-004-09: Maximum discount limit
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_max_limit(self, mock_get_code):
        """Should apply maximum discount limit."""
        # Arrange
        self.mock_discount.is_active.return_value = True
        self.mock_discount.type = 'percentage'
        self.mock_discount.value = 50.0  # 50% would be $50
        self.mock_discount.max_discount = 20.0  # But max is $20
        mock_get_code.return_value = self.mock_discount
        
        # Act
        result = calculate_discount(100.0, 'SAVE50', 'bronze')
        
        # Assert
        assert result == 20.0  # Limited to max discount

    # TEST-ORD-004-10: Discount cannot exceed subtotal
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_exceeds_subtotal(self, mock_get_code):
        """Should limit discount to subtotal amount."""
        # Arrange
        self.mock_discount.is_active.return_value = True
        self.mock_discount.type = 'fixed'
        self.mock_discount.value = 150.0  # $150 discount
        mock_get_code.return_value = self.mock_discount
        
        # Act
        result = calculate_discount(100.0, 'BIGDISCOUNT', 'bronze')
        
        # Assert  
        assert result == 100.0  # Limited to subtotal

    # TEST-ORD-004-11: Edge case - minimum order exactly met
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_minimum_order_exact(self, mock_get_code):
        """Should work when order exactly meets minimum."""
        # Arrange
        self.mock_discount.is_active.return_value = True
        self.mock_discount.type = 'percentage'
        self.mock_discount.value = 10.0
        self.mock_discount.minimum_order = 100.0
        mock_get_code.return_value = self.mock_discount
        
        # Act
        result = calculate_discount(100.0, 'SAVE10', 'bronze')
        
        # Assert
        assert result == 10.0

    # TEST-ORD-004-12: Rounding to 2 decimal places
    @patch('order_service.DiscountCode.get_by_code')
    def test_calculate_discount_rounding(self, mock_get_code):
        """Should round result to 2 decimal places."""
        # Arrange
        self.mock_discount.is_active.return_value = True
        self.mock_discount.type = 'percentage'
        self.mock_discount.value = 7.77  # Will create repeating decimal
        mock_get_code.return_value = self.mock_discount
        
        # Act
        result = calculate_discount(100.0, 'SAVE777', 'bronze')
        
        # Assert
        assert result == 7.77  # Rounded to 2 decimal places
```

### Step 3: Integration Test Generation

**Identify integration test gaps**:

#### **API Integration Tests**

```javascript
// tests/integration/userRegistration.integration.test.js
describe('User Registration Integration', () => {
  
  beforeEach(async () => {
    await testDb.clear();
    await testDb.seed();
  });

  // TEST-INT-USER-001: Complete registration flow
  test('should register user with all validations', async () => {
    // Arrange
    const userData = {
      email: 'integration@test.com',
      password: 'SecurePassword123!',
      name: 'Integration Test User'
    };

    // Act
    const response = await request(app)
      .post('/api/users')
      .send(userData)
      .expect(201);

    // Assert - API Response
    expect(response.body).toHaveProperty('id');
    expect(response.body.email).toBe(userData.email);
    expect(response.body).not.toHaveProperty('password');

    // Assert - Database State
    const userInDb = await User.findById(response.body.id);
    expect(userInDb).toBeTruthy();
    expect(userInDb.email).toBe(userData.email);
    expect(userInDb.password).not.toBe(userData.password); // Should be hashed

    // Assert - Password Hashing
    const isPasswordValid = await bcrypt.compare(userData.password, userInDb.password);
    expect(isPasswordValid).toBe(true);

    // Assert - Email Sent
    expect(emailService.sendWelcomeEmail).toHaveBeenCalledWith(
      userData.email,
      userData.name
    );
  });

  // TEST-INT-USER-002: Registration with duplicate email
  test('should prevent duplicate email registration', async () => {
    // Arrange - Create existing user
    await User.create({
      email: 'existing@test.com',
      password: await bcrypt.hash('ExistingPassword123!', 12),
      name: 'Existing User'
    });

    const duplicateUserData = {
      email: 'existing@test.com',
      password: 'NewPassword123!',
      name: 'Duplicate User'
    };

    // Act
    const response = await request(app)
      .post('/api/users')
      .send(duplicateUserData)
      .expect(409);

    // Assert - Error Response
    expect(response.body.error).toBe('User already exists');

    // Assert - No New User Created
    const users = await User.findByEmail('existing@test.com');
    expect(users).toHaveLength(1);
    expect(users[0].name).toBe('Existing User'); // Original user unchanged
  });

  // TEST-INT-USER-003: Registration triggers downstream services
  test('should trigger user onboarding workflow', async () => {
    // Arrange
    const userData = {
      email: 'workflow@test.com',
      password: 'WorkflowPassword123!',
      name: 'Workflow User'
    };

    // Act
    const response = await request(app)
      .post('/api/users')
      .send(userData)
      .expect(201);

    // Assert - User Profile Created
    const userProfile = await UserProfile.findByUserId(response.body.id);
    expect(userProfile).toBeTruthy();
    expect(userProfile.onboardingStatus).toBe('pending');

    // Assert - Analytics Event Tracked
    expect(analyticsService.track).toHaveBeenCalledWith('user_registered', {
      userId: response.body.id,
      email: userData.email,
      registrationDate: expect.any(Date)
    });

    // Assert - Welcome Email Queued
    expect(emailQueue.add).toHaveBeenCalledWith('welcome_email', {
      userId: response.body.id,
      email: userData.email,
      name: userData.name
    });
  });
});
```

### Step 4: Test Quality Standards

**Ensure all generated tests meet quality standards**:

#### **Test Quality Checklist**

- [ ] **AAA Pattern**: Arrange, Act, Assert clearly separated
- [ ] **Single Responsibility**: Each test verifies one behavior  
- [ ] **Descriptive Names**: Test names describe the scenario
- [ ] **Independent**: Tests don't depend on each other
- [ ] **Repeatable**: Tests produce same result every time
- [ ] **Fast**: Tests run quickly (<100ms per unit test)
- [ ] **Deterministic**: No flaky tests due to timing/randomness

#### **Test Naming Convention**

```javascript
// Pattern: should_[expected behavior]_when_[condition]
test('should return 400 when email format is invalid', () => {
  // Test implementation
});

test('should create user successfully when all data is valid', () => {
  // Test implementation
});

test('should hash password when user is created', () => {
  // Test implementation
});
```

#### **Test Organization**

```javascript
describe('UserController', () => {
  describe('POST /api/users', () => {
    describe('when data is valid', () => {
      test('should create user with 201 status', () => {});
      test('should return user data without password', () => {});
      test('should hash password before saving', () => {});
    });
    
    describe('when data is invalid', () => {
      test('should return 400 for invalid email', () => {});
      test('should return 400 for weak password', () => {});
      test('should return 409 for duplicate email', () => {});
    });
    
    describe('when external service fails', () => {
      test('should return 500 when database is unavailable', () => {});
      test('should return 500 when email service fails', () => {});
    });
  });
});
```

### Step 5: Test Generation Strategy

#### **Priority-Based Test Generation**

**Critical Priority (P0)**:
1. **Main business logic paths** - Core functionality that drives business value
2. **Data integrity operations** - Database operations, transactions
3. **Security-critical code** - Authentication, authorization, input validation
4. **Payment/financial operations** - Money-related calculations and transfers

**High Priority (P1)**:
1. **Error handling paths** - Exception scenarios and recovery
2. **Integration points** - External API calls, database interactions
3. **Configuration-dependent code** - Environment-specific behavior
4. **Performance-critical paths** - Code that affects response times

**Medium Priority (P2)**:
1. **Edge cases and boundary conditions** - Min/max values, empty inputs
2. **Logging and monitoring code** - Observability features
3. **Helper and utility functions** - Supporting code
4. **UI interaction logic** - Frontend event handlers

**Low Priority (P3)**:
1. **Constants and configuration** - Static values and settings
2. **Simple getter/setter methods** - Basic data access
3. **Deprecated code paths** - Legacy functionality being phased out

#### **Test Generation Template**

```markdown
# Test Generation Plan for [Component]

## Coverage Gap Analysis
- **Current Coverage**: [X]%
- **Target Coverage**: 80%
- **Gap**: [Y]% points
- **Lines to Cover**: [Z] lines

## Untested Code Paths
1. **[Function/Method Name]** (Lines X-Y)
   - **Risk Level**: [Critical/High/Medium/Low]
   - **Complexity**: [High/Medium/Low]  
   - **Business Impact**: [High/Medium/Low]
   - **Test Priority**: [P0/P1/P2/P3]

## Generated Tests

### Unit Tests
- [ ] **TEST-[COMP]-[NUM]-01**: Happy path scenario
- [ ] **TEST-[COMP]-[NUM]-02**: Invalid input handling
- [ ] **TEST-[COMP]-[NUM]-03**: Boundary conditions
- [ ] **TEST-[COMP]-[NUM]-04**: Exception scenarios
- [ ] **TEST-[COMP]-[NUM]-05**: Edge cases

### Integration Tests  
- [ ] **TEST-INT-[COMP]-001**: End-to-end workflow
- [ ] **TEST-INT-[COMP]-002**: External service integration
- [ ] **TEST-INT-[COMP]-003**: Database transaction handling

### Performance Tests (if applicable)
- [ ] **TEST-PERF-[COMP]-001**: Response time under load
- [ ] **TEST-PERF-[COMP]-002**: Memory usage patterns
- [ ] **TEST-PERF-[COMP]-003**: Concurrent request handling

## Requirement Traceability
- **REQ-F-[ID]**: [Requirement description] → Tests: TEST-[COMP]-[NUM]-01, TEST-[COMP]-[NUM]-02
- **REQ-NF-[ID]**: [Requirement description] → Tests: TEST-PERF-[COMP]-001

## Success Criteria
- [ ] All P0/P1 code paths have tests
- [ ] Coverage increases to 80%+ 
- [ ] All tests pass in CI/CD pipeline
- [ ] Tests are maintainable and well-documented
```

## 🚀 Usage

### Comprehensive Test Gap Analysis:
```bash
/test-gap-filler.prompt.md Analyze test coverage and generate missing tests.

Current coverage: 67%
Target: 80%

Focus on:
- Untested error handling paths
- Missing integration tests
- Edge cases and boundary conditions
- Critical business logic without tests

Generate high-quality tests with AAA pattern and requirement traceability.
```

### Specific Component Testing:
```bash
# Generate tests for specific file
/test-gap-filler.prompt.md Generate comprehensive test suite for paymentGateway.js

Current coverage: 45%
Missing tests for:
- Refund processing logic
- Payment failure scenarios  
- Currency conversion edge cases

# Generate integration tests
/test-gap-filler.prompt.md Create integration tests for user registration flow

Test complete workflow:
- API request validation
- Database operations
- Email service integration
- Analytics tracking
```

### Performance-Critical Testing:
```bash
/test-gap-filler.prompt.md Generate performance tests for order processing service

Requirements:
- Handle 1000 concurrent orders
- Response time <500ms (95th percentile)
- Memory usage <500MB under load
```

## 📊 Test Coverage Dashboard

### **Coverage Report Template**

```markdown
# Test Coverage Report

**Generated**: [Date]
**Target Coverage**: 80%
**Current Coverage**: [X]%

## Coverage by Component

| Component | Lines | Coverage | Tests | Status |
|-----------|-------|----------|-------|--------|
| userController | 234 | 92% | 15 | ✅ Target Met |
| orderService | 445 | 78% | 12 | ⚠️ Close to Target |
| paymentGateway | 189 | 45% | 3 | 🔴 Below Target |
| utilityHelpers | 78 | 95% | 8 | ✅ Excellent |

## Critical Gaps (Immediate Action Required)

1. **paymentGateway.js**: 35% gap (67 untested lines)
   - Missing: Error handling, refund logic, webhook processing
   - **Risk**: Critical (handles financial transactions)
   - **Action**: Generate 8-10 additional tests

2. **orderService.py**: 2% gap (9 untested lines)  
   - Missing: Discount calculation edge cases
   - **Risk**: Medium (business logic)
   - **Action**: Generate 3-4 additional tests

## Test Quality Metrics

- **Test Execution Time**: [X]ms (Target: <5000ms)
- **Flaky Tests**: [X] (Target: 0)
- **Test Maintainability**: [X]/10 (Target: 8+)

## Recommendations

1. **Immediate**: Add tests for paymentGateway.js
2. **This Week**: Complete orderService.py coverage  
3. **This Month**: Review and refactor flaky tests
```

---

**Every line tested, every bug prevented!** 🧪