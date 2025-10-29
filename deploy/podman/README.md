# ocserv-modern Podman Container Infrastructure

Production-grade containerized development environment for ocserv-modern using **Podman**, **Buildah**, **Skopeo**, and **crun**.

## Quick Start

```bash
# 1. Build all containers
make build-all

# 2. Start development environment
make dev

# 3. Run tests
make test

# 4. Build release artifacts
make build
```

## Table of Contents

- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Container Environments](#container-environments)
- [Usage](#usage)
- [Makefile Commands](#makefile-commands)
- [Volume Management](#volume-management)
- [Image Registry Operations](#image-registry-operations)
- [Troubleshooting](#troubleshooting)
- [Documentation](#documentation)

## Overview

Four specialized container environments:

| Environment | Purpose | Base | Size Target | Use Case |
|-------------|---------|------|-------------|----------|
| **dev** | Development | UBI9 | <500MB | Interactive development, debugging |
| **test** | Testing | dev | <500MB | Running tests, generating coverage |
| **build** | Build | UBI9-minimal | <200MB | Creating release artifacts |
| **ci** | CI/CD | build | <200MB | Automated testing in pipelines |

### Key Features

- **Rootless by default**: Enhanced security
- **SELinux-compatible**: Proper volume labeling
- **Latest libraries**: wolfSSL 5.8.2 (GPLv3), libuv 1.51.0, llhttp 9.3.0, cJSON 1.7.18, mimalloc 2.2.4
- **crun runtime**: Faster startup
- **Production-ready**: Security hardening, resource limits

## Prerequisites

### Required Software

```bash
# Oracle Linux 10 / RHEL 10
sudo dnf install podman buildah skopeo crun

# Podman Compose
pip3 install podman-compose
```

### Rootless Setup

```bash
# Enable user namespaces
sudo sysctl -w user.max_user_namespaces=28633

# Configure subuid/subgid
sudo usermod --add-subuids 100000-165535 --add-subgids 100000-165535 $USER

# Install dependencies
sudo dnf install fuse-overlayfs slirp4netns container-selinux

# Verify
./scripts/verify-rootless.sh
./scripts/verify-selinux.sh
```

## Installation

```bash
cd /opt/projects/repositories/ocserv-modern/deploy/podman

# Build all containers (8-12 minutes)
make build-all

# Verify
make info
```

## Container Environments

### Development (dev)

**Features**: Full toolchain, debug symbols, gdb, valgrind, perf

```bash
# Start interactive session
make dev

# Inside container
meson setup build --buildtype=debug
ninja -C build
```

### Test (test)

**Features**: Test frameworks, coverage tools, network utilities

```bash
# Run all tests
make test

# Generate coverage
podman-compose run --rm test bash -c "ninja -C build coverage-html"
```

### Build (build)

**Features**: Release-optimized (O3, LTO), packaging tools

```bash
# Build release
make release

# Output: artifacts/ocserv-modern-YYYYMMDD.tar.gz
```

### CI (ci)

**Features**: Lightweight, automated testing, quick validation

```bash
# Full CI pipeline
make ci

# Quick validation
make quick
```

## Usage

### Development Workflow

```bash
# Start dev container
make dev

# Edit code on host (changes visible immediately)
vim ../../src/main.c

# Rebuild in container
ninja -C build
```

### Testing Workflow

```bash
# Run tests with coverage
make full-test

# Run benchmarks
make benchmark

# Valgrind
podman-compose run --rm test bash -c "meson test -C build --wrap='valgrind --leak-check=full'"
```

### Release Workflow

```bash
make release
ls -lh artifacts/
```

## Makefile Commands

### Build

```bash
make build-all       # Build all images
make build-dev       # Development image
make build-test      # Test image
make build-build     # Build image
make build-ci        # CI image
```

### Run

```bash
make dev             # Start development
make test            # Run tests
make build           # Build release
make ci              # Run CI
```

### Shell Access

```bash
make shell-dev       # Interactive dev shell
make shell-test      # Interactive test shell
make shell-build     # Interactive build shell
make shell-ci        # Interactive CI shell
```

### Validation

```bash
make lint            # Code quality checks
make validate        # Quick validation
make verify-rootless # Verify rootless setup
make verify-selinux  # Verify SELinux
```

### Cleanup

```bash
make clean           # Remove containers + build cache
make clean-images    # Remove built images
make clean-all       # Full cleanup (WARNING!)
make prune           # Safe cleanup
```

### Advanced

```bash
make info            # System information
make rebuild-all     # Rebuild from scratch
make quick           # Quick build + test
make full-test       # Full test suite
make benchmark       # Performance tests
```

## Volume Management

### Volumes

- **dev-home**: Developer home (persistent)
- **build-cache**: Meson cache (persistent)
- **test-results**: Test outputs
- **ci-reports**: CI reports

### Backup/Restore

```bash
# Backup
make volumes-backup

# Restore from latest
make volumes-restore

# Restore from specific backup
./scripts/restore-volumes.sh volumes/backups/20251029_143000/
```

## Image Registry Operations

### Push to Registry

```bash
# Login
podman login ghcr.io

# Push all
make push-all

# Push specific image
./scripts/push-image.sh localhost/ocserv-modern-dev:latest ghcr.io/dantte-lp/ocserv-modern-dev:latest
```

### Inspect Images

```bash
# Inspect all
make inspect-all

# Inspect specific
skopeo inspect containers-storage:localhost/ocserv-modern-dev:latest
```

## Troubleshooting

### Permission Denied

```bash
./scripts/verify-rootless.sh
grep "^$USER:" /etc/subuid /etc/subgid
```

### SELinux Denials

```bash
./scripts/verify-selinux.sh
sudo ausearch -m avc -ts recent | grep container
chcon -R -t container_file_t /opt/projects/repositories/ocserv-modern
```

### Build Failures

```bash
make clean
make build-all
df -h
podman system prune -af
```

See [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) for detailed guide.

## Documentation

- [CONTAINER_ARCHITECTURE.md](docs/CONTAINER_ARCHITECTURE.md) - Design and architecture
- [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) - Common issues
- [Configuration Files](config/) - Runtime configuration

## License Notice

**IMPORTANT**: wolfSSL v5.8.2 uses GPLv3 license (changed from GPLv2).
Verify compatibility before distribution.

---

Generated with Claude Code - https://claude.com/claude-code
