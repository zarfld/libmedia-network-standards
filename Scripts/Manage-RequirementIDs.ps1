# Requirement ID Management PowerShell Script
# Wrapper for requirements-id-registry.py for Windows development

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
$PythonScript = Join-Path $ScriptDir "requirements-id-registry.py"
$RepoRoot = Split-Path -Parent $ScriptDir

# Check if Python is available
try {
    $pythonVersion = py --version 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Python is not available. Please install Python 3.7+ and add to PATH."
        exit 1
    }
} catch {
    Write-Error "Python is not available. Please install Python 3.7+ and add to PATH."
    exit 1
}

# Show help if requested
if ($Help) {
    Write-Host @"
Requirement ID Registry Management

USAGE:
    .\Manage-RequirementIDs.ps1 [OPTIONS]

OPTIONS:
    -Scan                    Scan existing requirement IDs across all specs
    -NextF <count>          Get next N functional requirement IDs
    -NextNF <count>         Get next N non-functional requirement IDs  
    -Validate <file>        Validate YAML front matter in specification file
    -Assign <file>          Auto-assign IDs to specification file
    -FCount <count>         Number of functional requirements to assign (default: 50)
    -NFCount <count>        Number of non-functional requirements to assign (default: 0)
    -Status                 Show current registry status
    -Help                   Show this help message

EXAMPLES:
    # Scan all existing requirement IDs
    .\Manage-RequirementIDs.ps1 -Scan
    
    # Get next 50 functional requirement IDs  
    .\Manage-RequirementIDs.ps1 -NextF 50
    
    # Auto-assign 50 functional IDs to IEEE 1588 architecture
    .\Manage-RequirementIDs.ps1 -Assign "03-architecture\ieee-1588-2019-ptpv2-architecture-spec.md" -FCount 50
    
    # Validate YAML front matter
    .\Manage-RequirementIDs.ps1 -Validate "03-architecture\ieee-1722-2016-avtp-architecture-spec.md"
    
    # Show registry status
    .\Manage-RequirementIDs.ps1 -Status
"@
    exit 0
}

# Build Python command arguments
$pythonArgs = @("$PythonScript", "--repo-root", "$RepoRoot")

if ($Scan) {
    $pythonArgs += "--scan"
} elseif ($NextF -gt 0) {
    $pythonArgs += @("--next-f", $NextF)
} elseif ($NextNF -gt 0) {
    $pythonArgs += @("--next-nf", $NextNF)
} elseif ($Validate -ne "") {
    $validatePath = Join-Path $RepoRoot $Validate
    if (-not (Test-Path $validatePath)) {
        Write-Error "File not found: $validatePath"
        exit 1
    }
    $pythonArgs += @("--validate", $validatePath)
} elseif ($Assign -ne "") {
    $assignPath = Join-Path $RepoRoot $Assign
    if (-not (Test-Path $assignPath)) {
        Write-Error "File not found: $assignPath"
        exit 1
    }
    $pythonArgs += @("--assign", $assignPath, "--f-count", $FCount, "--nf-count", $NFCount)
} else {
    # Default: show status
}

# Execute Python script
Write-Host "Executing: py $($pythonArgs -join ' ')" -ForegroundColor Gray
try {
    & py @pythonArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Python script execution failed with exit code: $LASTEXITCODE"
        exit $LASTEXITCODE
    }
} catch {
    Write-Error "Failed to execute Python script: $_"
    exit 1
}

Write-Host "`n✅ Requirement ID management completed successfully." -ForegroundColor Green