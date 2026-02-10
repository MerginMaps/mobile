# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Mergin Maps Mobile is a cross-platform geospatial surveying application built with C++ and Qt 6/QML. It integrates with QGIS for project design and syncs data with Mergin Maps Cloud. Target platforms: Android, iOS, Linux, macOS, Windows.

## Build System

Uses CMake 3.22+ with vcpkg for dependency management. Dependencies (Qt, GDAL, QGIS) are built automatically during CMake configure.

### Directory Layout Expected

```
mm1/
  build/
  vcpkg/    # clone from github.com/microsoft/vcpkg, checkout to VCPKG_BASELINE
  mobile/   # this repository
```

### Build Commands (Linux/macOS)

```bash
# Configure (first time takes ~1 hour for deps)
cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DVCPKG_TARGET_TRIPLET=x64-linux \  # or arm64-osx for Apple Silicon
  -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DUSE_MM_SERVER_API_KEY=FALSE \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
  -DENABLE_TESTS=TRUE \
  -GNinja \
  -S ../mobile

# Build
ninja

# Run
./app/Input  # Linux
./app/MerginMaps.app/Contents/MacOS/MerginMaps  # macOS
```

### Running Tests

```bash
# Enable tests in cmake with -DENABLE_TESTS=TRUE
# Set environment variables:
export TEST_MERGIN_URL=https://app.dev.merginmaps.com/
export TEST_API_USERNAME=<username>
export TEST_API_PASSWORD=<password>

# Run all tests
ctest

# Run individual test
./MerginMaps --testMerginApi
```

### Code Formatting

```bash
# C++ formatting (required, CI enforced) - uses astyle 3.4.13
./scripts/format_cpp.bash

# CMake formatting
./scripts/format_cmake.bash

# Static analysis
./scripts/cppcheck.bash
```

## Code Architecture

### Source Structure

- `app/` - Main application: UI logic, QML components, platform-specific code
  - `attributes/` - Form attribute handling
  - `qml/` - All QML UI files (2-space indentation)
  - `android/`, `ios/` - Platform-specific resources
- `core/` - Business logic, API client, project management
  - `merginapi.cpp` - Mergin Maps cloud API client
  - `localprojectsmanager.*` - Local project storage
  - `project.*` - Project model
- `gallery/` - UI component development/demo app
- `test/` - Unit tests and test data

### Key Classes

- `MerginApi` - Cloud service API client
- `LocalProjectsManager` - Local project storage management
- `Project` - Project data model
- `GeodiffUtils` - Geographic diff operations

### Architecture Pattern

MVVM with Qt's model-view architecture. C++ handles business logic; QML handles UI.

## Code Style

### C++

Follow QGIS code style: https://docs.qgis.org/3.28/en/docs/developers_guide/codingstandards.html

### QML

Follow https://github.com/Furkanzmc/QML-Coding-Guide with 2-space indentation.

Key rules:
- Root item `id` should be `root`
- Properties ordered: required props, regular props, signals, size/position, other props, attached props, states, signal handlers, visual children, non-visual children, functions
- Only one ternary per line; use if-else blocks for complex conditionals:
```qml
// Bad
width: hasFocus ? 100 : isVisible ? 40 : 10

// Good
width: {
  if (hasFocus) return 100
  else if (isVisible) return 40
  return 10
}
```

## Git Workflow

Trunk-based development on `master`. Use `feature/`, `bugfix/`, `hotfix/` branch prefixes.

PRs require clear descriptions, linked issues, and screenshots/videos for UI changes. C++ changes should include unit tests.

## Secrets

API keys are in `core/merginsecrets.cpp.enc`. Decrypt for development against production/dev servers:
```bash
cd core/
openssl aes-256-cbc -d -in merginsecrets.cpp.enc -out merginsecrets.cpp -md md5
```
