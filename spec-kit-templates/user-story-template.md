# User Story Template (XP Format)

> **Extreme Programming Practice**: User stories are the XP way of capturing requirements with customer involvement.

---

## Story Information

```yaml
storyId: STORY-XXX
title: [Story title]
status: backlog  # backlog | ready | in-progress | testing | done
priority: high  # critical | high | medium | low
storyPoints: 5  # Fibonacci: 1, 2, 3, 5, 8, 13, 21
iteration: Sprint-XX
assignedTo: [Team member]
createdBy: [Product Owner]
createdDate: 2025-02-15
traceability:
  stakeholderReq: StR-001
  systemReq: REQ-F-001
```

---

## User Story

### As a... I want... So that...

```gherkin
As a [specific user role/persona]
I want to [perform some action/have some capability]
So that [achieve some business value/benefit]
```

### Example

```gherkin
As a registered user
I want to reset my password via email
So that I can regain access to my account if I forget my password
```

---

## Business Value

**Priority Justification**:
[Why is this story important? What business value does it provide?]

**User Impact**:
[How many users affected? How often will this feature be used?]

**Business Metrics**:

- Expected usage: [X times per day/week]
- User satisfaction impact: [High/Medium/Low]
- Revenue impact: [$X or None]
- Risk if not implemented: [High/Medium/Low]

---

## Acceptance Criteria

> These define "done" - when all criteria pass, the story is complete.

### Scenario 1: [Happy Path/Main Success Scenario]

```gherkin
Given [the initial context/preconditions]
And [additional context if needed]
When [the user performs this action]
Then [this should be the result]
And [additional expected outcomes]
```

### Example

```gherkin
Given I am on the login page
And I have forgotten my password
When I click "Forgot Password"
And I enter my email address "user@example.com"
And I click "Send Reset Link"
Then I should see a message "Password reset link sent to your email"
And I should receive an email with a password reset link
And the link should be valid for 24 hours
```

### Scenario 2: [Alternative Flow or Edge Case]

```gherkin
Given [different context]
When [action]
Then [expected result]
```

### Example

```gherkin
Given I am on the password reset page
And I entered an unregistered email address
When I click "Send Reset Link"
Then I should see a message "If that email exists, a reset link has been sent"
And no email should be sent (security: don't reveal if email exists)
```

### Scenario 3: [Error Handling]

```gherkin
Given [error condition]
When [action]
Then [error handling result]
```

### Example

```gherkin
Given I received a password reset link
And the link has expired (>24 hours old)
When I click on the expired link
Then I should see an error message "This reset link has expired"
And I should see a button "Request New Reset Link"
```

---

## Definition of Done (DoD)

> **XP Practice**: Clear definition of what "done" means ensures quality.

### Implementation Checklist

- [ ] **Code Written**: All functionality implemented per acceptance criteria
- [ ] **Tests Written First (TDD)**: All tests written before implementation
  - [ ] Unit tests (>80% coverage)
  - [ ] Integration tests
  - [ ] Acceptance tests (automated BDD)
- [ ] **Tests Passing**: All tests green
- [ ] **Code Reviewed**: Peer review completed (pair programming or formal review)
- [ ] **Refactored**: Code is clean, no duplication, follows standards
- [ ] **Integrated**: Merged into main branch
- [ ] **Deployed**: Deployed to staging environment
- [ ] **Documented**: User-facing documentation updated
- [ ] **Accepted**: Product Owner acceptance obtained

---

## Test Strategy

### Unit Tests (TDD)

**Test First! Write these tests BEFORE writing implementation code.**

```typescript
// Test example (write first!)
describe('PasswordResetService', () => {
  describe('requestPasswordReset', () => {
    it('should send reset email for valid registered email', async () => {
      // Arrange
      const email = 'user@example.com';
      const mockUserRepo = createMockUserRepo({ email });
      const mockEmailService = createMockEmailService();
      const service = new PasswordResetService(mockUserRepo, mockEmailService);
      
      // Act
      await service.requestPasswordReset(email);
      
      // Assert
      expect(mockEmailService.sendEmail).toHaveBeenCalledWith({
        to: email,
        subject: 'Password Reset',
        body: expect.stringContaining('reset link')
      });
    });
    
    it('should not reveal if email does not exist (security)', async () => {
      // Security test: don't leak information about registered emails
      const email = 'nonexistent@example.com';
      const mockUserRepo = createMockUserRepo({ emailExists: false });
      const service = new PasswordResetService(mockUserRepo);
      
      // Should return success message regardless
      const result = await service.requestPasswordReset(email);
      expect(result.message).toBe('If that email exists, a reset link has been sent');
    });
  });
});
```

### Integration Tests

```typescript
describe('Password Reset Integration', () => {
  let app: TestApp;
  let database: TestDatabase;
  let emailServer: MockEmailServer;
  
  beforeAll(async () => {
    database = await TestDatabase.create();
    emailServer = await MockEmailServer.start();
    app = await TestApp.create({ database, emailServer });
  });
  
  it('should complete full password reset workflow', async () => {
    // Create user in database
    const user = await database.users.create({
      email: 'test@example.com',
      password: 'OldPassword123!'
    });
    
    // Request password reset
    const response = await app.api.post('/api/auth/forgot-password', {
      email: 'test@example.com'
    });
    expect(response.status).toBe(200);
    
    // Verify email sent
    const emails = await emailServer.getReceivedEmails();
    expect(emails).toHaveLength(1);
    
    // Extract reset token from email
    const resetToken = extractResetToken(emails[0].body);
    
    // Use reset token to set new password
    const resetResponse = await app.api.post('/api/auth/reset-password', {
      token: resetToken,
      newPassword: 'NewPassword456!'
    });
    expect(resetResponse.status).toBe(200);
    
    // Verify can login with new password
    const loginResponse = await app.api.post('/api/auth/login', {
      email: 'test@example.com',
      password: 'NewPassword456!'
    });
    expect(loginResponse.status).toBe(200);
  });
});
```

### Acceptance Tests (BDD)

```gherkin
Feature: Password Reset
  As a user who has forgotten their password
  I want to reset my password via email
  So that I can regain access to my account

  Background:
    Given the system is running
    And a user exists with email "user@example.com"

  @critical @security
  Scenario: Successfully request password reset
    Given I am on the login page
    When I click "Forgot Password"
    And I enter email "user@example.com"
    And I click "Send Reset Link"
    Then I should see a success message
    And I should receive an email with a reset link
    And the reset link should be valid for 24 hours

  @security
  Scenario: Request reset for non-existent email (security - don't reveal)
    Given I am on the forgot password page
    When I enter email "nonexistent@example.com"
    And I click "Send Reset Link"
    Then I should see the same success message
    And no email should be sent
    And no error should reveal that the email doesn't exist

  @validation
  Scenario: Complete password reset successfully
    Given I received a valid password reset link
    When I click the reset link
    And I enter new password "NewSecure123!"
    And I confirm new password "NewSecure123!"
    And I click "Reset Password"
    Then I should see a success message "Password reset successfully"
    And I should be able to login with the new password

  @validation
  Scenario: Reject weak password
    Given I received a valid password reset link
    When I click the reset link
    And I enter new password "weak"
    And I click "Reset Password"
    Then I should see an error "Password must be at least 8 characters"
    And my password should not be changed

  @expiration
  Scenario: Expired reset link
    Given I received a password reset link 25 hours ago
    When I click the expired reset link
    Then I should see an error message "This reset link has expired"
    And I should see a button "Request New Reset Link"
```

---

## Technical Notes

### Implementation Hints

**Security Considerations**:

- Don't reveal if email exists or not (prevent email enumeration)
- Reset tokens must be cryptographically secure (crypto.randomBytes)
- Tokens should be single-use (mark as used after reset)
- Rate limit password reset requests (5 per hour per IP)
- Log all password reset attempts for security monitoring

**Performance Considerations**:

- Send emails asynchronously (don't block HTTP response)
- Cache user lookups (check cache before database)
- Token generation should be fast (< 50ms)

**Example Implementation**:

```typescript
class PasswordResetService {
  async requestPasswordReset(email: string): Promise<{message: string}> {
    // Look up user (cache first, then database)
    const user = await this.userRepository.findByEmail(email);
    
    if (user) {
      // Generate secure token
      const token = await this.generateSecureToken();
      const expiresAt = new Date(Date.now() + 24 * 60 * 60 * 1000); // 24 hours
      
      // Store token
      await this.resetTokenRepository.create({
        userId: user.id,
        token,
        expiresAt,
        used: false
      });
      
      // Send email asynchronously
      await this.emailQueue.publish({
        type: 'PASSWORD_RESET',
        to: email,
        token
      });
    }
    
    // Always return same message (security: don't reveal if email exists)
    return { message: 'If that email exists, a reset link has been sent' };
  }
  
  private async generateSecureToken(): Promise<string> {
    const bytes = await crypto.randomBytes(32);
    return bytes.toString('base64url');
  }
}
```

### Dependencies

**Required Stories** (must be completed first):

- STORY-XXX: User registration
- STORY-XXX: Email service integration

**Dependent Stories** (blocked by this story):

- STORY-XXX: Account security settings

**Related Stories**:

- STORY-XXX: Two-factor authentication
- STORY-XXX: Account lockout after failed attempts

---

## Estimation

### Story Points: 5

**Estimation Factors**:

- **Complexity**: Medium (crypto, security, email)
- **Uncertainty**: Low (well-understood problem)
- **Effort**: ~2-3 days for pair
- **Integration**: Medium (email service, database)

**Breakdown** (planning poker):

- Database schema: 1 point
- Token generation: 1 point
- Email integration: 2 points
- Security considerations: 1 point

### Time Box

**Sprint Duration**: 2 weeks  
**Target Completion**: Day 5 of sprint  
**Pair Programming**: Recommended (security-critical feature)

---

## Questions & Clarifications

### Open Questions (Ask Product Owner)

- [ ] Should reset links work only once, or multiple times within 24h?
- [ ] What happens if user requests multiple reset links?
- [ ] Should we notify user (via their registered email) when a reset is requested?
- [ ] Rate limiting: how many reset requests per hour/day per user/IP?

### Assumptions

- Email service is already integrated and working
- User has access to their registered email
- Email delivery is reliable (<5% failure rate acceptable)

### Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| Email service downtime | High | Queue emails, retry logic |
| Token generation collision | Medium | Use crypto-secure random, check uniqueness |
| User doesn't receive email | Medium | Clear error messages, resend option |

---

## Collaboration Notes

### Pair Programming

**Recommended Pairing**: Senior + Mid-level

**Roles**:

- **Driver**: Types the tests and implementation
- **Navigator**: Thinks about edge cases, security, and design

**Focus Areas**:

- Security (token generation, email enumeration)
- Test coverage (especially edge cases)
- Error handling

### Customer Involvement

**Product Owner Check-ins**:

- Day 1: Review acceptance criteria
- Day 3: Demo working feature in staging
- Day 5: Final acceptance

---

## Progress Tracking

### Task Breakdown

- [ ] Write acceptance tests (BDD)
- [ ] Design database schema for reset tokens
- [ ] Write unit tests for token generation
- [ ] Implement token generation (TDD)
- [ ] Write unit tests for password reset service
- [ ] Implement password reset service (TDD)
- [ ] Integrate with email service
- [ ] Write integration tests
- [ ] Manual testing in staging
- [ ] Security review
- [ ] Code review
- [ ] Documentation
- [ ] Product Owner acceptance

### Daily Progress

**Day 1**:

- [x] Acceptance tests written
- [x] Database schema designed

**Day 2**:

- [ ] Token generation implemented
- [ ] Password reset service implemented

**Day 3**:

- [ ] Integration complete
- [ ] All tests passing

---

## Retrospective Notes (After Completion)

**What Went Well**:

- [To be filled after story completion]

**What Could Be Improved**:

- [To be filled after story completion]

**Velocity Impact**:

- Estimated: 5 points
- Actual: [X points]
- Variance: [+/- X points]

**Lessons Learned**:

- [To be filled after story completion]

---

**Remember**: 
- ✅ Tests first (TDD)!
- ✅ Pair for complex/security-critical code
- ✅ Integrate frequently
- ✅ Keep it simple (YAGNI)
- ✅ Customer acceptance required for "done"
