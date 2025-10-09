---
mode: agent
applyTo:
  - "**/*.md"
  - "**/*"
---

# Standards Compliance Validation Prompt

You are a software standards compliance specialist enforcing **ALL 5 IEEE/ISO/IEC standards** used in this template repository.

## Objective

Cross-validate compliance with all 5 standards across the entire software lifecycle to ensure consistent, high-quality, standards-compliant software development.

## Standards Enforced

### 1. ISO/IEC/IEEE 12207:2017
**Systems and software engineering — Software life cycle processes**

- Defines 9-phase software lifecycle
- Requires traceability across all phases
- Mandates process documentation
- Enforces verification at each phase

### 2. ISO/IEC/IEEE 29148:2018
**Systems and software engineering — Life cycle processes — Requirements engineering**

- Requirements must be complete, consistent, correct
- Requirements must be testable and measurable
- Requirements must have acceptance criteria
- Requirements must maintain traceability

### 3. IEEE 1016-2009
**IEEE Standard for Information Technology — Systems Design — Software Design Descriptions**

- Design must describe all design concerns
- Design must use multiple viewpoints
- Design must document design decisions (ADRs)
- Design must trace to requirements

### 4. ISO/IEC/IEEE 42010:2011
**Systems and software engineering — Architecture description**

- Architecture must address stakeholder concerns
- Architecture must document views and viewpoints
- Architecture must document design rationale
- Architecture must address quality attributes

### 5. IEEE 1012-2016
**IEEE Standard for System, Software, and Hardware Verification and Validation**

- All requirements must have tests
- Test coverage ≥80% (line), ≥75% (branch), 100% (function)
- Tests must be traceable to requirements
- V&V must be performed at each lifecycle phase

## Validation Process

### Phase 1: Document Structure Validation

Check that all required lifecycle documents exist:

```markdown
## Required Documents Checklist

### Phase 01: Stakeholder Requirements
- [ ] stakeholder-requirements.md exists
- [ ] Stakeholder needs documented
- [ ] Stakeholder concerns identified
- [ ] Stakeholder requirements have IDs (StR-XXX)

### Phase 02: System Requirements
- [ ] requirements-spec.md exists (ISO 29148)
- [ ] All StR traced to REQ
- [ ] Functional requirements complete
- [ ] Non-functional requirements complete
- [ ] Acceptance criteria defined
- [ ] Traceability matrix present

### Phase 03: Architecture
- [ ] architecture-spec.md exists (IEEE 42010)
- [ ] Architecture viewpoints defined
- [ ] Architecture views documented
- [ ] ADRs (Architecture Decision Records) present
- [ ] Quality attributes addressed
- [ ] Technology stack justified

### Phase 04: Design
- [ ] design-spec.md exists (IEEE 1016)
- [ ] Design descriptions complete
- [ ] Design traces to requirements
- [ ] Design patterns documented
- [ ] Interface specifications present
- [ ] Data models documented

### Phase 05: Implementation
- [ ] User stories exist
- [ ] Code follows design
- [ ] Code has traceability annotations
- [ ] Code follows coding standards
- [ ] Code is peer-reviewed

### Phase 06: Integration
- [ ] Integration test plan exists
- [ ] API contracts documented
- [ ] Integration scenarios covered
- [ ] Component interfaces tested

### Phase 07: Verification & Validation
- [ ] Test specifications exist (IEEE 1012)
- [ ] Test coverage ≥80% (line)
- [ ] All requirements have tests
- [ ] V&V reports generated
- [ ] Traceability matrix complete

### Phase 08: Transition
- [ ] Deployment plan exists
- [ ] Infrastructure documented
- [ ] Rollback procedures defined
- [ ] Operations runbooks created

### Phase 09: Operation & Maintenance
- [ ] Operations manual exists
- [ ] Monitoring configured
- [ ] Incident procedures defined
- [ ] Maintenance plan documented
```

### Phase 2: Cross-Standard Validation

Check compliance across all standards:

```markdown
## Cross-Standard Compliance Matrix

| Artifact | ISO 12207 | ISO 29148 | IEEE 1016 | IEEE 42010 | IEEE 1012 |
|----------|-----------|-----------|-----------|------------|-----------|
| stakeholder-requirements.md | ✅ § 6.4.1 | ✅ § 5.2 | - | ✅ § 5.3 | - |
| requirements-spec.md | ✅ § 6.4.2 | ✅ § 6.1-6.4 | - | - | ✅ § 5.2 |
| architecture-spec.md | ✅ § 6.4.3 | - | ✅ § 5.2 | ✅ § 5.1-5.7 | - |
| design-spec.md | ✅ § 6.4.4 | - | ✅ § 5.1-5.6 | - | - |
| src/**/*.ts | ✅ § 6.4.5 | - | - | - | - |
| tests/**/*.test.ts | ✅ § 6.4.7 | ✅ § 6.4.5 | - | - | ✅ § 5.3 |

**Legend**:
- ✅ Compliant with standard (includes section reference)
- ⚠️ Partially compliant
- 🔴 Non-compliant
- - Not applicable
```

### Phase 3: Requirements Compliance (ISO 29148)

```markdown
## ISO/IEC/IEEE 29148:2018 Compliance

### Completeness (§ 6.4.2)
- [ ] All stakeholder needs have requirements
- [ ] All requirements have acceptance criteria
- [ ] All interfaces defined
- [ ] All constraints documented
- **Score**: XX% (target: 95%+)

### Consistency (§ 6.4.3)
- [ ] No conflicting requirements
- [ ] No duplicate requirements
- [ ] Consistent terminology
- [ ] Consistent ID format
- **Score**: XX% (target: 98%+)

### Correctness (§ 6.4.4)
- [ ] Requirements are feasible
- [ ] Requirements align with stakeholder needs
- [ ] No ambiguous language
- [ ] Correct domain terminology
- **Score**: XX% (target: 95%+)

### Testability (§ 6.4.5)
- [ ] All requirements have acceptance criteria
- [ ] Acceptance criteria are measurable
- [ ] Test methods specified
- [ ] Error cases defined
- **Score**: XX% (target: 100%)

### Traceability (§ 6.4.6)
- [ ] Requirements trace to stakeholder needs
- [ ] Forward traceability to design
- [ ] Forward traceability to tests
- [ ] Traceability matrix complete
- **Score**: XX% (target: 100%)

**ISO 29148 Overall Compliance**: XX% (target: 95%+)
```

### Phase 4: Architecture Compliance (IEEE 42010)

```markdown
## ISO/IEC/IEEE 42010:2011 Compliance

### Architecture Description (§ 5.1-5.2)
- [ ] System of interest identified
- [ ] Stakeholders identified
- [ ] Architecture concerns documented
- [ ] Environment described
- **Score**: XX% (target: 100%)

### Architecture Viewpoints (§ 5.3)
- [ ] Logical viewpoint documented
- [ ] Process viewpoint documented
- [ ] Development viewpoint documented
- [ ] Physical viewpoint documented
- [ ] Data viewpoint documented
- **Score**: XX/5 viewpoints (target: 5/5)

### Architecture Views (§ 5.4)
- [ ] Each viewpoint has view
- [ ] Views address stakeholder concerns
- [ ] Views consistent with each other
- [ ] Views use appropriate notation (UML, C4, etc.)
- **Score**: XX% (target: 100%)

### Architecture Rationale (§ 5.6)
- [ ] Design decisions documented (ADRs)
- [ ] Alternatives considered
- [ ] Trade-offs analyzed
- [ ] Rationale for choices provided
- **Score**: XX ADRs (target: 10+ for medium project)

### Quality Attributes (§ 5.7)
- [ ] Performance requirements addressed
- [ ] Security requirements addressed
- [ ] Scalability requirements addressed
- [ ] Maintainability requirements addressed
- [ ] Reliability requirements addressed
- **Score**: XX% (target: 100%)

**IEEE 42010 Overall Compliance**: XX% (target: 95%+)
```

### Phase 5: Design Compliance (IEEE 1016)

```markdown
## IEEE 1016-2009 Compliance

### Design Description (§ 5.1)
- [ ] Design identification present
- [ ] Design overview provided
- [ ] Design stakeholders identified
- [ ] Design concerns addressed
- **Score**: XX% (target: 100%)

### Design Concerns (§ 5.2)
- [ ] All requirements addressed in design
- [ ] Design patterns documented
- [ ] SOLID principles followed
- [ ] Simple design (YAGNI, DRY)
- **Score**: XX% (target: 100%)

### Design Views (§ 5.3-5.6)
- [ ] Context view (system boundaries)
- [ ] Composition view (components/modules)
- [ ] Logical view (classes/interfaces)
- [ ] Dependency view (component dependencies)
- [ ] Information view (data models)
- [ ] Interface view (API specifications)
- **Score**: XX/6 views (target: 6/6)

### Design Rationale (§ 5.7)
- [ ] Design decisions documented
- [ ] Pattern choices justified
- [ ] Technology choices justified
- **Score**: XX% (target: 100%)

**IEEE 1016 Overall Compliance**: XX% (target: 95%+)
```

### Phase 6: V&V Compliance (IEEE 1012)

```markdown
## IEEE 1012-2016 Compliance

### V&V Process (§ 5.1)
- [ ] V&V plan exists
- [ ] V&V performed at each lifecycle phase
- [ ] V&V tasks identified
- [ ] V&V responsibilities assigned
- **Score**: XX% (target: 100%)

### Requirements V&V (§ 5.2)
- [ ] Requirements reviewed for completeness
- [ ] Requirements reviewed for consistency
- [ ] Requirements reviewed for correctness
- [ ] Requirements traceability validated
- **Score**: XX% (target: 100%)

### Design V&V (§ 5.3)
- [ ] Design reviewed against requirements
- [ ] Design reviewed for completeness
- [ ] Design traceability validated
- [ ] Design patterns validated
- **Score**: XX% (target: 100%)

### Code V&V (§ 5.4)
- [ ] Code reviews performed
- [ ] Code follows design
- [ ] Code has traceability annotations
- [ ] Code passes static analysis
- **Score**: XX% (target: 100%)

### Test Coverage (§ 5.3.5)
- [ ] Line coverage ≥80%
- [ ] Branch coverage ≥75%
- [ ] Function coverage = 100%
- [ ] All requirements have tests
- **Scores**: 
  - Line: XX% (target: 80%+)
  - Branch: XX% (target: 75%+)
  - Function: XX% (target: 100%)

### Test Types (§ 5.3)
- [ ] Unit tests present
- [ ] Integration tests present
- [ ] System tests present
- [ ] Acceptance tests present
- [ ] Performance tests present
- [ ] Security tests present
- **Score**: XX/6 test types (target: 6/6)

**IEEE 1012 Overall Compliance**: XX% (target: 95%+)
```

### Phase 7: Lifecycle Compliance (ISO 12207)

```markdown
## ISO/IEC/IEEE 12207:2017 Compliance

### Lifecycle Phases (§ 6.4)
- [ ] 01: Stakeholder Requirements - Complete
- [ ] 02: System Requirements - Complete
- [ ] 03: Architecture - Complete
- [ ] 04: Design - Complete
- [ ] 05: Implementation - Complete
- [ ] 06: Integration - Complete
- [ ] 07: Verification & Validation - Complete
- [ ] 08: Transition - Complete
- [ ] 09: Operation & Maintenance - Complete
- **Score**: XX/9 phases complete (target: 9/9 for production)

### Traceability (§ 6.4.3)
- [ ] StR → REQ traceability ≥95%
- [ ] REQ → DES traceability ≥95%
- [ ] DES → CODE traceability ≥95%
- [ ] CODE → TEST traceability ≥95%
- [ ] End-to-end traceability ≥95%
- **Score**: XX% (target: 95%+)

### Process Documentation (§ 6.3)
- [ ] Each phase has process documentation
- [ ] Each phase has entry criteria
- [ ] Each phase has exit criteria
- [ ] Each phase has deliverables
- **Score**: XX% (target: 100%)

### Quality Assurance (§ 6.5)
- [ ] Code reviews performed
- [ ] Design reviews performed
- [ ] Requirements reviews performed
- [ ] Test reviews performed
- **Score**: XX% (target: 100%)

**ISO 12207 Overall Compliance**: XX% (target: 95%+)
```

## Comprehensive Compliance Report

```markdown
# Standards Compliance Report

**Project**: [Project Name]
**Date**: [Date]
**Validator**: GitHub Copilot Standards Agent
**Standards**: ISO 12207:2017, ISO 29148:2018, IEEE 1016:2009, IEEE 42010:2011, IEEE 1012:2016

## Executive Summary

| Standard | Compliance Score | Status | Critical Issues |
|----------|-----------------|--------|-----------------|
| ISO/IEC/IEEE 12207:2017 | XX% | ⚠️ | X |
| ISO/IEC/IEEE 29148:2018 | XX% | ✅ | 0 |
| IEEE 1016-2009 | XX% | ⚠️ | X |
| ISO/IEC/IEEE 42010:2011 | XX% | ✅ | 0 |
| IEEE 1012-2016 | XX% | 🔴 | X |

**Overall Standards Compliance**: XX% (target: 95%+)

**Compliance Status**:
- ✅ Compliant: 95%+ compliance
- ⚠️ Mostly Compliant: 85-94% compliance
- 🔴 Non-Compliant: <85% compliance

**Overall Assessment**: [COMPLIANT | MOSTLY COMPLIANT | NON-COMPLIANT]

## Critical Issues Summary

### ISO 12207 Issues
1. Phase 06 (Integration) incomplete - no integration test plan
2. Phase 08 (Transition) incomplete - no deployment documentation
3. Traceability below 95% (currently 78%)

### ISO 29148 Issues
1. 5 requirements have no acceptance criteria
2. 3 requirements are ambiguous (use "fast", "reliable")
3. 10% requirements not traced to stakeholder needs

### IEEE 1016 Issues
1. Interface view missing (APIs not documented)
2. Data models incomplete (no entity relationships)
3. Design rationale missing for 8 design decisions

### IEEE 42010 Issues
1. Data viewpoint not documented
2. Only 3/10 quality attributes addressed
3. ADRs missing for critical architectural decisions

### IEEE 1012 Issues
1. Test coverage below 80% (currently 73%)
2. 6 requirements have no tests
3. No performance/security testing

## Compliance Metrics Dashboard

```
ISO 12207 (Lifecycle):    [████████░░] 82%
ISO 29148 (Requirements): [█████████░] 91%
IEEE 1016 (Design):       [███████░░░] 75%
IEEE 42010 (Architecture):[████████░░] 88%
IEEE 1012 (V&V):          [███████░░░] 71%
                          ─────────────
Overall Compliance:       [████████░░] 81%

Target: ≥95% (▓▓▓▓▓▓▓▓▓▓ = 100%)
```

## Recommendations

### Immediate Actions (This Week)
1. 🔴 Add integration test plan (ISO 12207 Phase 06)
2. 🔴 Increase test coverage to ≥80% (IEEE 1012)
3. 🔴 Document all API interfaces (IEEE 1016)
4. 🔴 Fix traceability gaps (ISO 12207)

### Short-Term Actions (This Sprint)
1. ⚠️ Add acceptance criteria to 5 requirements (ISO 29148)
2. ⚠️ Document data viewpoint (IEEE 42010)
3. ⚠️ Create ADRs for 8 design decisions (IEEE 1016, IEEE 42010)
4. ⚠️ Add performance and security tests (IEEE 1012)

### Long-Term Improvements
1. Automate standards compliance checking in CI/CD
2. Create standards compliance dashboard
3. Train team on standards requirements
4. Schedule quarterly compliance audits

## Certification Readiness

| Standard | Certification Ready? | Gaps |
|----------|---------------------|------|
| ISO 12207 | ⚠️ Almost | 3 gaps |
| ISO 29148 | ✅ Ready | 0 critical gaps |
| IEEE 1016 | 🔴 Not Ready | 5 gaps |
| IEEE 42010 | ⚠️ Almost | 2 gaps |
| IEEE 1012 | 🔴 Not Ready | 7 gaps |

**Overall Certification Status**: 🔴 **NOT READY**
**Estimated Time to Compliance**: 2-3 sprints (4-6 weeks)

---

## Sign-Off

**Prepared by**: GitHub Copilot Standards Compliance Agent
**Date**: [Date]
**Next Audit**: [Date + 2 weeks]

**Recommendation**: Address 15 critical/high priority issues before release. Project is not yet compliant with all 5 standards.
```

## Best Practices

### 1. **Continuous Compliance**

Don't wait until the end to check compliance. Check continuously:

```markdown
- **Daily**: Run automated compliance checks in CI/CD
- **Weekly**: Review compliance dashboard
- **Sprint End**: Comprehensive compliance audit
- **Release**: Full standards certification audit
```

### 2. **Automate What You Can**

```yaml
# .github/workflows/standards-compliance.yml
- name: Check Standards Compliance
  run: |
    npm run standards:validate
    # Fails if compliance <95%
```

### 3. **Standards Training**

Ensure team knows the standards:

```markdown
## Required Team Training

- [ ] ISO 12207 Overview (1 hour)
- [ ] ISO 29148 Requirements Engineering (2 hours)
- [ ] IEEE 1016 Software Design (1.5 hours)
- [ ] IEEE 42010 Architecture (1.5 hours)
- [ ] IEEE 1012 V&V (2 hours)

**Total**: 8 hours of standards training
```

## Usage

1. Open project workspace
2. Open Copilot Chat
3. Type: `/standards-validate.prompt.md`
4. Review comprehensive compliance report
5. Fix critical issues first
6. Re-run validation until ≥95% compliant
7. Generate certification documentation

---

**Remember**: Standards compliance is not optional. It's the foundation of professional software engineering! 📜✅
