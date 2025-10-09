# Phase 06: Integration

**Standards**: ISO/IEC/IEEE 12207:2017 (Integration Process)  
**XP Integration**: Continuous Integration, Integration Testing, Collective Ownership

## 🎯 Phase Objectives

1. Integrate software units into components
2. Integrate components into complete system
3. Perform integration testing
4. Verify inter-component interfaces
5. Automate integration and deployment
6. Maintain system integrity through continuous integration

## 📂 Working Directory Context

```yaml
applyTo:
  - "06-integration/**/*.md"
  - "06-integration/integration-tests/**"
  - "06-integration/ci-config/**"
  - "06-integration/deployment/**"
  - "**/integration/**/*.test.{js,ts,py}"
```

## 📋 ISO/IEC/IEEE 12207:2017 Compliance

### Integration Process Activities

1. **Integration Strategy**
   - Define integration approach (big-bang, incremental, top-down, bottom-up)
   - Identify integration sequence
   - Define integration environment

2. **Integration Execution**
   - Integrate software units and components
   - Execute integration tests
   - Document integration results

3. **Integration Verification**
   - Verify integrated components
   - Verify interfaces between components
   - Resolve integration issues

## 🎨 XP Practices for Integration

### Continuous Integration (CI)
**Core Practice**:
- Integrate code multiple times per day
- Automated build and test on every commit
- Keep the build green
- Fix broken builds within 10 minutes

### Integration Testing
- Test component interactions
- Test external system interfaces
- Test with real (or realistic) dependencies
- Automated integration test suites

### Collective Code Ownership
- Team owns integration process
- Anyone can fix integration issues
- Shared responsibility for system integrity

## 📝 Required Deliverables

### 1. Integration Strategy Document
**Location**: `integration-strategy.md`

```markdown
# Integration Strategy

## 1. Integration Approach

### Incremental Integration
We will use **continuous incremental integration**:
- Integrate components as they become available
- Test after each integration
- Fix issues immediately
- Maintain working system at all times

### Integration Sequence
```
Phase 1: Core Components
- Database layer
- Business logic layer
- API layer

Phase 2: External Integrations
- Authentication service
- Payment gateway
- Email service

Phase 3: UI Components
- Web frontend
- Mobile apps
- Admin dashboard
```

## 2. Integration Environment

### Development Integration
- **Trigger**: Every commit to feature branch
- **Tests**: Unit + Integration tests
- **Deployment**: None

### Staging Integration
- **Trigger**: Merge to `develop` branch
- **Tests**: Full test suite
- **Deployment**: Staging environment
- **Data**: Anonymized production data

### Production Integration
- **Trigger**: Merge to `main` branch
- **Tests**: Smoke tests
- **Deployment**: Production (blue-green)
- **Rollback**: Automated on failure

## 3. Integration Tools

### CI/CD Pipeline
- **Platform**: GitHub Actions / GitLab CI / Jenkins
- **Build Tool**: [Tool name]
- **Test Runner**: [Tool name]
- **Deployment**: Docker + Kubernetes

### Integration Testing Tools
- **API Testing**: Postman/Newman, REST-assured
- **Database**: TestContainers
- **Mocking**: WireMock, Mockserver
- **Load Testing**: k6, Artillery

## 4. Integration Testing Strategy

### Component Integration Tests
Test interactions between internal components:
```typescript
describe('UserService Integration', () => {
  let app: TestApp;
  let database: TestDatabase;
  
  beforeAll(async () => {
    database = await TestDatabase.create();
    app = await TestApp.create({ database });
  });
  
  afterAll(async () => {
    await app.close();
    await database.close();
  });
  
  it('should create user and persist to database', async () => {
    const userData = { username: 'test', email: 'test@example.com' };
    const user = await app.service.user.create(userData);
    
    const saved = await database.users.findById(user.id);
    expect(saved).toMatchObject(userData);
  });
});
```

### External System Integration Tests
Test interactions with external services:
```typescript
describe('Payment Gateway Integration', () => {
  let mockPaymentServer: MockServer;
  
  beforeAll(() => {
    mockPaymentServer = startMockServer(8080);
  });
  
  it('should process payment via gateway', async () => {
    mockPaymentServer.expectRequest({
      method: 'POST',
      path: '/payments',
      body: { amount: 100 }
    }).respondWith({
      status: 200,
      body: { transactionId: '12345', status: 'success' }
    });
    
    const result = await paymentService.process({ amount: 100 });
    expect(result.transactionId).toBe('12345');
  });
});
```

## 5. Continuous Integration Configuration

### Build Pipeline
1. **Checkout**: Get latest code
2. **Install**: Install dependencies
3. **Lint**: Check code quality
4. **Build**: Compile/transpile code
5. **Unit Test**: Run unit tests
6. **Integration Test**: Run integration tests
7. **Security Scan**: Check for vulnerabilities
8. **Build Artifacts**: Create deployment artifacts
9. **Deploy**: Deploy to environment (if tests pass)

### Quality Gates
- Unit test coverage > 80%
- Integration tests all passing
- No critical vulnerabilities
- Code quality score > 75
- No code smells

## 6. Integration Issue Resolution

### Issue Tracking
- Log all integration failures
- Assign owner immediately
- Target resolution: < 10 minutes
- Escalate if not resolved in 30 minutes

### Root Cause Analysis
For recurring integration issues:
1. Identify root cause
2. Document in post-mortem
3. Implement preventive measures
4. Update integration tests

## 7. Traceability

| Component | Design Element | Integration Tests |
|-----------|---------------|-------------------|
| UserService | DES-C-001 | INT-TEST-001 |
| PaymentGateway | DES-C-005 | INT-TEST-005 |
```

### 2. CI/CD Pipeline Configuration
**Location**: `ci-config/github-actions.yml`

```yaml
name: CI/CD Pipeline

on:
  push:
    branches: [main, develop, 'feature/**']
  pull_request:
    branches: [main, develop]

env:
  NODE_VERSION: '18'
  COVERAGE_THRESHOLD: 80

jobs:
  lint:
    name: Lint Code
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Node.js
        uses: actions/setup-node@v3
        with:
          node-version: ${{ env.NODE_VERSION }}
          cache: 'npm'
      
      - name: Install dependencies
        run: npm ci
      
      - name: Run linter
        run: npm run lint
      
      - name: Check formatting
        run: npm run format:check

  test:
    name: Run Tests
    runs-on: ubuntu-latest
    needs: lint
    
    services:
      postgres:
        image: postgres:14
        env:
          POSTGRES_PASSWORD: test
          POSTGRES_DB: testdb
        options: >-
          --health-cmd pg_isready
          --health-interval 10s
          --health-timeout 5s
          --health-retries 5
        ports:
          - 5432:5432
      
      redis:
        image: redis:7
        options: >-
          --health-cmd "redis-cli ping"
          --health-interval 10s
          --health-timeout 5s
          --health-retries 5
        ports:
          - 6379:6379
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Node.js
        uses: actions/setup-node@v3
        with:
          node-version: ${{ env.NODE_VERSION }}
          cache: 'npm'
      
      - name: Install dependencies
        run: npm ci
      
      - name: Run unit tests
        run: npm run test:unit -- --coverage
        env:
          NODE_ENV: test
      
      - name: Run integration tests
        run: npm run test:integration
        env:
          NODE_ENV: test
          DATABASE_URL: postgresql://postgres:test@localhost:5432/testdb
          REDIS_URL: redis://localhost:6379
      
      - name: Check coverage threshold
        run: |
          COVERAGE=$(node -p "require('./coverage/coverage-summary.json').total.lines.pct")
          echo "Coverage: $COVERAGE%"
          if (( $(echo "$COVERAGE < $COVERAGE_THRESHOLD" | bc -l) )); then
            echo "Coverage $COVERAGE% is below threshold $COVERAGE_THRESHOLD%"
            exit 1
          fi
      
      - name: Upload coverage reports
        uses: codecov/codecov-action@v3
        with:
          files: ./coverage/coverage-final.json
          flags: unittests
          name: codecov-umbrella

  security:
    name: Security Scan
    runs-on: ubuntu-latest
    needs: test
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Run security audit
        run: npm audit --audit-level=high
      
      - name: Run SAST scan
        uses: github/codeql-action/analyze@v2
      
      - name: Check for secrets
        uses: trufflesecurity/trufflehog@main
        with:
          path: ./
          base: main
          head: HEAD

  build:
    name: Build Application
    runs-on: ubuntu-latest
    needs: [test, security]
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Node.js
        uses: actions/setup-node@v3
        with:
          node-version: ${{ env.NODE_VERSION }}
          cache: 'npm'
      
      - name: Install dependencies
        run: npm ci
      
      - name: Build application
        run: npm run build
      
      - name: Build Docker image
        run: docker build -t myapp:${{ github.sha }} .
      
      - name: Save Docker image
        run: docker save myapp:${{ github.sha }} | gzip > myapp.tar.gz
      
      - name: Upload build artifacts
        uses: actions/upload-artifact@v3
        with:
          name: build-artifacts
          path: |
            dist/
            myapp.tar.gz

  deploy-staging:
    name: Deploy to Staging
    runs-on: ubuntu-latest
    needs: build
    if: github.ref == 'refs/heads/develop'
    environment:
      name: staging
      url: https://staging.example.com
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Download build artifacts
        uses: actions/download-artifact@v3
        with:
          name: build-artifacts
      
      - name: Deploy to staging
        run: |
          # Deploy using your preferred method
          kubectl apply -f k8s/staging/
      
      - name: Run smoke tests
        run: npm run test:smoke
        env:
          BASE_URL: https://staging.example.com

  deploy-production:
    name: Deploy to Production
    runs-on: ubuntu-latest
    needs: build
    if: github.ref == 'refs/heads/main'
    environment:
      name: production
      url: https://example.com
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Download build artifacts
        uses: actions/download-artifact@v3
        with:
          name: build-artifacts
      
      - name: Deploy to production (blue-green)
        run: |
          # Blue-green deployment
          kubectl apply -f k8s/production/
      
      - name: Run smoke tests
        run: npm run test:smoke
        env:
          BASE_URL: https://example.com
      
      - name: Rollback on failure
        if: failure()
        run: kubectl rollout undo deployment/myapp
```

### 3. Integration Test Suite
**Location**: `integration-tests/user-workflow.test.ts`

```typescript
import { TestApp } from '../test-utils/TestApp';
import { TestDatabase } from '../test-utils/TestDatabase';

describe('User Workflow Integration Tests', () => {
  let app: TestApp;
  let db: TestDatabase;
  let apiClient: APIClient;
  
  beforeAll(async () => {
    // Setup test environment
    db = await TestDatabase.create();
    app = await TestApp.create({
      database: db,
      redis: await TestRedis.create()
    });
    apiClient = app.getAPIClient();
  });
  
  afterAll(async () => {
    await app.close();
    await db.close();
  });
  
  beforeEach(async () => {
    await db.clear();
  });
  
  describe('User Registration Flow', () => {
    it('should complete full registration workflow', async () => {
      // 1. Register user
      const registrationData = {
        username: 'newuser',
        email: 'newuser@example.com',
        password: 'SecurePass123!'
      };
      
      const registerResponse = await apiClient.post('/api/auth/register', registrationData);
      expect(registerResponse.status).toBe(201);
      expect(registerResponse.data).toHaveProperty('userId');
      
      const userId = registerResponse.data.userId;
      
      // 2. Verify user exists in database
      const userInDb = await db.users.findById(userId);
      expect(userInDb).toBeDefined();
      expect(userInDb.username).toBe('newuser');
      expect(userInDb.isActive).toBe(true);
      
      // 3. Login with credentials
      const loginResponse = await apiClient.post('/api/auth/login', {
        username: 'newuser',
        password: 'SecurePass123!'
      });
      expect(loginResponse.status).toBe(200);
      expect(loginResponse.data).toHaveProperty('accessToken');
      
      const accessToken = loginResponse.data.accessToken;
      
      // 4. Access protected resource
      const profileResponse = await apiClient.get('/api/users/me', {
        headers: { Authorization: `Bearer ${accessToken}` }
      });
      expect(profileResponse.status).toBe(200);
      expect(profileResponse.data.username).toBe('newuser');
      
      // 5. Verify audit log
      const auditLogs = await db.auditLogs.findByUserId(userId);
      expect(auditLogs).toHaveLength(2); // Registration + Login
      expect(auditLogs[0].action).toBe('USER_REGISTERED');
      expect(auditLogs[1].action).toBe('USER_LOGGED_IN');
    });
  });
  
  describe('External Service Integration', () => {
    it('should integrate with email service', async () => {
      const mockEmailServer = startMockEmailServer();
      
      // Register user (triggers welcome email)
      const response = await apiClient.post('/api/auth/register', {
        username: 'test',
        email: 'test@example.com',
        password: 'Pass123!'
      });
      
      // Verify email was sent
      await waitFor(() => {
        const emails = mockEmailServer.getReceivedEmails();
        expect(emails).toHaveLength(1);
        expect(emails[0].to).toBe('test@example.com');
        expect(emails[0].subject).toContain('Welcome');
      });
      
      mockEmailServer.close();
    });
  });
});
```

## 🚨 Critical Requirements for This Phase

### Always Do
✅ Integrate continuously (multiple times/day)  
✅ Run full test suite on integration  
✅ Fix broken builds immediately (<10 min)  
✅ Test all component interfaces  
✅ Use real dependencies where possible  
✅ Automate integration testing  
✅ Monitor integration health  
✅ Document integration issues  

### Never Do
❌ Commit on broken build  
❌ Skip integration tests  
❌ Integrate without testing  
❌ Leave build broken overnight  
❌ Ignore integration failures  
❌ Disable failing tests  

## 📊 Phase Exit Criteria

✅ All components integrated successfully  
✅ Integration tests passing  
✅ CI/CD pipeline operational  
✅ No blocking integration issues  
✅ System deploys automatically  
✅ Integration documentation complete  

## 🎯 Next Phase

**Phase 07: Verification & Validation** (`07-verification-validation/`)

---

**Remember**: "If it hurts, do it more often" - The more frequently you integrate, the easier it becomes!
