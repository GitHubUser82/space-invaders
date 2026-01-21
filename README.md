# space-invaders
An emulator of the Space Invaders arcade machine.

## Build

### Windows

This project uses **vcpkg** to manage dependencies (e.g. glfw3).
You can set the `VCPKG_ROOT`environment variable, or replace `VCPKG_ROOT` with the absolute path to your vcpkg installation.

#### PowerShell

##### Debug
```powershell
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="$($Env:VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake"
cmake --build build-debug
```

##### Release
```powershell
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="$($Env:VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake"
cmake --build build-release
```

#### cmd

##### Debug
```cmd
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build build-debug
```

##### Release
```cmd
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build build-release
```