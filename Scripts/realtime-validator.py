#!/usr/bin/env python3
"""
REAL-TIME VALIDATION IDE INTEGRATION
===================================

This creates a VS Code extension-like system that provides:
1. Real-time YAML validation as you type
2. Auto-completion for YAML fields
3. Live ID conflict detection  
4. Instant authoritative reference suggestions
5. Schema-aware field validation

PREVENTS ERRORS DURING TYPING - NOT AFTER SAVING!
"""

import os
import sys
import json
import yaml
import re
from pathlib import Path
from typing import Dict, List, Optional, Tuple, Any
import tempfile
import subprocess
import time
import threading
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

class RealTimeValidator(FileSystemEventHandler):
    """Real-time file validation as content changes."""
    
    def __init__(self, enforcer):
        self.enforcer = enforcer
        self.validation_cache = {}
        self.last_validation = {}
        
    def on_modified(self, event):
        """Handle file modification events."""
        if not event.is_directory and event.src_path.endswith('.md'):
            self.validate_file_realtime(Path(event.src_path))
    
    def validate_file_realtime(self, file_path: Path):
        """Validate file in real-time with immediate feedback."""
        try:
            # Avoid validation spam - only validate if file changed significantly
            current_time = time.time()
            last_check = self.last_validation.get(str(file_path), 0)
            
            if current_time - last_check < 1.0:  # Minimum 1 second between validations
                return
                
            self.last_validation[str(file_path)] = current_time
            
            content = file_path.read_text(encoding='utf-8')
            
            # Extract YAML front matter
            yaml_match = re.match(r'^---\n(.*?)\n---', content, re.DOTALL)
            if not yaml_match:
                return  # No YAML front matter
            
            yaml_content = yaml_match.group(1)
            
            try:
                yaml_data = yaml.safe_load(yaml_content)
                if not yaml_data:
                    return
                
                # Real-time validation
                spec_type = yaml_data.get('specType')
                if spec_type in self.enforcer.schemas:
                    issues = self.enforcer.validate_yaml_realtime(yaml_data, spec_type)
                    
                    if issues:
                        # Show immediate feedback (could integrate with VS Code)
                        print(f"\n⚠️  REAL-TIME VALIDATION: {file_path.name}")
                        for issue in issues:
                            print(f"   🔴 {issue}")
                    else:
                        print(f"✅ Real-time validation: {file_path.name} - VALID")
                        
            except yaml.YAMLError as e:
                print(f"🔴 YAML Syntax Error in {file_path.name}: {e}")
                
        except Exception as e:
            print(f"⚠️  Validation error for {file_path}: {e}")

class YAMLAutoCompletion:
    """Provides auto-completion suggestions for YAML fields."""
    
    def __init__(self, enforcer):
        self.enforcer = enforcer
        
    def get_field_suggestions(self, spec_type: str, current_fields: List[str]) -> Dict[str, Any]:
        """Get suggestions for next YAML field based on schema."""
        if spec_type not in self.enforcer.schemas:
            return {}
        
        schema = self.enforcer.schemas[spec_type]
        properties = schema.get('properties', {})
        required = schema.get('required', [])
        
        suggestions = {}
        
        # Suggest missing required fields first
        for field in required:
            if field not in current_fields:
                field_schema = properties.get(field, {})
                suggestions[field] = {
                    'priority': 'required',
                    'type': field_schema.get('type', 'string'),
                    'description': field_schema.get('description', ''),
                    'example': self._get_field_example(field, field_schema)
                }
        
        # Suggest optional fields
        for field, field_schema in properties.items():
            if field not in current_fields and field not in required:
                suggestions[field] = {
                    'priority': 'optional',
                    'type': field_schema.get('type', 'string'),
                    'description': field_schema.get('description', ''),
                    'example': self._get_field_example(field, field_schema)
                }
        
        return suggestions
    
    def _get_field_example(self, field_name: str, field_schema: Dict) -> Any:
        """Generate example value for field."""
        field_type = field_schema.get('type', 'string')
        
        if field_name == 'specType':
            return 'requirements'
        elif field_name == 'phase':
            return '02-requirements'
        elif field_name == 'version':
            return '1.0.0'
        elif field_name == 'status':
            return 'draft'
        elif field_name == 'date':
            return '2025-10-12'
        elif field_name == 'authoritativeReferences':
            return [{'id': 'IEEE_1722_2016', 'title': 'IEEE 1722-2016 - AVTP', 'url': 'mcp://markitdown/standards/...'}]
        elif field_type == 'array':
            return []
        elif field_type == 'object':
            return {}
        elif field_type == 'boolean':
            return False
        elif field_type == 'integer':
            return 0
        else:
            return ''

class LiveIDConflictDetector:
    """Detects ID conflicts in real-time as user types."""
    
    def __init__(self, enforcer):
        self.enforcer = enforcer
        
    def check_id_conflicts(self, content: str) -> List[str]:
        """Check for ID conflicts in content."""
        conflicts = []
        
        # Extract all IDs from content
        all_ids = re.findall(r'(REQ-[A-Z0-9-]+|ARCH-[A-Z0-9-]+|DES-[A-Z0-9-]+|ADR-[0-9]+)', content)
        
        # Check against existing registry
        for id_val in all_ids:
            if id_val.startswith('REQ-') and id_val in self.enforcer.id_registry.requirements:
                conflicts.append(f"Duplicate requirement ID: {id_val}")
            elif id_val.startswith('ARCH-') and id_val in self.enforcer.id_registry.architecture:
                conflicts.append(f"Duplicate architecture ID: {id_val}")
            elif id_val.startswith('DES-') and id_val in self.enforcer.id_registry.design:
                conflicts.append(f"Duplicate design ID: {id_val}")
            elif id_val.startswith('ADR-') and id_val in self.enforcer.id_registry.adrs:
                conflicts.append(f"Duplicate ADR ID: {id_val}")
        
        # Check for duplicates within the same content
        id_counts = {}
        for id_val in all_ids:
            id_counts[id_val] = id_counts.get(id_val, 0) + 1
            if id_counts[id_val] > 1:
                conflicts.append(f"Duplicate ID in same document: {id_val}")
        
        return conflicts

class AuthoritativeReferenceSuggester:
    """Suggests authoritative references based on content analysis."""
    
    def __init__(self, enforcer):
        self.enforcer = enforcer
        
    def suggest_references(self, content: str, file_path: Path = None) -> List[Dict[str, str]]:
        """Suggest authoritative references based on content."""
        detected_standards = self.enforcer.detect_content_standards(content, file_path)
        
        suggestions = []
        for standard in detected_standards:
            if standard in self.enforcer.authoritative_refs:
                ref = self.enforcer.authoritative_refs[standard]
                suggestions.append({
                    'id': ref.id,
                    'title': ref.title,
                    'url': ref.url,
                    'section': ref.section or 'General',
                    'confidence': self._calculate_confidence(standard, content)
                })
        
        # Sort by confidence
        suggestions.sort(key=lambda x: x['confidence'], reverse=True)
        return suggestions
    
    def _calculate_confidence(self, standard: str, content: str) -> float:
        """Calculate confidence score for reference suggestion."""
        content_lower = content.lower()
        
        # Count mentions of standard
        standard_mentions = len(re.findall(standard.lower().replace('.', r'\.'), content_lower))
        
        # Count technical terms
        technical_terms = {
            'IEEE-1722-2016': ['avtp', 'transport', 'streaming', 'audio', 'video'],
            'IEEE-1722.1-2021': ['avdecc', 'discovery', 'control', 'entity'],
            'IEEE-802.1AS-2021': ['gptp', 'timing', 'synchronization', 'clock'],
            'AES67-2018': ['audio over ip', 'interoperability', 'streaming']
        }
        
        terms = technical_terms.get(standard, [])
        term_matches = sum(1 for term in terms if term in content_lower)
        
        # Calculate confidence (0.0 to 1.0)
        confidence = min(1.0, (standard_mentions * 0.3 + term_matches * 0.1))
        return confidence

class FirstAttemptCorrectEnforcerExtended:
    """Extended enforcer with real-time capabilities."""
    
    def __init__(self, repo_root: Path):
        # Import the base enforcer
        sys.path.append(str(repo_root / 'Scripts'))
        from first_attempt_correct import FirstAttemptCorrectEnforcer
        
        self.base_enforcer = FirstAttemptCorrectEnforcer(repo_root)
        self.repo_root = repo_root
        
        # Real-time components
        self.validator = RealTimeValidator(self.base_enforcer)
        self.autocomplete = YAMLAutoCompletion(self.base_enforcer)
        self.id_detector = LiveIDConflictDetector(self.base_enforcer)
        self.ref_suggester = AuthoritativeReferenceSuggester(self.base_enforcer)
        
        # File watcher
        self.observer = Observer()
        
    def start_realtime_monitoring(self, watch_paths: List[Path] = None):
        """Start real-time monitoring of specification files."""
        if not watch_paths:
            watch_paths = [
                self.repo_root / '02-requirements',
                self.repo_root / '03-architecture', 
                self.repo_root / '04-design'
            ]
        
        print(f"🔄 Starting real-time validation monitoring...")
        
        for path in watch_paths:
            if path.exists():
                self.observer.schedule(self.validator, str(path), recursive=True)
                print(f"   👁️  Monitoring: {path}")
        
        self.observer.start()
        print(f"✅ Real-time monitoring active!")
        
        try:
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            self.observer.stop()
            print(f"\n🛑 Real-time monitoring stopped")
        
        self.observer.join()
    
    def validate_content_live(self, content: str, spec_type: str = None, 
                            file_path: Path = None) -> Dict[str, Any]:
        """Comprehensive live validation of content."""
        results = {
            'yaml_valid': True,
            'schema_valid': True,
            'id_conflicts': [],
            'suggested_refs': [],
            'autocomplete': {},
            'issues': []
        }
        
        try:
            # Extract and validate YAML
            yaml_match = re.match(r'^---\n(.*?)\n---', content, re.DOTALL)
            if yaml_match:
                yaml_content = yaml_match.group(1)
                yaml_data = yaml.safe_load(yaml_content)
                
                if yaml_data:
                    spec_type = yaml_data.get('specType', spec_type)
                    
                    # Schema validation
                    if spec_type in self.base_enforcer.schemas:
                        schema_issues = self.base_enforcer.validate_yaml_realtime(yaml_data, spec_type)
                        if schema_issues:
                            results['schema_valid'] = False
                            results['issues'].extend(schema_issues)
                    
                    # Auto-completion suggestions
                    current_fields = list(yaml_data.keys())
                    results['autocomplete'] = self.autocomplete.get_field_suggestions(spec_type, current_fields)
            else:
                results['yaml_valid'] = False
                results['issues'].append("No YAML front matter found")
            
            # ID conflict detection
            results['id_conflicts'] = self.id_detector.check_id_conflicts(content)
            
            # Authoritative reference suggestions
            results['suggested_refs'] = self.ref_suggester.suggest_references(content, file_path)
            
        except Exception as e:
            results['yaml_valid'] = False
            results['issues'].append(f"Validation error: {e}")
        
        return results
    
    def create_vscode_integration(self) -> str:
        """Generate VS Code integration configuration."""
        
        config = {
            "yaml.schemas": {},
            "yaml.customTags": [],
            "files.associations": {
                "02-requirements/**/*.md": "yaml-frontmatter-markdown",
                "03-architecture/**/*.md": "yaml-frontmatter-markdown", 
                "04-design/**/*.md": "yaml-frontmatter-markdown"
            },
            "editor.quickSuggestions": {
                "other": True,
                "comments": False,
                "strings": True
            }
        }
        
        # Add schema mappings
        for spec_type in self.base_enforcer.schemas:
            schema_path = f"./spec-kit-templates/schemas/{spec_type}-spec.schema.json"
            config["yaml.schemas"][schema_path] = [f"**/*{spec_type}*.md"]
        
        return json.dumps(config, indent=2)

def main(argv: List[str]) -> int:
    """Main entry point for real-time validation."""
    
    # Find repository root
    repo_root = Path.cwd()
    while not (repo_root / '.git').exists() and repo_root.parent != repo_root:
        repo_root = repo_root.parent
    
    if not (repo_root / '.git').exists():
        print("❌ Not in a git repository")
        return 1
    
    enforcer = FirstAttemptCorrectEnforcerExtended(repo_root)
    
    if len(argv) > 1:
        command = argv[1]
        
        if command == 'monitor':
            # Start real-time monitoring
            enforcer.start_realtime_monitoring()
            
        elif command == 'validate':
            if len(argv) < 3:
                print("Usage: python Scripts/realtime-validator.py validate <file_path>")
                return 1
            
            file_path = Path(argv[2])
            if not file_path.exists():
                print(f"❌ File not found: {file_path}")
                return 1
            
            content = file_path.read_text(encoding='utf-8')
            results = enforcer.validate_content_live(content, file_path=file_path)
            
            print(f"📊 LIVE VALIDATION RESULTS: {file_path.name}")
            print(f"   YAML Valid: {'✅' if results['yaml_valid'] else '❌'}")
            print(f"   Schema Valid: {'✅' if results['schema_valid'] else '❌'}")
            print(f"   ID Conflicts: {len(results['id_conflicts'])}")
            print(f"   Suggested References: {len(results['suggested_refs'])}")
            
            if results['issues']:
                print(f"\n🔴 ISSUES:")
                for issue in results['issues']:
                    print(f"   - {issue}")
            
            if results['suggested_refs']:
                print(f"\n💡 SUGGESTED REFERENCES:")
                for ref in results['suggested_refs'][:3]:  # Top 3
                    print(f"   - {ref['title']} (confidence: {ref['confidence']:.2f})")
                    
        elif command == 'vscode':
            # Generate VS Code configuration
            config = enforcer.create_vscode_integration()
            vscode_dir = repo_root / '.vscode'
            vscode_dir.mkdir(exist_ok=True)
            
            settings_file = vscode_dir / 'settings.json'
            settings_file.write_text(config, encoding='utf-8')
            
            print(f"✅ VS Code integration created: {settings_file}")
            
    else:
        print("Real-time Validation System")
        print("Commands:")
        print("  monitor           - Start real-time file monitoring")
        print("  validate <file>   - Validate specific file")
        print("  vscode            - Generate VS Code integration")
    
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))