# GitHub Actions Workflows

This directory contains GitHub Actions workflows for the Invelog project.

## Quick Links

- 📖 **[Workflow Documentation](WORKFLOWS.md)** - Complete user guide
- 🔧 **[Setup Summary](SETUP_SUMMARY.md)** - Technical implementation details
- 📊 **[Workflow Diagrams](WORKFLOW_DIAGRAM.md)** - Visual flowcharts and diagrams

## Workflows

### 🔄 Build Test (CI)
**File:** `workflows/build-test.yml`

Automatically tests builds on Linux, Windows, and macOS when code is pushed or PRs are created.

- **Trigger:** Automatic on push/PR to main or develop branches
- **Platforms:** Ubuntu, Windows, macOS
- **Purpose:** Ensure code compiles across all platforms

### 🚀 Release
**File:** `workflows/release.yml`

Creates official releases with source code and platform-specific executables.

- **Trigger:** Manual (Actions tab → Release → Run workflow)
- **Input:** Version tag (e.g., v1.0.0)
- **Output:** GitHub release with 4 assets (source + 3 platform binaries)

## Getting Started

### For Contributors
Push your code or create a PR - the build test workflow runs automatically!

### For Maintainers
To create a release:
1. Go to **Actions** tab
2. Select **Release** workflow
3. Click **Run workflow**
4. Enter version (e.g., `v1.0.0`)
5. Click **Run workflow** button

## Documentation Files

| File | Purpose |
|------|---------|
| `WORKFLOWS.md` | Complete user guide with troubleshooting |
| `SETUP_SUMMARY.md` | Technical implementation details |
| `WORKFLOW_DIAGRAM.md` | Visual flowcharts and quick reference |
| `README.md` | This file - overview and quick links |

## Security

- ✅ CodeQL verified - 0 alerts
- ✅ Explicit GITHUB_TOKEN permissions set
- ✅ Minimal required permissions used

## Support

Questions or issues with workflows? Check the documentation files linked above or open an issue in the repository.
