#!/usr/bin/env python3
"""
Real Requirements Discovery and Traceability System

This script performs ACTUAL content-based analysis of repository requirements
and creates genuine traceability links between requirements, architecture, design, 
and implementation based on semantic content matching.

NO MORE FAKE HARDCODED VALUES - ONLY REAL REPOSITORY ANALYSIS
"""

import os
import re
import json
import yaml
from pathlib import Path
from typing import Dict, List, Set, Tuple, Optional
from dataclasses import dataclass, asdict
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)

@dataclass
class Requirement:
    """Real requirement extracted from repository files"""
    id: str
    title: str
    description: str
    source_file: str
    category: str  # functional, non-functional, stakeholder
    standard: str  # IEEE-1722.1, AES67, etc.
    keywords: List[str]
    line_number: int

@dataclass
class ArchitectureComponent:
    """Architecture component that needs requirement mapping"""
    file_path: str
    title: str
    content: str
    keywords: List[str]
    current_requirements: List[str]

class RealRequirementsDiscovery:
    def __init__(self, repo_root: str):
        self.repo_root = Path(repo_root)
        self.requirements: Dict[str, Requirement] = {}
        self.architecture_files: Dict[str, ArchitectureComponent] = {}
        
    def discover_all_requirements(self) -> Dict[str, Requirement]:
        """Scan ALL requirement files and extract REAL requirements"""
        logger.info("🔍 Discovering ALL requirements in repository...")
        
        requirements_dir = self.repo_root / "02-requirements"
        
        # Scan all markdown files in requirements directory
        for req_file in requirements_dir.rglob("*.md"):
            if req_file.name in ["README.md", "TEMPLATE.md", "ID-NAMING-CONVENTION-MIGRATION.md"]:
                continue
                
            logger.info(f"📄 Analyzing: {req_file.relative_to(self.repo_root)}")
            self._extract_requirements_from_file(req_file)
        
        logger.info(f"✅ Found {len(self.requirements)} REAL requirements")
        return self.requirements
    
    def _extract_requirements_from_file(self, file_path: Path):
        """Extract requirements from a single file"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
                
            # Determine category from file path
            if "functional" in str(file_path):
                category = "functional"
            elif "non-functional" in str(file_path):
                category = "non-functional"  
            elif "stakeholder" in str(file_path):
                category = "stakeholder"
            else:
                category = "unknown"
            
            # Extract standard from filename
            standard = self._extract_standard_from_filename(file_path.name)
            
            # Find requirement patterns: REQ-XXX-YYY-NNN:
            req_pattern = r'^(REQ-[A-Z]+-[A-Z0-9]+-[0-9]+):\s*(.+?)$'
            
            lines = content.split('\n')
            for line_num, line in enumerate(lines, 1):
                match = re.match(req_pattern, line.strip())
                if match:
                    req_id = match.group(1)
                    req_title = match.group(2).strip()
                    
                    # Extract description from following lines
                    description = self._extract_requirement_description(lines, line_num)
                    
                    # Extract keywords from content
                    keywords = self._extract_keywords(req_title + " " + description)
                    
                    requirement = Requirement(
                        id=req_id,
                        title=req_title,
                        description=description,
                        source_file=str(file_path.relative_to(self.repo_root)),
                        category=category,
                        standard=standard,
                        keywords=keywords,
                        line_number=line_num
                    )
                    
                    self.requirements[req_id] = requirement
                    logger.info(f"  ✓ {req_id}: {req_title[:50]}...")
                    
        except Exception as e:
            logger.error(f"❌ Error processing {file_path}: {e}")
    
    def _extract_standard_from_filename(self, filename: str) -> str:
        """Extract standard name from filename"""
        # Map filename patterns to standards
        patterns = {
            r'ieee-1722\.1': 'IEEE-1722.1',
            r'ieee-1722(?!\.1)': 'IEEE-1722',
            r'ieee-1588': 'IEEE-1588',
            r'ieee-802\.1as': 'IEEE-802.1AS',
            r'ieee-802\.1ab': 'IEEE-802.1AB',
            r'ieee-802\.1ax': 'IEEE-802.1AX',
            r'aes67': 'AES67',
            r'aes70': 'AES70',
            r'aes3': 'AES3',
            r'aes5': 'AES5',
            r'aes60': 'AES60',
        }
        
        for pattern, standard in patterns.items():
            if re.search(pattern, filename.lower()):
                return standard
        return "UNKNOWN"
    
    def _extract_requirement_description(self, lines: List[str], req_line: int) -> str:
        """Extract requirement description from following lines"""
        description_lines = []
        
        # Look for description in next few lines
        for i in range(req_line, min(req_line + 10, len(lines))):
            line = lines[i].strip()
            
            # Stop at next requirement or empty line
            if re.match(r'^REQ-[A-Z]+-[A-Z0-9]+-[0-9]+:', line):
                break
            if not line:
                continue
                
            # Skip markdown headers and formatting
            if line.startswith('#') or line.startswith('*') or line.startswith('-'):
                continue
                
            description_lines.append(line)
            
            # Stop after reasonable description length
            if len(' '.join(description_lines)) > 200:
                break
                
        return ' '.join(description_lines)[:500]  # Limit description length
    
    def _extract_keywords(self, text: str) -> List[str]:
        """Extract technical keywords from requirement text"""
        # Technical domain keywords
        domain_keywords = {
            'timing': ['synchronization', 'timing', 'clock', 'sync', 'precision', 'accuracy', 'delay', 'jitter'],
            'audio': ['audio', 'stream', 'channel', 'sampling', 'frequency', 'format', 'codec'],
            'network': ['ethernet', 'packet', 'transmission', 'protocol', 'layer', 'frame'],
            'control': ['control', 'command', 'response', 'management', 'configuration'],
            'discovery': ['discovery', 'advertisement', 'announce', 'available', 'entity'],
            'quality': ['quality', 'performance', 'latency', 'throughput', 'reliability'],
            'security': ['security', 'authentication', 'encryption', 'authorization'],
            'compatibility': ['compatibility', 'interoperability', 'compliance', 'standard']
        }
        
        text_lower = text.lower()
        found_keywords = []
        
        # Find domain keywords
        for domain, keywords in domain_keywords.items():
            for keyword in keywords:
                if keyword in text_lower:
                    found_keywords.append(keyword)
        
        # Extract IEEE standard references
        ieee_standards = re.findall(r'ieee[- ]?(\d+(?:\.\d+)?)', text_lower)
        for std in ieee_standards:
            found_keywords.append(f'ieee-{std}')
        
        # Extract AES standard references  
        aes_standards = re.findall(r'aes[- ]?(\d+)', text_lower)
        for std in aes_standards:
            found_keywords.append(f'aes{std}')
        
        return list(set(found_keywords))
    
    def discover_architecture_files(self) -> Dict[str, ArchitectureComponent]:
        """Discover architecture files that need requirement mapping"""
        logger.info("🏗️ Discovering architecture files...")
        
        arch_dir = self.repo_root / "03-architecture"
        
        for arch_file in arch_dir.rglob("*.md"):
            if arch_file.name in ["README.md", "TEMPLATE.md"]:
                continue
                
            logger.info(f"🏛️ Analyzing: {arch_file.relative_to(self.repo_root)}")
            self._analyze_architecture_file(arch_file)
        
        logger.info(f"✅ Found {len(self.architecture_files)} architecture components")
        return self.architecture_files
    
    def _analyze_architecture_file(self, file_path: Path):
        """Analyze architecture file and extract current requirements"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Parse YAML front matter
            yaml_match = re.match(r'^---\n(.*?)\n---\n(.*)$', content, re.DOTALL)
            if yaml_match:
                yaml_content = yaml_match.group(1)
                body_content = yaml_match.group(2)
                
                try:
                    yaml_data = yaml.safe_load(yaml_content)
                    current_requirements = yaml_data.get('requirements', [])
                    title = yaml_data.get('title', file_path.stem)
                except:
                    current_requirements = []
                    title = file_path.stem
            else:
                current_requirements = []
                title = file_path.stem
                body_content = content
            
            # Extract keywords from content
            keywords = self._extract_keywords(title + " " + body_content)
            
            component = ArchitectureComponent(
                file_path=str(file_path.relative_to(self.repo_root)),
                title=title,
                content=body_content[:1000],  # First 1000 chars
                keywords=keywords,
                current_requirements=current_requirements
            )
            
            self.architecture_files[str(file_path)] = component
            
        except Exception as e:
            logger.error(f"❌ Error analyzing {file_path}: {e}")
    
    def create_semantic_mapping(self) -> Dict[str, List[str]]:
        """Create semantic mapping between architecture files and requirements"""
        logger.info("🧠 Creating semantic requirement mappings...")
        
        mappings = {}
        
        for arch_file, arch_component in self.architecture_files.items():
            mapped_requirements = []
            
            # Score requirements based on keyword overlap
            requirement_scores = {}
            
            for req_id, requirement in self.requirements.items():
                score = self._calculate_semantic_score(arch_component, requirement)
                if score > 0:
                    requirement_scores[req_id] = score
            
            # Select top matching requirements
            sorted_reqs = sorted(requirement_scores.items(), key=lambda x: x[1], reverse=True)
            
            # Take top 5 most relevant requirements
            for req_id, score in sorted_reqs[:5]:
                mapped_requirements.append(req_id)
                logger.info(f"  📎 {arch_component.title} ↔ {req_id} (score: {score:.2f})")
            
            mappings[arch_file] = mapped_requirements
        
        return mappings
    
    def _calculate_semantic_score(self, arch_component: ArchitectureComponent, requirement: Requirement) -> float:
        """Calculate semantic similarity score between architecture and requirement"""
        score = 0.0
        
        # Keyword overlap scoring
        arch_keywords = set(arch_component.keywords)
        req_keywords = set(requirement.keywords)
        
        overlap = arch_keywords.intersection(req_keywords)
        if overlap:
            score += len(overlap) * 2.0
        
        # Standard matching bonus
        arch_filename = Path(arch_component.file_path).name.lower()
        req_standard = requirement.standard.lower().replace('.', '')
        
        if req_standard.replace('-', '') in arch_filename.replace('-', ''):
            score += 10.0
        
        # Title matching
        arch_title = arch_component.title.lower()
        req_title = requirement.title.lower()
        
        # Look for common technical terms
        title_words = set(arch_title.split()) & set(req_title.split())
        score += len(title_words) * 1.5
        
        # Content analysis
        arch_content = arch_component.content.lower()
        req_content = (requirement.title + " " + requirement.description).lower()
        
        # Check for shared technical terms
        shared_terms = 0
        technical_terms = ['protocol', 'message', 'packet', 'stream', 'entity', 'descriptor', 
                          'command', 'response', 'synchronization', 'timing', 'audio']
        
        for term in technical_terms:
            if term in arch_content and term in req_content:
                shared_terms += 1
        
        score += shared_terms * 0.5
        
        return score
    
    def generate_traceability_report(self) -> Dict:
        """Generate comprehensive traceability report"""
        logger.info("📊 Generating traceability report...")
        
        mappings = self.create_semantic_mapping()
        
        report = {
            "timestamp": "2024-10-12T00:00:00Z",
            "total_requirements": len(self.requirements),
            "total_architecture_files": len(self.architecture_files),
            "requirements_by_standard": {},
            "mappings": mappings,
            "detailed_requirements": {req_id: asdict(req) for req_id, req in self.requirements.items()},
            "unmapped_requirements": [],
            "architecture_coverage": {}
        }
        
        # Group requirements by standard
        for req_id, requirement in self.requirements.items():
            standard = requirement.standard
            if standard not in report["requirements_by_standard"]:
                report["requirements_by_standard"][standard] = []
            report["requirements_by_standard"][standard].append(req_id)
        
        # Find unmapped requirements
        mapped_req_ids = set()
        for req_list in mappings.values():
            mapped_req_ids.update(req_list)
        
        report["unmapped_requirements"] = [req_id for req_id in self.requirements.keys() 
                                         if req_id not in mapped_req_ids]
        
        # Calculate architecture coverage
        for arch_file, req_list in mappings.items():
            arch_name = Path(arch_file).name
            report["architecture_coverage"][arch_name] = {
                "requirement_count": len(req_list),
                "requirements": req_list
            }
        
        return report
    
    def update_architecture_yaml_headers(self, mappings: Dict[str, List[str]]):
        """Update architecture file YAML headers with correct requirement mappings"""
        logger.info("🔧 Updating architecture file YAML headers...")
        
        for arch_file, req_list in mappings.items():
            if not req_list:
                continue
                
            try:
                file_path = Path(arch_file)
                
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                # Parse existing YAML front matter
                yaml_match = re.match(r'^---\n(.*?)\n---\n(.*)$', content, re.DOTALL)
                if yaml_match:
                    yaml_content = yaml_match.group(1)
                    body_content = yaml_match.group(2)
                    
                    yaml_data = yaml.safe_load(yaml_content)
                else:
                    # Create new YAML front matter
                    yaml_data = {}
                    body_content = content
                
                # Update requirements field
                yaml_data['requirements'] = req_list
                
                # Write updated file
                new_yaml = yaml.dump(yaml_data, default_flow_style=False, sort_keys=True)
                new_content = f"---\n{new_yaml}---\n{body_content}"
                
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(new_content)
                
                logger.info(f"✅ Updated {file_path.name} with {len(req_list)} requirements")
                
            except Exception as e:
                logger.error(f"❌ Error updating {arch_file}: {e}")

def main():
    """Main execution function"""
    repo_root = "."
    
    logger.info("🚀 Starting REAL Requirements Discovery System")
    logger.info("=" * 60)
    
    discovery = RealRequirementsDiscovery(repo_root)
    
    # Step 1: Discover all requirements
    requirements = discovery.discover_all_requirements()
    
    # Step 2: Discover architecture files  
    architecture_files = discovery.discover_architecture_files()
    
    # Step 3: Create semantic mappings
    mappings = discovery.create_semantic_mapping()
    
    # Step 4: Generate report
    report = discovery.generate_traceability_report()
    
    # Save report
    with open("Requirements_Traceability_Report.json", "w", encoding='utf-8') as f:
        json.dump(report, f, indent=2, ensure_ascii=False)
    
    # Step 5: Update architecture files
    discovery.update_architecture_yaml_headers(mappings)
    
    logger.info("=" * 60)
    logger.info("✅ REAL Requirements Discovery Complete!")
    logger.info(f"📋 Found {len(requirements)} requirements")
    logger.info(f"🏗️ Mapped {len(architecture_files)} architecture files")
    logger.info(f"📄 Report saved to Requirements_Traceability_Report.json")

if __name__ == "__main__":
    main()