param(
    [switch]$Run,
    [ValidateSet('Debug','Release')]
    [string]$Config = 'Release'
)

# Build helper for predictor-only project.
# Prefers CMake/MSVC on Windows; falls back to g++ if available.

Set-StrictMode -Version Latest

$rootDir = $PSScriptRoot
$srcDir  = Join-Path $rootDir 'src'
$buildDir = Join-Path $rootDir 'build'
if (-not (Test-Path $buildDir)) { New-Item -ItemType Directory $buildDir | Out-Null }

$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmake) {
    Write-Host "Configuring with CMake..."
    & cmake -S $rootDir -B $buildDir
    if ($LASTEXITCODE -ne 0) { Write-Error "CMake configure failed."; exit $LASTEXITCODE }

    Write-Host "Building with CMake ($Config)..."
    & cmake --build $buildDir --config $Config
    if ($LASTEXITCODE -ne 0) { Write-Error "CMake build failed."; exit $LASTEXITCODE }

    $outExe = Join-Path $buildDir (Join-Path $Config 'predictor.exe')
    if (-not (Test-Path $outExe)) { Write-Error "predictor.exe not found after build."; exit 1 }
    Write-Host ("Built: " + $outExe)

    if ($Run.IsPresent) {
        $csv = Join-Path $rootDir 'data\stock_data.csv'
        if (-not (Test-Path $csv)) { Write-Error ("CSV not found: " + $csv); exit 1 }
        Write-Host ("Running: " + $outExe + " " + $csv + " 1 0.8")
        & $outExe $csv 1 0.8
    }
    exit 0
}

# Fallback: build with g++ if CMake is not available
$gpp = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $gpp) { Write-Error "Neither CMake nor g++ found on PATH."; exit 1 }

# Explicitly compile only predictor sources (exclude legacy backtester files)
$sources = @(
    Join-Path $srcDir 'predictor.cpp'
    Join-Path $srcDir 'csv_loader.cpp'
    Join-Path $srcDir 'indicator.cpp'
    Join-Path $srcDir 'feature_engineer.cpp'
    Join-Path $srcDir 'linear_regression.cpp'
)

$outExe = Join-Path $buildDir 'predictor.exe'
Write-Host ('Compiling with g++: ' + ($sources -join ' '))
& g++ -std=c++17 -O2 $sources -o $outExe
if ($LASTEXITCODE -ne 0) { Write-Error ("Build failed (g++ exit code " + $LASTEXITCODE + ")"); exit $LASTEXITCODE }

Write-Host ("Built: " + $outExe)

if ($Run.IsPresent) {
    $csv = Join-Path $rootDir 'data\stock_data.csv'
    if (-not (Test-Path $csv)) { Write-Error ("CSV not found: " + $csv); exit 1 }
    Write-Host ("Running: " + $outExe + " " + $csv + " 1 0.8")
    & $outExe $csv 1 0.8
}
