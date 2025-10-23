# GitHub Workflows Documentation

This repository includes two GitHub Actions workflows for continuous integration and release management.

## Build Test Workflow

**File:** `.github/workflows/build-test.yml`

### Purpose
Automatically tests the build process across multiple platforms to ensure code compiles correctly.

### Triggers
- Push to `main` or `develop` branches
- Pull requests targeting `main` or `develop` branches

### Platforms Tested
- **Linux** (Ubuntu latest)
- **Windows** (Windows latest)
- **macOS** (macOS latest)

### What It Does
1. Checks out the repository
2. Sets up CMake build environment
3. Configures CMake with Release build type
4. Builds the project
5. Uploads build artifacts for inspection

### Viewing Results
- Go to the "Actions" tab in GitHub
- Click on a workflow run to see build status for each platform
- Download artifacts to inspect built binaries

## Release Workflow

**File:** `.github/workflows/release.yml`

### Purpose
Creates official releases with source code and pre-compiled executables for all supported platforms.

### Triggers
- **Manual only** - Must be triggered manually via GitHub Actions UI

### How to Create a Release

1. Go to the **Actions** tab in your GitHub repository
2. Select **Release** workflow from the left sidebar
3. Click **Run workflow** button
4. Enter the version tag (e.g., `v1.0.0`, `v1.2.3`)
5. Click **Run workflow** to start

### What It Does

#### Step 1: Create Release
1. Checks out the repository
2. Creates a source code archive (zip)
3. Creates a new GitHub release with the specified version tag
4. Uploads the source code archive to the release

#### Step 2: Build Executables
For each platform (Linux, Windows, macOS):
1. Checks out the repository
2. Configures and builds the project with CMake
3. Packages the executables with documentation:
   - Linux: `invelog-vX.X.X-linux-x64.tar.gz`
   - macOS: `invelog-vX.X.X-macos-x64.tar.gz`
   - Windows: `invelog-vX.X.X-windows-x64.zip`
4. Uploads each package to the release

### Release Assets

Each release includes:
- **Source code** (`invelog-vX.X.X-source.zip`) - Complete source code archive
- **Linux binary** (`invelog-vX.X.X-linux-x64.tar.gz`) - Executables for Linux
- **macOS binary** (`invelog-vX.X.X-macos-x64.tar.gz`) - Executables for macOS
- **Windows binary** (`invelog-vX.X.X-windows-x64.zip`) - Executables for Windows

Each binary package contains:
- `invelog` or `invelog.exe` - Main demo application
- `invelog_server` or `invelog_server.exe` - Database server
- `README.md` - Project documentation
- `docs/` - Additional documentation

## Version Numbering

Follow semantic versioning (SemVer) format:
- `vMAJOR.MINOR.PATCH` (e.g., `v1.0.0`, `v2.1.3`)
- **MAJOR**: Incompatible API changes
- **MINOR**: New functionality (backwards compatible)
- **PATCH**: Bug fixes (backwards compatible)

## Requirements

### For Build Test Workflow
- No special requirements - runs automatically on push/PR

### For Release Workflow
- Requires maintainer access to trigger manual workflow
- GitHub automatically provides `GITHUB_TOKEN` for creating releases
- Build must complete successfully on all platforms

## Troubleshooting

### Build Failures
If builds fail on any platform:
1. Check the workflow logs in the Actions tab
2. Look for compilation errors or missing dependencies
3. Fix the code and push again to re-trigger the build test

### Release Failures
If release creation fails:
1. Check that the version tag doesn't already exist
2. Ensure all builds complete successfully
3. Check workflow logs for specific error messages

### Missing Executables
If executables are missing from release:
- Check that the build produced binaries in expected locations
- Verify CMake output directories match workflow expectations:
  - Linux/macOS: `build/bin/`
  - Windows: `build/bin/Release/`

## Notes

- Build artifacts from the Build Test workflow are kept for 90 days (GitHub default)
- Release assets are kept permanently unless manually deleted
- The workflows use `actions/checkout@v4` and `actions/upload-artifact@v4`
- Deprecated `actions/create-release@v1` and `actions/upload-release-asset@v1` are used for release creation (consider upgrading to newer alternatives in the future)

## Future Enhancements

Potential improvements for these workflows:
- Add automated testing steps
- Add code coverage reporting
- Migrate to newer release actions (e.g., `softprops/action-gh-release`)
- Add checksum generation for release assets
- Add release notes generation from commits
- Add pre-release/draft options
- Add build caching to speed up builds
