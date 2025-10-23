# Workflow Diagram

## Build Test Workflow (CI)

```
┌─────────────────────────────────────────────────────────────┐
│                    Build Test Workflow                      │
│                  (Automatic on Push/PR)                     │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │     Triggers on Push/PR to:             │
        │     • main branch                       │
        │     • develop branch                    │
        └─────────────────────────────────────────┘
                              │
                              ▼
        ┌────────────────────────────────────────────────┐
        │          Matrix Build (Parallel)               │
        │   ┌──────────┬──────────┬──────────┐          │
        │   │  Ubuntu  │ Windows  │  macOS   │          │
        │   │  Latest  │  Latest  │  Latest  │          │
        │   └──────────┴──────────┴──────────┘          │
        └────────────────────────────────────────────────┘
                              │
                              ▼
        ┌────────────────────────────────────────────────┐
        │         Build Steps (Per Platform):            │
        │  1. Checkout code                              │
        │  2. Setup CMake                                │
        │  3. Configure CMake (Release)                  │
        │  4. Build with CMake                           │
        │  5. Upload artifacts                           │
        └────────────────────────────────────────────────┘
                              │
                              ▼
        ┌────────────────────────────────────────────────┐
        │              Artifacts Uploaded:               │
        │  • invelog-ubuntu-latest-Release               │
        │  • invelog-windows-latest-Release              │
        │  • invelog-macos-latest-Release                │
        └────────────────────────────────────────────────┘
```

## Release Workflow (Manual)

```
┌─────────────────────────────────────────────────────────────┐
│                      Release Workflow                       │
│                    (Manual Trigger Only)                    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │     User Triggers Workflow:             │
        │     • Goes to Actions tab               │
        │     • Clicks "Release" workflow         │
        │     • Enters version (e.g., v1.0.0)     │
        │     • Clicks "Run workflow"             │
        └─────────────────────────────────────────┘
                              │
                              ▼
┌───────────────────────────────────────────────────────────────┐
│                    JOB 1: Create Release                      │
├───────────────────────────────────────────────────────────────┤
│  Steps:                                                       │
│  1. Checkout code                                             │
│  2. Create source archive (zip)                               │
│  3. Create GitHub release with tag                            │
│  4. Upload source archive to release                          │
│                                                               │
│  Output:                                                      │
│  • Release URL for uploading assets                           │
│  • Version tag                                                │
└───────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌───────────────────────────────────────────────────────────────┐
│          JOB 2: Build Release (Matrix - Parallel)             │
├───────────────────────────────────────────────────────────────┤
│  ┌──────────────┬──────────────┬──────────────┐              │
│  │    Linux     │   Windows    │    macOS     │              │
│  └──────────────┴──────────────┴──────────────┘              │
│                                                               │
│  For Each Platform:                                           │
│  1. Checkout code                                             │
│  2. Setup CMake                                               │
│  3. Configure & Build (Release)                               │
│  4. Package binaries + docs                                   │
│  5. Upload to release                                         │
└───────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌────────────────────────────────────────────────┐
        │            Release Assets Created:             │
        │                                                │
        │  📦 invelog-vX.X.X-source.zip                  │
        │  📦 invelog-vX.X.X-linux-x64.tar.gz            │
        │  📦 invelog-vX.X.X-macos-x64.tar.gz            │
        │  📦 invelog-vX.X.X-windows-x64.zip             │
        └────────────────────────────────────────────────┘
```

## Workflow Files Structure

```
.github/
├── workflows/
│   ├── build-test.yml       # CI workflow - automatic
│   └── release.yml          # Release workflow - manual
├── WORKFLOWS.md             # User documentation
├── SETUP_SUMMARY.md         # Technical summary
└── WORKFLOW_DIAGRAM.md      # This file - visual guide
```

## Key Points

### Build Test Workflow
- **Trigger:** Automatic on push/PR
- **Branches:** main, develop
- **Platforms:** Linux, Windows, macOS (parallel)
- **Purpose:** Continuous Integration - ensure code builds
- **Output:** Build artifacts (temporary, 90 days)

### Release Workflow
- **Trigger:** Manual (workflow_dispatch)
- **Input Required:** Version tag (e.g., v1.0.0)
- **Jobs:** 2 (sequential: create-release → build-release)
- **Platforms:** Linux, Windows, macOS (parallel in job 2)
- **Purpose:** Create versioned releases with executables
- **Output:** GitHub release with downloadable assets

## Dependencies Flow

```
Build Dependencies (Auto-fetched by CMake):
    ↓
┌─────────────────────────┐
│   FetchContent from:    │
│   • nlohmann/json       │
│   • cpp-httplib         │
│   • SQLite3 (if needed) │
└─────────────────────────┘
    ↓
┌─────────────────────────┐
│   Build Libraries:      │
│   • invelog_lib         │
│   • invelog_server_lib  │
└─────────────────────────┘
    ↓
┌─────────────────────────┐
│   Build Executables:    │
│   • invelog             │
│   • invelog_server      │
└─────────────────────────┘
```

## Usage Quick Reference

| Action | Trigger | Result |
|--------|---------|--------|
| Push to main/develop | Automatic | Build test runs on all platforms |
| Create PR to main/develop | Automatic | Build test runs on all platforms |
| Manual release workflow | Manual (Actions UI) | Creates release with binaries |

## Monitoring

- **View Runs:** GitHub → Actions tab
- **Build Logs:** Click on workflow run → Click on job
- **Artifacts:** Click on workflow run → Scroll to "Artifacts"
- **Releases:** GitHub → Releases section
