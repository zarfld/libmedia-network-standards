#!/usr/bin/env powershell
# Script to update Cross-Standards Architecture Integration Requirements IDs

$filePath = "d:\Repos\libmedia-network-standards\02-requirements\functional\cross-standards-architecture-integration-requirements.md"

Write-Host "Updating Cross-Standards Architecture Integration Requirements IDs..." -ForegroundColor Green

# Read the file content
$content = Get-Content $filePath -Raw

# Define the replacement mappings for different ID types
$replacements = @{
    # Functional Requirements F001-F008 patterns
    'F001:' = 'REQ-F-CROSSARCH-001:'
    'F002:' = 'REQ-F-CROSSARCH-002:'
    'F003:' = 'REQ-F-CROSSARCH-003:'
    'F004:' = 'REQ-F-CROSSARCH-004:'
    'F005:' = 'REQ-F-CROSSARCH-005:'
    'F006:' = 'REQ-F-CROSSARCH-006:'
    'F007:' = 'REQ-F-CROSSARCH-007:'
    'F008:' = 'REQ-F-CROSSARCH-008:'
    
    # Functional sub-requirements patterns (F001.001, etc.)
    'F001\.(\d{3}):' = 'REQ-F-CROSSARCH-001.$1:'
    'F002\.(\d{3}):' = 'REQ-F-CROSSARCH-002.$1:'
    'F003\.(\d{3}):' = 'REQ-F-CROSSARCH-003.$1:'
    'F004\.(\d{3}):' = 'REQ-F-CROSSARCH-004.$1:'
    'F005\.(\d{3}):' = 'REQ-F-CROSSARCH-005.$1:'
    'F006\.(\d{3}):' = 'REQ-F-CROSSARCH-006.$1:'
    'F007\.(\d{3}):' = 'REQ-F-CROSSARCH-007.$1:'
    'F008\.(\d{3}):' = 'REQ-F-CROSSARCH-008.$1:'
    
    # Interface Requirements
    'IR-001:' = 'IR-CROSSARCH-001:'
    'IR-002:' = 'IR-CROSSARCH-002:'
    'IR-003:' = 'IR-CROSSARCH-003:'
    'IR-004:' = 'IR-CROSSARCH-004:'
    'IR-005:' = 'IR-CROSSARCH-005:'
    'IR-006:' = 'IR-CROSSARCH-006:'
    'IR-007:' = 'IR-CROSSARCH-007:'
    'IR-008:' = 'IR-CROSSARCH-008:'
    'IR-009:' = 'IR-CROSSARCH-009:'
    'IR-010:' = 'IR-CROSSARCH-010:'
    'IR-011:' = 'IR-CROSSARCH-011:'
    'IR-012:' = 'IR-CROSSARCH-012:'
    'IR-013:' = 'IR-CROSSARCH-013:'
    'IR-014:' = 'IR-CROSSARCH-014:'
    'IR-015:' = 'IR-CROSSARCH-015:'
    
    # Verification Requirements
    'VR-001:' = 'VR-CROSSARCH-001:'
    'VR-002:' = 'VR-CROSSARCH-002:'
    'VR-003:' = 'VR-CROSSARCH-003:'
    'VR-004:' = 'VR-CROSSARCH-004:'
    'VR-005:' = 'VR-CROSSARCH-005:'
    'VR-006:' = 'VR-CROSSARCH-006:'
    'VR-007:' = 'VR-CROSSARCH-007:'
    'VR-008:' = 'VR-CROSSARCH-008:'
    'VR-009:' = 'VR-CROSSARCH-009:'
    'VR-010:' = 'VR-CROSSARCH-010:'
    'VR-011:' = 'VR-CROSSARCH-011:'
    'VR-012:' = 'VR-CROSSARCH-012:'
    'VR-013:' = 'VR-CROSSARCH-013:'
    'VR-014:' = 'VR-CROSSARCH-014:'
    'VR-015:' = 'VR-CROSSARCH-015:'
    'VR-016:' = 'VR-CROSSARCH-016:'
    'VR-017:' = 'VR-CROSSARCH-017:'
    'VR-018:' = 'VR-CROSSARCH-018:'
    'VR-019:' = 'VR-CROSSARCH-019:'
    'VR-020:' = 'VR-CROSSARCH-020:'
    
    # Acceptance Criteria
    'AC-001:' = 'AC-CROSSARCH-001:'
    'AC-002:' = 'AC-CROSSARCH-002:'
    'AC-003:' = 'AC-CROSSARCH-003:'
    'AC-004:' = 'AC-CROSSARCH-004:'
    'AC-005:' = 'AC-CROSSARCH-005:'
    'AC-006:' = 'AC-CROSSARCH-006:'
    'AC-007:' = 'AC-CROSSARCH-007:'
    'AC-008:' = 'AC-CROSSARCH-008:'
    'AC-009:' = 'AC-CROSSARCH-009:'
    'AC-010:' = 'AC-CROSSARCH-010:'
    'AC-011:' = 'AC-CROSSARCH-011:'
    'AC-012:' = 'AC-CROSSARCH-012:'
    'AC-013:' = 'AC-CROSSARCH-013:'
    'AC-014:' = 'AC-CROSSARCH-014:'
    'AC-015:' = 'AC-CROSSARCH-015:'
    'AC-016:' = 'AC-CROSSARCH-016:'
    'AC-017:' = 'AC-CROSSARCH-017:'
    'AC-018:' = 'AC-CROSSARCH-018:'
    'AC-019:' = 'AC-CROSSARCH-019:'
    'AC-020:' = 'AC-CROSSARCH-020:'
    'AC-021:' = 'AC-CROSSARCH-021:'
    'AC-022:' = 'AC-CROSSARCH-022:'
    'AC-023:' = 'AC-CROSSARCH-023:'
    'AC-024:' = 'AC-CROSSARCH-024:'
    'AC-025:' = 'AC-CROSSARCH-025:'
    'AC-026:' = 'AC-CROSSARCH-026:'
    'AC-027:' = 'AC-CROSSARCH-027:'
    'AC-028:' = 'AC-CROSSARCH-028:'
    'AC-029:' = 'AC-CROSSARCH-029:'
    'AC-030:' = 'AC-CROSSARCH-030:'
    
    # Non-functional Requirements
    'NFR-001:' = 'REQ-NF-CROSSARCH-001:'
    'NFR-002:' = 'REQ-NF-CROSSARCH-002:'
    'NFR-003:' = 'REQ-NF-CROSSARCH-003:'
    'NFR-004:' = 'REQ-NF-CROSSARCH-004:'
    'NFR-005:' = 'REQ-NF-CROSSARCH-005:'
    'NFR-006:' = 'REQ-NF-CROSSARCH-006:'
    'NFR-007:' = 'REQ-NF-CROSSARCH-007:'
    'NFR-008:' = 'REQ-NF-CROSSARCH-008:'
    'NFR-009:' = 'REQ-NF-CROSSARCH-009:'
    'NFR-010:' = 'REQ-NF-CROSSARCH-010:'
    'NFR-011:' = 'REQ-NF-CROSSARCH-011:'
    'NFR-012:' = 'REQ-NF-CROSSARCH-012:'
    'NFR-013:' = 'REQ-NF-CROSSARCH-013:'
    'NFR-014:' = 'REQ-NF-CROSSARCH-014:'
    'NFR-015:' = 'REQ-NF-CROSSARCH-015:'
    'NFR-016:' = 'REQ-NF-CROSSARCH-016:'
    'NFR-017:' = 'REQ-NF-CROSSARCH-017:'
    'NFR-018:' = 'REQ-NF-CROSSARCH-018:'
    'NFR-019:' = 'REQ-NF-CROSSARCH-019:'
    'NFR-020:' = 'REQ-NF-CROSSARCH-020:'
    'NFR-021:' = 'REQ-NF-CROSSARCH-021:'
    'NFR-022:' = 'REQ-NF-CROSSARCH-022:'
    'NFR-023:' = 'REQ-NF-CROSSARCH-023:'
    'NFR-024:' = 'REQ-NF-CROSSARCH-024:'
    'NFR-025:' = 'REQ-NF-CROSSARCH-025:'
    'NFR-026:' = 'REQ-NF-CROSSARCH-026:'
    'NFR-027:' = 'REQ-NF-CROSSARCH-027:'
    'NFR-028:' = 'REQ-NF-CROSSARCH-028:'
    'NFR-029:' = 'REQ-NF-CROSSARCH-029:'
    'NFR-030:' = 'REQ-NF-CROSSARCH-030:'
}

Write-Host "Applying ID replacements..." -ForegroundColor Yellow

# Apply replacements
foreach ($key in $replacements.Keys) {
    $oldPattern = $key
    $newPattern = $replacements[$key]
    
    if ($key -match '\\') {
        # Handle regex patterns for sub-requirements
        $content = $content -replace $oldPattern, $newPattern
    } else {
        # Handle simple string replacements
        $content = $content.Replace($oldPattern, $newPattern)
    }
    
    if ($content -match $newPattern) {
        Write-Host "✅ Replaced: $oldPattern → $newPattern" -ForegroundColor Green
    }
}

# Write the updated content back to the file
$content | Set-Content $filePath -Encoding UTF8

Write-Host ""
Write-Host "✅ Cross-Standards Architecture Integration Requirements IDs updated successfully!" -ForegroundColor Green
Write-Host "File: $filePath" -ForegroundColor Cyan