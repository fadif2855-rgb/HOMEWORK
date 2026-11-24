# Builds (and runs) the project from ANY folder. Usage:  .\build_and_run.ps1   (add -NoRun to only build)
param([switch]$NoRun)

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

# If the build folder was created in another location (project moved / copied / unzipped elsewhere),
# CMake refuses to use it -> delete it and start fresh.
$cache = ".\build\CMakeCache.txt"
if (Test-Path $cache) {
    $line = Select-String -Path $cache -Pattern "^CMAKE_HOME_DIRECTORY:INTERNAL=(.*)$" | Select-Object -First 1
    if ($line) {
        $oldHome = $line.Matches[0].Groups[1].Value.Trim().Replace("/", "\")
        if ($oldHome -ne $PSScriptRoot) {
            Write-Host "Build folder belongs to another location, recreating it..."
            Remove-Item ".\build" -Recurse -Force
        }
    }
}

# Configure every time: it is fast, and it makes CMake notice new / renamed .cpp files.
cmake -S . -B build -G "MinGW Makefiles"
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed" }

cmake --build build
if ($LASTEXITCODE -ne 0) { throw "Build failed" }

# Always refresh the assets next to the exe (textures / shaders / models), even if no .cpp changed.
foreach ($d in "textures", "shaders", "models") {
    New-Item -ItemType Directory -Force ".\build\$d" | Out-Null
    Copy-Item ".\GLProject\$d\*" ".\build\$d" -Recurse -Force
}

if (-not $NoRun) {
    Set-Location ".\build"
    .\GLProject.exe
}
