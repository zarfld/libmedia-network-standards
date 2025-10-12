#!/usr/bin/env python3
"""
Standards-compliant template generator
Creates new spec files with proper ISO/IEC/IEEE compliance
Following XP practices and TDD approach
"""

import sys
import pathlib
from datetime import datetime
import argparse

TEMPLATES = {
    'requirements': {
        'dir': '02-requirements',
        'subdir': 'functional',
        'template': '''---
specType: requirements
standard: ISO/IEC/IEEE 29148:2018
phase: 02-requirements
version: 1.0.0
author: {author}
date: "{date}"
status: draft
traceability:
  stakeholderRequirements:
    - REQ-STK-{topic}-001
---

# {title} - Requirements Specification

> Per ISO/IEC/IEEE 29148:2018 Requirements Engineering processes

## User Stories (XP Practice)

### Epic: {title}

**As a** {stakeholder},  
**I need** {functionality},  
**So that** {benefit}.

**Acceptance Criteria:**
- [ ] {acceptance_criterion_1}
- [ ] {acceptance_criterion_2}
- [ ] {acceptance_criterion_3}

## Functional Requirements

### REQ-F-{topic_upper}-001: Core Functionality
**Priority**: Must Have  
**Rationale**: {rationale}  
**Dependencies**: None  

**Specification:**
{detailed_specification}

### REQ-F-{topic_upper}-002: Interface Requirements  
**Priority**: Must Have  
**Rationale**: Hardware abstraction per architecture standards  
**Dependencies**: REQ-F-{topic_upper}-001  

**Specification:**
System shall provide hardware-agnostic interfaces per ADR-001.

## Non-Functional Requirements

### REQ-NF-{topic_upper}-001: Performance Requirements
**Priority**: Must Have  
**Rationale**: Real-time media networking requirements  

**Specification:**
- Response time: < {response_time}ms
- Throughput: > {throughput} packets/second  
- Timing accuracy: ± {timing_accuracy}ns

### REQ-NF-{topic_upper}-002: Standards Compliance
**Priority**: Must Have  
**Rationale**: IEEE certification requirements  

**Specification:**
Implementation shall comply with {ieee_standard} specification.

## Verification Criteria

Following IEEE 1012-2016 V&V procedures:

| Requirement | Verification Method | Acceptance Criteria |
|-------------|-------------------|-------------------|
| REQ-F-{topic_upper}-001 | Unit Testing | All unit tests pass |
| REQ-F-{topic_upper}-002 | Integration Testing | Interface compliance verified |
| REQ-NF-{topic_upper}-001 | Performance Testing | Performance targets met |
| REQ-NF-{topic_upper}-002 | Standards Testing | IEEE compliance validated |

## Traceability Matrix

| Stakeholder Req | System Req | Test Case | Status |
|----------------|------------|-----------|--------|
| REQ-STK-{topic}-001 | REQ-F-{topic_upper}-001 | TC-{topic_upper}-001 | ⏳ Pending |
| REQ-STK-{topic}-001 | REQ-F-{topic_upper}-002 | TC-{topic_upper}-002 | ⏳ Pending |
'''
    },
    
    'architecture': {
        'dir': '03-architecture',
        'subdir': 'decisions',
        'template': '''---
specType: architecture
standard: ISO/IEC/IEEE 42010:2011
phase: 03-architecture
version: 1.0.0
author: {author}
date: "{date}"
status: draft
traceability:
  requirements:
    - REQ-F-001
    - REQ-NF-001
---

# ADR-{adr_number}: {title}

> Architecture Decision Record per ISO/IEC/IEEE 42010:2011

## Metadata
```yaml
adrId: ADR-{adr_number}
status: proposed
relatedRequirements:
  - REQ-F-{topic_upper}-001
  - REQ-NF-{topic_upper}-001
relatedComponents:
  - {component_name}
supersedes: []
supersededBy: null
author: {author}
date: {date}
reviewers: []
```

## Context

### Architectural Concern
{architectural_concern_description}

### Stakeholder Concerns
- **{stakeholder_1}**: {concern_1}
- **{stakeholder_2}**: {concern_2}
- **{stakeholder_3}**: {concern_3}

### Forces in Conflict
1. **{force_1}**: {force_1_description}
2. **{force_2}**: {force_2_description}
3. **{force_3}**: {force_3_description}

### Quality Attributes (ISO/IEC 25010)
- **{quality_1}**: {quality_1_requirement}
- **{quality_2}**: {quality_2_requirement}

## Decision

**We will {decision_statement}**

Key architectural choices:
1. {choice_1}
2. {choice_2}
3. {choice_3}

## Status
**PROPOSED** - This decision addresses {main_concern} while maintaining {key_constraint}.

## Rationale

### Why This Option Best Addresses Forces
1. **{rationale_1}**
2. **{rationale_2}**  
3. **{rationale_3}**

### Trade-off Analysis
- **Performance**: {performance_impact}
- **Maintainability**: {maintainability_impact}
- **Complexity**: {complexity_impact}

## Considered Alternatives

| Alternative | Summary | Pros | Cons | Reason Not Chosen |
|------------|---------|------|------|-------------------|
| {alt_1} | {alt_1_summary} | {alt_1_pros} | {alt_1_cons} | {alt_1_reason} |
| {alt_2} | {alt_2_summary} | {alt_2_pros} | {alt_2_cons} | {alt_2_reason} |

## Implementation Guidelines (IEEE 1016-2009)

### Design Principles
```cpp
// Example implementation pattern
class {ComponentName} {{
public:
    // {interface_description}
    virtual {return_type} {method_name}({parameters}) = 0;
}};
```

## Verification Criteria (IEEE 1012-2016)

### Architecture Compliance Tests
1. **{test_1}**: {test_1_description}
2. **{test_2}**: {test_2_description}

### Success Metrics
- ✅ {success_metric_1}
- ✅ {success_metric_2}
- ✅ {success_metric_3}

## Dependencies

### Upstream Dependencies
- **{upstream_dep}**: {upstream_description}

### Downstream Dependencies  
- **{downstream_dep}**: {downstream_description}

## Notes

This ADR follows ISO/IEC/IEEE 42010:2011 architecture description practices and integrates with XP development practices including TDD and continuous integration.
'''
    }
}

def generate_template(spec_type, title, author="Development Team", **kwargs):
    """Generate a standards-compliant specification template"""
    
    if spec_type not in TEMPLATES:
        raise ValueError(f"Unknown spec type: {spec_type}. Supported: {list(TEMPLATES.keys())}")
    
    template_info = TEMPLATES[spec_type]
    
    # Generate default values
    topic = title.lower().replace(' ', '-').replace('_', '-')
    topic_upper = topic.upper().replace('-', '_')
    date = datetime.now().strftime('%Y-%m-%d')
    
    # Merge provided kwargs with defaults
    template_vars = {
        'title': title,
        'author': author,
        'date': date,
        'topic': topic,
        'topic_upper': topic_upper,
        **kwargs
    }
    
    # Add spec-specific defaults
    if spec_type == 'requirements':
        defaults = {
            'stakeholder': 'system user',
            'functionality': 'core system capability',
            'benefit': 'system meets requirements',
            'acceptance_criterion_1': 'Functional requirement satisfied',
            'acceptance_criterion_2': 'Performance requirement met',
            'acceptance_criterion_3': 'Standards compliance verified',
            'rationale': 'Essential for system functionality',
            'detailed_specification': 'System shall implement required functionality per specification.',
            'response_time': '10',
            'throughput': '1000',
            'timing_accuracy': '100',
            'ieee_standard': 'IEEE 802.1AS-2021'
        }
    elif spec_type == 'architecture':
        defaults = {
            'adr_number': '001',
            'architectural_concern': 'Key architectural decision required for system design',
            'stakeholder_1': 'Standards Developers',
            'concern_1': 'Standards compliance and protocol correctness',
            'stakeholder_2': 'Hardware Integrators', 
            'concern_2': 'Multi-vendor compatibility and performance',
            'stakeholder_3': 'System Architects',
            'concern_3': 'Maintainability and testability',
            'force_1': 'Standards Compliance vs Performance',
            'force_1_description': 'Pure standards implementation may impact performance',
            'force_2': 'Portability vs Optimization',
            'force_2_description': 'Hardware abstraction adds overhead but enables portability',
            'force_3': 'Simplicity vs Flexibility',
            'force_3_description': 'Simple design may limit future extensibility',
            'quality_1': 'Maintainability',
            'quality_1_requirement': 'Code must be easily maintainable and testable',
            'quality_2': 'Performance',
            'quality_2_requirement': 'System must meet real-time performance requirements',
            'decision_statement': 'implement the proposed architectural solution',
            'choice_1': 'Hardware abstraction layer implementation',
            'choice_2': 'Standards-compliant interface design',
            'choice_3': 'TDD-driven development approach',
            'main_concern': 'the identified architectural concern',
            'key_constraint': 'standards compliance requirements',
            'rationale_1': 'Addresses stakeholder concerns effectively',
            'rationale_2': 'Maintains standards compliance',
            'rationale_3': 'Enables testable, maintainable implementation',
            'performance_impact': 'Minimal overhead acceptable for benefits gained',
            'maintainability_impact': 'Significantly improved through abstraction',
            'complexity_impact': 'Moderate increase justified by benefits',
            'alt_1': 'Alternative Approach A',
            'alt_1_summary': 'Different implementation approach',
            'alt_1_pros': 'Simpler implementation',
            'alt_1_cons': 'Limited flexibility',
            'alt_1_reason': 'Does not meet extensibility requirements',
            'alt_2': 'Alternative Approach B',
            'alt_2_summary': 'Another implementation option',
            'alt_2_pros': 'Higher performance',
            'alt_2_cons': 'Reduced maintainability',
            'alt_2_reason': 'Violates maintainability requirements',
            'ComponentName': 'ArchitecturalComponent',
            'interface_description': 'Core interface definition',
            'return_type': 'int',
            'method_name': 'execute_operation',
            'parameters': 'const Parameters& params',
            'test_1': 'Interface Compliance Test',
            'test_1_description': 'Verify interface implements required contracts',
            'test_2': 'Performance Validation Test',
            'test_2_description': 'Validate performance meets requirements',
            'success_metric_1': 'Interface compliance verified',
            'success_metric_2': 'Performance requirements met',
            'success_metric_3': 'Standards compliance validated',
            'upstream_dep': 'Requirements Specification',
            'upstream_description': 'Architectural decisions based on requirements',
            'downstream_dep': 'Implementation Components',
            'downstream_description': 'Components implement architectural decisions',
            'component_name': 'CoreArchitecturalComponent'
        }
    
    # Merge defaults with provided values
    for key, value in defaults.items():
        template_vars.setdefault(key, value)
    
    # Generate the content
    content = template_info['template'].format(**template_vars)
    
    # Create file path
    base_dir = pathlib.Path(template_info['dir'])
    if 'subdir' in template_info:
        base_dir = base_dir / template_info['subdir']
    
    filename = f"{topic}.md"
    filepath = base_dir / filename
    
    return filepath, content

def main():
    parser = argparse.ArgumentParser(description='Generate standards-compliant specification templates')
    parser.add_argument('type', choices=['requirements', 'architecture'], 
                       help='Type of specification to generate')
    parser.add_argument('title', help='Title of the specification')
    parser.add_argument('--author', default='Development Team', 
                       help='Author name')
    parser.add_argument('--output', help='Output file path (optional)')
    parser.add_argument('--dry-run', action='store_true', 
                       help='Show content without creating file')
    
    args = parser.parse_args()
    
    try:
        filepath, content = generate_template(args.type, args.title, args.author)
        
        if args.output:
            filepath = pathlib.Path(args.output)
        
        if args.dry_run:
            print(f"Would create: {filepath}")
            print("=" * 60)
            print(content)
        else:
            # Create directory if needed
            filepath.parent.mkdir(parents=True, exist_ok=True)
            
            # Write file
            filepath.write_text(content, encoding='utf-8')
            
            print(f"✅ Created standards-compliant {args.type} specification:")
            print(f"   📄 {filepath}")
            print(f"   📋 Following ISO/IEC/IEEE standards compliance")
            print(f"   🎯 Ready for XP TDD development")
            
            # Run validation
            print(f"\n🔍 Validating generated file...")
            import subprocess
            result = subprocess.run([
                sys.executable, 'Scripts/validate-spec-structure.py', str(filepath)
            ], capture_output=True, text=True)
            
            if result.returncode == 0:
                print("✅ Generated file passes standards validation")
            else:
                print("⚠️ Generated file has validation issues:")
                print(result.stdout)
                print(result.stderr)
                
    except Exception as e:
        print(f"❌ Error generating template: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()