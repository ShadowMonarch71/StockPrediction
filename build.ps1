param(
    [switch]$Run
)

# Simple build helper for the project. Uses g++ (MinGW/MSYS2) on PATH.
# Usage:
#   ./build.ps1        # builds build\strategy.exe
#   ./build.ps1 -Run   # builds then runs the exe with default sample CSV

Set-StrictMode -Version Latest

$srcDir = Join-Path $PSScriptRoot 'src'
$outDir = Join-Path $PSScriptRoot 'build'
if (-not (Test-Path $outDir)) { New-Item -ItemType Directory $outDir | Out-Null }

$g = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $g) { Write-Error "g++ not found on PATH. Install MSYS2 or MinGW-w64 and add g++ to PATH."; exit 1 }

$sources = Get-ChildItem -Path $srcDir -Filter '*.cpp' | ForEach-Object { $_.FullName }
if ($sources.Count -eq 0) { Write-Error "No source files found in $srcDir"; exit 1 }

$outExe = Join-Path $outDir 'strategy.exe'

Write-Host ('Compiling: ' + ($sources -join ' '))
& g++ -std=c++17 -O2 $sources -o $outExe
if ($LASTEXITCODE -ne 0) { Write-Error ("Build failed (g++ exit code " + $LASTEXITCODE + ")"); exit $LASTEXITCODE }

Write-Host ("Built: " + $outExe)

if ($Run.IsPresent) {
    $csv = Join-Path $PSScriptRoot 'data\sample.csv'
    if (-not (Test-Path $csv)) { Write-Error ("Sample CSV not found: " + $csv); exit 1 }
    Write-Host ("Running: " + $outExe + " " + $csv + " 5 3")
    & $outExe $csv 5 3
}
