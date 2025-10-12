# PowerShell-only Requirement ID Management Script
# Provides basic requirement ID scanning and management without Python dependency

param(
    [switch]$Scan,                    # Scan existing requirement IDs
    [int]$NextF = 0,                  # Get next N functional IDs
    [int]$NextNF = 0,                 # Get next N non-functional IDs
    [string]$Validate = "",           # Validate YAML front matter in file
    [string]$Assign = "",             # Auto-assign IDs to specification file
    [int]$FCount = 50,                # Number of functional requirements to assign
    [int]$NFCount = 0,                # Number of non-functional requirements to assign
    [switch]$Status,                  # Show registry status
    [switch]$Help                     # Show help
)

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Split-Path -Parent $ScriptDir
$RegistryFile = Join-Path $ScriptDir "requirement-id-registry.json"

# Initialize or load registry
function Get-Registry {
    if (Test-Path $RegistryFile) {
        try {
            $registry = Get-Content $RegistryFile | ConvertFrom-Json
            return $registry
        } catch {
            Write-Warning "Failed to load registry, creating new one"
        }
    }
    
    # Create new registry
    return @{
        functional_ids = @{}
        non_functional_ids = @{}
        next_functional = 1
        next_non_functional = 1
        last_updated = (Get-Date).ToString("yyyy-MM-ddTHH:mm:ss")
        conflicts = @()
    }
}

function Save-Registry {
    param($Registry)
    
    $Registry.last_updated = (Get-Date).ToString("yyyy-MM-ddTHH:mm:ss")
    $Registry | ConvertTo-Json -Depth 10 | Set-Content $RegistryFile
}

function Scan-ExistingIds {
    Write-Host "Scanning existing requirement IDs..." -ForegroundColor Cyan
    
    $registry = @{
        functional_ids = @{}
        non_functional_ids = @{}
        next_functional = 1
        next_non_functional = 1
        conflicts = @()
    }
    
    # Scan all markdown files
    $mdFiles = Get-ChildItem -Path $RepoRoot -Filter "*.md" -Recurse | Where-Object { 
        $_.FullName -notlike "*\.git\*" -and 
        $_.Name -ne "README.md" 
    }
    
    foreach ($file in $mdFiles) {
        $lineNumber = 0
        $content = Get-Content $file.FullName -ErrorAction SilentlyContinue
        
        foreach ($line in $content) {
            $lineNumber++
            
            # Find functional requirements
            $fMatches = [regex]::Matches($line, 'REQ-F-(\d+)')
            foreach ($match in $fMatches) {
                $num = [int]$match.Groups[1].Value
                $reqId = "REQ-F-{0:D3}" -f $num
                
                $reqInfo = @{
                    id = $reqId
                    type = 'F'
                    number = $num
                    file_path = $file.FullName.Substring($RepoRoot.Length + 1)
                    line_number = $lineNumber
                    description = $line.Trim()
                }
                
                if ($registry.functional_ids.ContainsKey($num.ToString())) {
                    $existing = $registry.functional_ids[$num.ToString()]
                    $conflict = "Duplicate $reqId in $($reqInfo.file_path):$lineNumber and $($existing.file_path):$($existing.line_number)"
                    $registry.conflicts += $conflict
                } else {
                    $registry.functional_ids[$num.ToString()] = $reqInfo
                }
            }
            
            # Find non-functional requirements
            $nfMatches = [regex]::Matches($line, 'REQ-NF-(\d+)')
            foreach ($match in $nfMatches) {
                $num = [int]$match.Groups[1].Value
                $reqId = "REQ-NF-{0:D3}" -f $num
                
                $reqInfo = @{
                    id = $reqId
                    type = 'NF'
                    number = $num
                    file_path = $file.FullName.Substring($RepoRoot.Length + 1)
                    line_number = $lineNumber
                    description = $line.Trim()
                }
                
                if ($registry.non_functional_ids.ContainsKey($num.ToString())) {
                    $existing = $registry.non_functional_ids[$num.ToString()]
                    $conflict = "Duplicate $reqId in $($reqInfo.file_path):$lineNumber and $($existing.file_path):$($existing.line_number)"
                    $registry.conflicts += $conflict
                } else {
                    $registry.non_functional_ids[$num.ToString()] = $reqInfo
                }
            }
        }
    }
    
    # Calculate next available IDs
    if ($registry.functional_ids.Count -gt 0) {
        $maxF = ($registry.functional_ids.Keys | ForEach-Object { [int]$_ } | Measure-Object -Maximum).Maximum
        $registry.next_functional = $maxF + 1
    }
    
    if ($registry.non_functional_ids.Count -gt 0) {
        $maxNF = ($registry.non_functional_ids.Keys | ForEach-Object { [int]$_ } | Measure-Object -Maximum).Maximum
        $registry.next_non_functional = $maxNF + 1
    }
    
    Save-Registry $registry
    
    Write-Host "Found $($registry.functional_ids.Count) functional requirements" -ForegroundColor Green
    Write-Host "Found $($registry.non_functional_ids.Count) non-functional requirements" -ForegroundColor Green
    Write-Host "Next functional ID: REQ-F-$($registry.next_functional.ToString('D3'))" -ForegroundColor Yellow
    Write-Host "Next non-functional ID: REQ-NF-$($registry.next_non_functional.ToString('D3'))" -ForegroundColor Yellow
    
    if ($registry.conflicts.Count -gt 0) {
        Write-Host "`n⚠️  Found $($registry.conflicts.Count) conflicts:" -ForegroundColor Red
        foreach ($conflict in $registry.conflicts) {
            Write-Host "  - $conflict" -ForegroundColor Red
        }
    }
    
    return $registry
}

function Get-NextFunctionalIds {
    param([int]$Count)
    
    $registry = Get-Registry
    $ids = @()
    
    for ($i = 0; $i -lt $Count; $i++) {
        $num = $registry.next_functional + $i
        $ids += "REQ-F-{0:D3}" -f $num
    }
    
    $registry.next_functional += $Count
    Save-Registry $registry
    
    Write-Host "Next $Count functional IDs:" -ForegroundColor Green
    foreach ($id in $ids) {
        Write-Host "  - `"$id`"" -ForegroundColor White
    }
    
    return $ids
}

function Get-NextNonFunctionalIds {
    param([int]$Count)
    
    $registry = Get-Registry
    $ids = @()
    
    for ($i = 0; $i -lt $Count; $i++) {
        $num = $registry.next_non_functional + $i
        $ids += "REQ-NF-{0:D3}" -f $num
    }
    
    $registry.next_non_functional += $Count
    Save-Registry $registry
    
    Write-Host "Next $Count non-functional IDs:" -ForegroundColor Green
    foreach ($id in $ids) {
        Write-Host "  - `"$id`"" -ForegroundColor White
    }
    
    return $ids
}

function Show-RegistryStatus {
    $registry = Get-Registry
    
    Write-Host "Requirement ID Registry Status:" -ForegroundColor Cyan
    Write-Host "Functional IDs: $($registry.functional_ids.Count) used, next: REQ-F-$($registry.next_functional.ToString('D3'))" -ForegroundColor White
    Write-Host "Non-functional IDs: $($registry.non_functional_ids.Count) used, next: REQ-NF-$($registry.next_non_functional.ToString('D3'))" -ForegroundColor White
    Write-Host "Conflicts: $($registry.conflicts.Count)" -ForegroundColor $(if ($registry.conflicts.Count -gt 0) { "Red" } else { "Green" })
    Write-Host "Last updated: $($registry.last_updated)" -ForegroundColor Gray
    
    if ($registry.conflicts.Count -gt 0) {
        Write-Host "`n⚠️  Active conflicts:" -ForegroundColor Red
        $showCount = [Math]::Min(5, $registry.conflicts.Count)
        for ($i = 0; $i -lt $showCount; $i++) {
            Write-Host "  - $($registry.conflicts[$i])" -ForegroundColor Red
        }
        if ($registry.conflicts.Count -gt 5) {
            Write-Host "  ... and $($registry.conflicts.Count - 5) more" -ForegroundColor Red
        }
    }
}

# Show help if requested
if ($Help) {
    Write-Host @"
Requirement ID Registry Management (PowerShell Edition)

USAGE:
    .\Manage-RequirementIDs-PS.ps1 [OPTIONS]

OPTIONS:
    -Scan                    Scan existing requirement IDs across all specs
    -NextF <count>          Get next N functional requirement IDs
    -NextNF <count>         Get next N non-functional requirement IDs  
    -Status                 Show current registry status
    -Help                   Show this help message

EXAMPLES:
    # Scan all existing requirement IDs
    .\Manage-RequirementIDs-PS.ps1 -Scan
    
    # Get next 50 functional requirement IDs  
    .\Manage-RequirementIDs-PS.ps1 -NextF 50
    
    # Get next 10 non-functional requirement IDs
    .\Manage-RequirementIDs-PS.ps1 -NextNF 10
    
    # Show registry status
    .\Manage-RequirementIDs-PS.ps1 -Status

REGISTRY FILE:
    Registry data is stored in: $RegistryFile
    
NOTE: This is a PowerShell-only version. For advanced features like YAML validation
and auto-assignment, use the Python version (requires Python 3.7+).
"@
    exit 0
}

# Execute requested action
if ($Scan) {
    Scan-ExistingIds | Out-Null
} elseif ($NextF -gt 0) {
    Get-NextFunctionalIds -Count $NextF | Out-Null
} elseif ($NextNF -gt 0) {
    Get-NextNonFunctionalIds -Count $NextNF | Out-Null
} elseif ($Status) {
    Show-RegistryStatus
} else {
    # Default: show status
    Show-RegistryStatus
}

Write-Host "`n✅ Requirement ID management completed successfully." -ForegroundColor Green