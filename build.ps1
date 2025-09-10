Param(
    [switch]$Clean,
    [string]$Generator,
    [ValidateSet("Win32","x64","ARM","ARM64")]
    [string]$Arch = "Win32"
)

$ErrorActionPreference = "Stop"

Push-Location $PSScriptRoot
try {
    $buildDir = Join-Path $PSScriptRoot "build"

    if ($Clean -and (Test-Path $buildDir)) {
        Write-Host "Cleaning build directory..."
        Remove-Item -Path $buildDir -Recurse -Force
    }

    if (-not (Test-Path $buildDir)) { New-Item -ItemType Directory -Path $buildDir | Out-Null }

    & cmake --version | Out-Null

    $baseArgs = @("-S", $PSScriptRoot, "-B", $buildDir, "-DCMAKE_BUILD_TYPE=Release")

    $tried = @()
    $genToTry = @()
    if ($Generator) {
        $genToTry += @{ Gen = $Generator; WithArch = $true }
    } else {
        $genToTry += @{ Gen = "Visual Studio 17 2022"; WithArch = $true }
        $genToTry += @{ Gen = "Visual Studio 16 2019"; WithArch = $true }
        $genToTry += @{ Gen = $null; WithArch = $false }
    }

    $configured = $false
    foreach ($item in $genToTry) {
        $gen = $item.Gen
        $withArch = $item.WithArch
        $args = @($baseArgs)
        if ($gen) { $args += @("-G", $gen) }
        if ($withArch -and $Arch) { $args += @("-A", $Arch) }

        if ($gen) { Write-Host "Configuring with generator '$gen' Arch=$Arch..." }
        else { Write-Host "Configuring with default generator..." }

        & cmake @args
        if ($LASTEXITCODE -eq 0) { $configured = $true; break }

        $label = if ($gen) { $gen } else { "<default>" }
        if ($withArch -and $Arch) { $label = "$label/$Arch" }
        $tried += @($label)
    }

    if (-not $configured) {
        throw "CMake configuration failed. Tried: $($tried -join ", ")"
    }

    Write-Host "Building Release..."
    & cmake --build $buildDir --config Release
    if ($LASTEXITCODE -ne 0) { throw "Build failed." }

    $dllName = "GunBound Broker Client.dll"
    $releaseDirCandidates = @(
        (Join-Path $buildDir "Release"),
        $buildDir
    )
    $found = $false
    foreach ($dir in $releaseDirCandidates) {
        $dllPath = Join-Path $dir $dllName
        if (Test-Path $dllPath) {
            Write-Host "Built: $dllPath"
            $found = $true
            break
        }
    }

    if (-not $found) {
        Write-Host "Build completed. DLL not found in standard locations."
        Write-Host "Check '$buildDir' for outputs."
    }
}
catch {
    Write-Error $_
}
finally {
    Pop-Location
}
