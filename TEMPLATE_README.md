# Copilot Instructions Template

**Standards-compliant software development lifecycle template with IEEE/ISO standards and Extreme Programming practices**

## ⭐ Features

- ✅ Complete 9-phase software lifecycle (ISO/IEC/IEEE 12207:2017)
- ✅ IEEE/ISO standards integration (29148, 1016, 42010, 1012)
- ✅ 12 Extreme Programming practices (TDD, CI, Pair Programming, etc.)
- ✅ Phase-specific GitHub Copilot instructions
- ✅ Spec-Kit templates for requirements, architecture, user stories
- ✅ Automated CI/CD with quality gates
- ✅ Requirements traceability enforcement
- ✅ Comprehensive documentation (4,500+ lines)

## 🚀 Quick Start

1. **Use this template**: Click "Use this template" button above
2. **Clone your new repository**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/YOUR_PROJECT.git
   cd YOUR_PROJECT
   ```
3. **Navigate to a phase** (e.g., requirements):
   ```bash
   cd 02-requirements
   ```
4. **Start working** - GitHub Copilot automatically loads phase-specific instructions!

## 📚 Documentation

- [Lifecycle Guide](docs/lifecycle-guide.md) - Complete walkthrough of all 9 phases
- [XP Practices Guide](docs/xp-practices.md) - Detailed guide to Extreme Programming
- [Copilot Usage Guide](docs/copilot-usage.md) - Maximize Copilot effectiveness
- [Documentation Overview](docs/README.md) - Navigate all documentation

## 🎯 What You Get

### Standards Compliance
- **ISO/IEC/IEEE 12207:2017** - Software life cycle processes
- **ISO/IEC/IEEE 29148:2018** - Requirements engineering
- **IEEE 1016-2009** - Software design descriptions
- **ISO/IEC/IEEE 42010:2011** - Architecture description
- **IEEE 1012-2016** - Verification and validation

### XP Practices Integrated
- Test-Driven Development (TDD)
- Continuous Integration
- Pair Programming
- Simple Design & YAGNI
- Refactoring
- Collective Code Ownership
- Coding Standards
- Sustainable Pace
- On-Site Customer
- Small Releases
- Metaphor
- Acceptance Testing

### GitHub Copilot Support
- Phase-specific context loading
- Automatic standards enforcement
- Template-driven development
- Clarifying questions framework

## 📁 Repository Structure

```
copilot-instructions-template/
├── .github/
│   ├── copilot-instructions.md          # Root Copilot instructions
│   └── workflows/
│       └── ci-standards-compliance.yml  # CI/CD pipeline
├── 01-stakeholder-requirements/         # Phase 01
├── 02-requirements/                     # Phase 02
├── 03-architecture/                     # Phase 03
├── 04-design/                          # Phase 04
├── 05-implementation/                   # Phase 05
├── 06-integration/                      # Phase 06
├── 07-verification-validation/          # Phase 07
├── 08-transition/                       # Phase 08
├── 09-operation-maintenance/            # Phase 09
├── spec-kit-templates/                  # Reusable templates
│   ├── requirements-spec.md
│   ├── architecture-spec.md
│   └── user-story-template.md
├── docs/                                # Documentation
│   ├── lifecycle-guide.md
│   ├── xp-practices.md
│   ├── copilot-usage.md
│   └── README.md
└── standards-compliance/                # Compliance tracking
    ├── checklists/
    ├── reviews/
    └── metrics/
```

## 🎓 Who Should Use This

- **Product Owners** - Structure requirements with IEEE 29148
- **Software Architects** - Document architecture with IEEE 42010
- **Developers** - Implement with TDD and XP practices
- **QA Engineers** - Verify and validate with IEEE 1012
- **DevOps Engineers** - CI/CD with automated quality gates
- **Teams** - Requiring standards compliance (aerospace, medical, finance, etc.)

## 💡 Example Workflow

```bash
# 1. Create user story
cd 02-requirements/user-stories
cp ../../spec-kit-templates/user-story-template.md STORY-001-user-login.md
code STORY-001-user-login.md  # Copilot helps!

# 2. Design architecture
cd ../../03-architecture
cp ../spec-kit-templates/architecture-spec.md architecture-description.md
code architecture-description.md  # Copilot suggests ADRs and diagrams

# 3. Implement with TDD
cd ../05-implementation
# Write test first
code tests/auth.test.ts  # Copilot suggests test cases
npm test  # RED

# Write implementation
code src/auth.ts  # Copilot suggests implementation
npm test  # GREEN

# 4. Push (CI/CD runs automatically)
git add .
git commit -m "feat: user authentication (TDD, closes STORY-001)"
git push  # CI runs tests, checks coverage, validates standards
```

## 📊 Quality Metrics

| Metric | Target | Automated Check |
|--------|--------|-----------------|
| Test Coverage | ≥80% | ✅ CI/CD |
| Cyclomatic Complexity | ≤10 | ✅ CI/CD |
| Requirements Traceability | 100% | ✅ CI/CD |
| Architecture Views | 5 views | ✅ CI/CD |
| Build Success Rate | >95% | ✅ CI/CD |

## 🔧 Customization

1. **Add domain-specific guidance** - Edit phase-specific `copilot-instructions.md`
2. **Create new templates** - Add to `spec-kit-templates/`
3. **Modify quality gates** - Edit `.github/workflows/ci-standards-compliance.yml`
4. **Add standards** - Update documentation in `docs/`

## 🤝 Contributing

Contributions welcome! Please:
1. Follow the same standards this template enforces
2. Update documentation
3. Add tests for new features
4. Submit PR with clear description

## 📝 License

[Your chosen license - MIT, Apache 2.0, etc.]

## 🙏 Acknowledgments

Built with:
- IEEE/ISO/IEC Software Engineering Standards
- Extreme Programming practices (Kent Beck)
- GitHub Copilot
- GitHub Spec-Kit

## 📧 Support

- **Issues**: Use GitHub Issues for bugs/feature requests
- **Discussions**: Use GitHub Discussions for questions
- **Documentation**: Check `docs/` folder

---

**Ready to build standards-compliant software with AI assistance?** 🚀

Use this template and let GitHub Copilot guide you through the complete software development lifecycle!
