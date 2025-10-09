# Phase 05: Implementation (Construction)

**Standards**: ISO/IEC/IEEE 12207:2017 (Implementation Process)  
**XP Integration**: Test-Driven Development (TDD), Pair Programming, Continuous Integration, Refactoring

## 🎯 Phase Objectives

1. Implement design specifications as working code
2. Apply Test-Driven Development (Red-Green-Refactor)
3. Write clean, maintainable, tested code
4. Perform continuous integration
5. Practice pair programming and collective ownership
6. Refactor continuously to improve design

## 📂 Working Directory Context

```yaml
applyTo:
  - "05-implementation/src/**/*.{js,ts,py,java,cs,go,rb}"
  - "05-implementation/tests/**/*.{js,ts,py,java,cs,go,rb}"
  - "05-implementation/docs/**/*.md"
  - "**/test*.{js,ts,py}"
  - "**/*.test.{js,ts,py}"
  - "**/*.spec.{js,ts,py}"
```

## 📋 ISO/IEC/IEEE 12207:2017 Compliance

### Implementation Process Activities

1. **Software Construction**
   - Implement software units
   - Develop unit tests
   - Update documentation

2. **Software Unit Verification**
   - Execute unit tests
   - Verify against design
   - Fix defects

3. **Software Integration**
   - Integrate units into components
   - Verify interfaces
   - Resolve integration issues

## 🎨 XP Core Practices for Implementation

### 1. Test-Driven Development (TDD)

**Red-Green-Refactor Cycle**:

```
🔴 RED: Write a failing test
  ↓
🟢 GREEN: Write minimal code to pass
  ↓
🔵 REFACTOR: Improve design while keeping tests green
  ↓
Repeat
```

**Example TDD Flow**:
```typescript
// 1. RED: Write failing test first
describe('UserService', () => {
  it('should create a new user with valid data', async () => {
    // Arrange
    const userData = { username: 'john', email: 'john@example.com' };
    const mockRepository = createMockRepository();
    const service = new UserService(mockRepository);
    
    // Act
    const user = await service.createUser(userData);
    
    // Assert
    expect(user).toBeDefined();
    expect(user.username).toBe('john');
    expect(mockRepository.save).toHaveBeenCalledWith(expect.objectContaining(userData));
  });
});

// 2. GREEN: Write minimal implementation to pass
class UserService {
  constructor(private repository: IUserRepository) {}
  
  async createUser(userData: CreateUserData): Promise<User> {
    const user = new User(userData);
    return await this.repository.save(user);
  }
}

// 3. REFACTOR: Improve design
class UserService {
  constructor(
    private repository: IUserRepository,
    private validator: IValidator,
    private logger: ILogger
  ) {}
  
  async createUser(userData: CreateUserData): Promise<User> {
    this.logger.info('Creating user', { username: userData.username });
    
    // Validate input
    await this.validator.validate(userData, CreateUserSchema);
    
    // Create and save user
    const user = new User(userData);
    const savedUser = await this.repository.save(user);
    
    this.logger.info('User created', { userId: savedUser.id });
    return savedUser;
  }
}
```

### 2. Pair Programming

**Roles**:
- **Driver**: Types the code
- **Navigator**: Reviews, thinks strategically, suggests improvements

**Benefits**:
- Continuous code review
- Knowledge sharing
- Better design decisions
- Fewer defects

**Best Practices**:
- Switch roles every 30 minutes
- Communicate constantly
- Respect each other
- Take breaks together

### 3. Continuous Integration

**CI Practice**:
```yaml
# .github/workflows/ci.yml
name: Continuous Integration

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Node.js
        uses: actions/setup-node@v3
        with:
          node-version: '18'
      
      - name: Install dependencies
        run: npm ci
      
      - name: Run linter
        run: npm run lint
      
      - name: Run tests
        run: npm test -- --coverage
      
      - name: Check coverage
        run: |
          if [ $(grep -oP '\d+(?=%)' coverage/coverage-summary.json | head -1) -lt 80 ]; then
            echo "Coverage below 80%"
            exit 1
          fi
```

**CI Rules**:
- Integrate multiple times per day
- Run all tests before committing
- Fix broken builds immediately (within 10 minutes)
- Never commit on a broken build

### 4. Collective Code Ownership

**Principles**:
- Anyone can modify any code
- No "my code" or "your code"
- Team is responsible for quality
- Share knowledge through pairing

**Practices**:
- Consistent coding standards
- Comprehensive tests protect against breakage
- Code reviews for all changes
- Pair rotation

### 5. Coding Standards

**Enforce with tools**:
```json
// .eslintrc.json
{
  "extends": ["eslint:recommended", "plugin:@typescript-eslint/recommended"],
  "rules": {
    "max-lines": ["error", 300],
    "max-lines-per-function": ["error", 50],
    "complexity": ["error", 10],
    "max-depth": ["error", 3],
    "max-params": ["error", 4]
  }
}
```

### 6. Refactoring

**Continuous Refactoring**:
- Refactor when you see code smells
- Keep tests green while refactoring
- Small, safe refactorings
- Leave code better than you found it (Boy Scout Rule)

**Common Refactorings**:
```typescript
// Before: Long method
class OrderProcessor {
  processOrder(order: Order) {
    // Validate order (20 lines)
    // Calculate prices (30 lines)
    // Apply discounts (25 lines)
    // Process payment (40 lines)
    // Send notifications (15 lines)
  }
}

// After: Extract methods
class OrderProcessor {
  processOrder(order: Order) {
    this.validateOrder(order);
    const total = this.calculateTotal(order);
    const discounted = this.applyDiscounts(total, order);
    this.processPayment(discounted, order);
    this.sendNotifications(order);
  }
  
  private validateOrder(order: Order): void { /* ... */ }
  private calculateTotal(order: Order): number { /* ... */ }
  private applyDiscounts(total: number, order: Order): number { /* ... */ }
  private processPayment(amount: number, order: Order): void { /* ... */ }
  private sendNotifications(order: Order): void { /* ... */ }
}
```

## 📝 Implementation Guidelines

### File Organization

```
src/
├── domain/                  # Domain models (business logic)
│   ├── entities/           # Domain entities
│   ├── value-objects/      # Value objects
│   └── services/           # Domain services
├── application/            # Application services (use cases)
│   ├── commands/           # Command handlers
│   ├── queries/            # Query handlers
│   └── dtos/               # Data transfer objects
├── infrastructure/         # External concerns
│   ├── database/           # Database implementation
│   ├── http/               # HTTP clients
│   └── messaging/          # Message queues
└── presentation/           # User interface
    ├── controllers/        # API controllers
    ├── views/              # UI views
    └── middleware/         # Request/response middleware

tests/
├── unit/                   # Unit tests (co-located preferred)
├── integration/            # Integration tests
└── fixtures/               # Test data and mocks
```

### Code Quality Standards

#### 1. SOLID Principles

**Single Responsibility Principle (SRP)**:
```typescript
// Bad: Class has multiple responsibilities
class User {
  save() { /* database logic */ }
  sendEmail() { /* email logic */ }
  validate() { /* validation logic */ }
}

// Good: Each class has single responsibility
class User {
  // Only domain logic
}

class UserRepository {
  save(user: User) { /* database logic */ }
}

class EmailService {
  sendWelcomeEmail(user: User) { /* email logic */ }
}

class UserValidator {
  validate(user: User) { /* validation logic */ }
}
```

**Open/Closed Principle (OCP)**:
```typescript
// Open for extension, closed for modification
interface PaymentProcessor {
  process(amount: number): Promise<void>;
}

class CreditCardProcessor implements PaymentProcessor {
  async process(amount: number): Promise<void> { /* ... */ }
}

class PayPalProcessor implements PaymentProcessor {
  async process(amount: number): Promise<void> { /* ... */ }
}

// Adding new payment method doesn't modify existing code
class CryptoProcessor implements PaymentProcessor {
  async process(amount: number): Promise<void> { /* ... */ }
}
```

**Liskov Substitution Principle (LSP)**: Subtypes must be substitutable for base types

**Interface Segregation Principle (ISP)**: Many specific interfaces > one general interface

**Dependency Inversion Principle (DIP)**: Depend on abstractions, not concretions

#### 2. Clean Code Practices

**Meaningful Names**:
```typescript
// Bad
const d = new Date();
function getData(u) { }

// Good
const currentDate = new Date();
function getUserById(userId: string) { }
```

**Small Functions**:
```typescript
// Bad: Function does too much
function processUserRegistration(userData) {
  // Validate (20 lines)
  // Hash password (5 lines)
  // Save to database (10 lines)
  // Send email (15 lines)
  // Log audit (5 lines)
}

// Good: Small, focused functions
function processUserRegistration(userData: UserData): Promise<User> {
  const validatedData = validateUserData(userData);
  const user = createUser(validatedData);
  const savedUser = await saveUser(user);
  await sendWelcomeEmail(savedUser);
  logUserCreation(savedUser);
  return savedUser;
}
```

**DRY (Don't Repeat Yourself)**:
```typescript
// Bad: Duplication
function calculatePriceForAdult(basePrice) {
  return basePrice * 1.1 + basePrice * 0.15;
}

function calculatePriceForChild(basePrice) {
  return basePrice * 0.5 * 1.1 + basePrice * 0.5 * 0.15;
}

// Good: Extract common logic
function calculatePrice(basePrice: number, multiplier: number): number {
  const withTax = basePrice * multiplier * 1.1;
  const withServiceFee = withTax + (basePrice * multiplier * 0.15);
  return withServiceFee;
}

function calculatePriceForAdult(basePrice: number): number {
  return calculatePrice(basePrice, 1.0);
}

function calculatePriceForChild(basePrice: number): number {
  return calculatePrice(basePrice, 0.5);
}
```

#### 3. Error Handling

```typescript
// Define error hierarchy
class ApplicationError extends Error {
  constructor(
    message: string,
    public readonly code: string,
    public readonly statusCode: number = 500
  ) {
    super(message);
    this.name = this.constructor.name;
    Error.captureStackTrace(this, this.constructor);
  }
}

class ValidationError extends ApplicationError {
  constructor(message: string, public readonly field?: string) {
    super(message, 'VALIDATION_ERROR', 400);
  }
}

class NotFoundError extends ApplicationError {
  constructor(resource: string, id: string) {
    super(`${resource} with id ${id} not found`, 'NOT_FOUND', 404);
  }
}

// Use in code
async function getUser(userId: string): Promise<User> {
  if (!isValidUuid(userId)) {
    throw new ValidationError('Invalid user ID format', 'userId');
  }
  
  const user = await userRepository.findById(userId);
  
  if (!user) {
    throw new NotFoundError('User', userId);
  }
  
  return user;
}

// Global error handler
app.use((error: Error, req: Request, res: Response, next: NextFunction) => {
  if (error instanceof ApplicationError) {
    return res.status(error.statusCode).json({
      error: error.code,
      message: error.message,
      ...(error instanceof ValidationError && { field: error.field })
    });
  }
  
  logger.error('Unhandled error', error);
  return res.status(500).json({
    error: 'INTERNAL_ERROR',
    message: 'An unexpected error occurred'
  });
});
```

#### 4. Logging

```typescript
interface ILogger {
  debug(message: string, context?: object): void;
  info(message: string, context?: object): void;
  warn(message: string, context?: object): void;
  error(message: string, error: Error, context?: object): void;
}

class UserService {
  constructor(
    private repository: IUserRepository,
    private logger: ILogger
  ) {}
  
  async createUser(userData: CreateUserData): Promise<User> {
    this.logger.info('Creating user', { username: userData.username });
    
    try {
      const user = await this.repository.save(new User(userData));
      this.logger.info('User created successfully', { userId: user.id });
      return user;
    } catch (error) {
      this.logger.error('Failed to create user', error as Error, { userData });
      throw error;
    }
  }
}
```

## 🚨 Critical Requirements for This Phase

### Always Do (XP Practices)
✅ **Write tests first (TDD)** - Red → Green → Refactor  
✅ **Integrate continuously** - Multiple times per day  
✅ **Pair program** - For complex or critical code  
✅ **Refactor mercilessly** - Keep code clean  
✅ **Follow coding standards** - Use linters and formatters  
✅ **Collective ownership** - Anyone can modify any code  
✅ **Keep it simple** - YAGNI, avoid over-engineering  
✅ **Run all tests** - Before every commit  

### Always Do (Standards Compliance)
✅ Implement per design specifications  
✅ Trace code to design elements  
✅ Document public APIs  
✅ Handle all error cases  
✅ Log significant events  
✅ Validate all inputs  
✅ Maintain >80% test coverage  

### Never Do
❌ Write code without tests (breaks TDD)  
❌ Commit on broken build  
❌ Skip refactoring ("we'll do it later")  
❌ Create long methods (>50 lines)  
❌ Create complex methods (cyclomatic complexity >10)  
❌ Ignore code smells  
❌ Skip error handling  
❌ Hard-code configuration  

## 📊 Code Quality Metrics

### Target Metrics
- **Test Coverage**: >80% (unit tests)
- **Cyclomatic Complexity**: <10 per method
- **Method Length**: <50 lines
- **Class Length**: <300 lines
- **Method Parameters**: <4 parameters
- **Maintainability Index**: >75
- **Code Duplication**: <3%

### Quality Gates
```yaml
# quality-gates.yml
coverage:
  minimum: 80
  
complexity:
  maximum: 10
  
code-smells:
  maximum: 0
  
security-hotspots:
  maximum: 0
  
duplications:
  maximum: 3
```

## 📊 Phase Entry Criteria

✅ Design specifications complete and approved  
✅ Development environment set up  
✅ CI/CD pipeline configured  
✅ Coding standards defined  
✅ Test framework configured  

## 📊 Phase Exit Criteria

✅ All code implemented per design  
✅ Unit tests written for all code (TDD)  
✅ Test coverage >80%  
✅ All tests passing  
✅ Code reviewed and approved  
✅ Coding standards compliance verified  
✅ No critical bugs  
✅ Documentation updated  
✅ Code integrated into main branch  
✅ Traceability established (code → design)  

## 🔗 Traceability

```
DES-C-XXX (Design Component/Class)
  ↓
CODE-XXX (Implementation)
TEST-UNIT-XXX (Unit Tests)
  ↓
[Next Phase: Integration - INT-XXX]
```

## 📚 Standards and Resources

- **ISO/IEC/IEEE 12207:2017** - Implementation process
- **Test-Driven Development By Example** - Kent Beck
- **Clean Code** - Robert C. Martin
- **Refactoring** - Martin Fowler
- **XP Explained** - Kent Beck

## 🎯 Next Phase

Once this phase is complete, proceed to:
**Phase 06: Integration** (`06-integration/`)

---

**Remember**: 
- **TDD**: Tests first, always! Red → Green → Refactor
- **CI**: Integrate frequently, fix breaks immediately
- **Refactor**: Keep code clean continuously
- **Simple**: Do the simplest thing that could possibly work (YAGNI)
- **Quality**: >80% coverage, low complexity, no duplication

**The XP mantra**: "Make it work, make it right, make it fast" (in that order!)
