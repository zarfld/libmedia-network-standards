---
mode: agent
---

# Specification Linting and Optimization

Optimize specification files for clarity, conciseness, and AI coding agent comprehension.

## Objectives

- **Clarity**: Make specifications unambiguous and easy to understand
- **Conciseness**: Remove redundancy while preserving all important details
- **Consistency**: Standardize terminology and structure
- **AI-Friendliness**: Optimize for GitHub Copilot and other AI coding agents

## Linting Rules

### 1. Language as Programming Language

Treat English/Markdown as a programming language:

- Use consistent terminology (avoid synonyms)
- Example: Choose ONE term and stick to it
  - ❌ pull/get/fetch/retrieve
  - ✅ pull (consistently)
- Define terms once, reference thereafter
- Use precise, technical language

### 2. Remove Duplication

- Eliminate repeated content
- Use references instead of copying
  - ✅ "See [User Authentication](#user-authentication)"
  - ❌ Repeating entire authentication section
- Consolidate similar requirements
- Extract common patterns to shared sections

### 3. Minimize Synonyms

Standardize vocabulary across the specification:

**Common Synonym Groups** (pick ONE per project):
- obtain/get/retrieve/fetch → **get**
- remove/delete/erase → **delete**
- create/make/generate/build → **create**
- update/modify/change/edit → **update**
- validate/verify/check → **validate**

### 4. Preserve Important Details

While optimizing, DO NOT remove:
- Technical requirements
- Acceptance criteria
- Traceability IDs (REQ-XXX, StR-XXX, etc.)
- Quality attributes
- Constraints
- Design decisions (ADRs)

### 5. Structural Consistency

- Use consistent heading levels
- Maintain uniform section structure
- Apply consistent formatting
- Standardize lists (all bullets OR all numbered)
- Use consistent code block languages

### 6. Remove Filler Words

Eliminate unnecessary words:

❌ Wordy:
```markdown
The system shall be able to provide the capability to allow users 
to perform authentication using their email address and password.
```

✅ Concise:
```markdown
The system shall authenticate users via email and password.
```

### 7. Active Voice

Prefer active voice over passive voice:

- ❌ "The data will be validated by the system"
- ✅ "The system validates the data"

### 8. Imperative Mood

Use imperative mood for instructions:

- ❌ "The developer should implement..."
- ✅ "Implement..."
- ❌ "It is recommended to use..."
- ✅ "Use..."

## Apply To

This prompt applies to specification files:

```
applyTo:
  - "**/*-spec.md"
  - "**/*-specification.md"
  - "**/requirements-*.md"
  - "**/architecture-*.md"
  - "**/design-*.md"
  - "**/main.md"
  - "**/spec.md"
```

## Linting Process

### 1. Analyze Terminology

- Identify all synonyms used
- Map to canonical terms
- Replace consistently throughout

### 2. Find Duplication

- Locate repeated content
- Consolidate or create references
- Use "See..." links

### 3. Simplify Language

- Remove filler words
- Shorten verbose phrases
- Use active voice
- Use imperative mood

### 4. Verify Completeness

After optimization, ensure:
- All requirements remain
- Traceability intact
- Acceptance criteria present
- Technical details preserved

### 5. Validate Structure

- Consistent heading hierarchy
- Uniform formatting
- Standard section order
- Clear navigation

## Example: Before vs. After

### Before (Verbose, Inconsistent)

```markdown
## User Login Feature

### Overview

This section describes the feature that will allow users to be able 
to perform authentication into the system using their email address 
and password credentials.

### Functional Requirements

REQ-F-001: The system should provide the capability for users to enter 
their email address into a text input field.

REQ-F-002: The system should provide the capability for users to enter 
their password into a password input field.

REQ-F-003: The system should be able to validate that the email address 
is in a valid format before attempting authentication.

REQ-F-004: The system should be able to validate that the password meets 
the minimum security requirements before attempting authentication.

REQ-F-005: The system should be able to retrieve the user record from 
the database based on the email address provided.

REQ-F-006: The system should be able to compare the provided password 
with the stored password hash using bcrypt hashing algorithm.

REQ-F-007: If authentication is successful, the system should create a 
session token and store it in the database.

REQ-F-008: If authentication fails, the system should display an error 
message to the user indicating that the credentials are invalid.
```

### After (Concise, Consistent)

```markdown
## User Login

Authenticate users via email and password.

### Requirements

**REQ-F-001**: User Login Form
- Email input field (validated format)
- Password input field (minimum security requirements)
- Submit button
- Traces to: StR-003

**REQ-F-002**: Authentication Process
1. Validate email format
2. Validate password requirements
3. Retrieve user record by email
4. Compare password with stored bcrypt hash
5. On success: Create session token, redirect to dashboard
6. On failure: Display "Invalid credentials" error
- Traces to: StR-003

### Acceptance Criteria

Given a registered user with email "user@example.com"
When they enter correct email and password
Then they are authenticated and redirected to dashboard
And a session token is created

Given invalid credentials
When user attempts to login
Then display "Invalid credentials" error
And do not create session token
```

**Improvements**:
- Reduced from ~250 words to ~100 words
- Eliminated "should provide the capability" repetition
- Consolidated related requirements
- Removed passive voice
- Added Given-When-Then acceptance criteria
- Maintained all technical details
- Improved readability

## What NOT to Modify

When linting specifications:

❌ **Do NOT modify**:
- Actual code files (only optimize Markdown)
- Traceability IDs
- Acceptance criteria specifics
- Technical constraints
- Design decisions
- This prompt itself

✅ **DO modify**:
- Verbose language
- Duplicate content
- Inconsistent terminology
- Passive voice
- Section structure
- Formatting

## Usage

To lint a specification file:

1. Open the specification file in your editor
2. Use GitHub Copilot Chat or invoke this prompt
3. Ask: "Lint this specification for clarity and conciseness"
4. Review the suggested changes
5. Accept changes that preserve all requirements
6. Test that Copilot can still understand the spec

## Integration with Development Workflow

### When to Lint

- After initial specification draft
- Before specification review
- When specification grows large (>500 lines)
- When team reports specification confusion
- Before major releases
- During retrospectives

### Quality Gates

Specifications should pass these checks:

- ✅ No term appears with >2 synonyms
- ✅ No section duplicated verbatim
- ✅ Active voice usage >80%
- ✅ Average sentence length <20 words
- ✅ All requirements have IDs
- ✅ All requirements have acceptance criteria
- ✅ Traceability links intact

## Benefits

Optimized specifications provide:

1. **Faster AI Comprehension**: Copilot understands intent quickly
2. **Reduced Context Length**: Shorter specs fit in context window
3. **Fewer Errors**: Clear specs → accurate code generation
4. **Easier Maintenance**: Concise specs easier to update
5. **Better Collaboration**: Team understands specifications faster
6. **Consistent Output**: Standardized terms → consistent code

## Automated Linting (Future Enhancement)

Consider adding automated linting:

```yaml
# .github/workflows/lint-specifications.yml
name: Lint Specifications

on:
  pull_request:
    paths:
      - '**/*-spec.md'
      - '**/*-specification.md'

jobs:
  lint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Check terminology consistency
        run: |
          # Check for synonym groups
          # Fail if inconsistent terminology detected
      
      - name: Check for duplication
        run: |
          # Detect repeated content
          # Warn if duplication exceeds threshold
      
      - name: Validate structure
        run: |
          # Check heading hierarchy
          # Verify required sections present
```

---

**Remember**: The goal is clarity and precision, not just brevity. A slightly longer but crystal-clear specification is better than an ambiguous short one.
