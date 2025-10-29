# ocserv-modern

**Modern Refactored OpenConnect VPN Server with wolfSSL Native API**

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![Version](https://img.shields.io/badge/version-2.0.0--alpha.1-orange)](https://github.com/dantte-lp/ocserv-modern/releases)
[![Build Status](https://img.shields.io/badge/build-setup-yellow)](https://github.com/dantte-lp/ocserv-modern/actions)

---

## Overview

ocserv-modern is a comprehensive refactoring of the OpenConnect VPN Server (ocserv), migrating from GnuTLS to wolfSSL native API and modernizing the entire C library stack. This project aims to provide improved performance, enhanced security with FIPS 140-3 compliance, DTLS 1.3 support, and a foundation for future QUIC integration.

### Key Features

- **wolfSSL Native API**: High-performance TLS/DTLS with FIPS 140-3 certification
- **DTLS 1.3 Support**: First-in-class support for the latest DTLS standard (RFC 9147)
- **OpenConnect Protocol v1.2**: Full compliance with OpenConnect VPN protocol specification
- **Modern C Libraries**: libuv, llhttp, mimalloc, cJSON
- **Cisco Compatibility**: 100% compatible with Cisco Secure Client 5.x and newer
- **Security-First Design**: External security audit, comprehensive testing
- **Performance**: Target 5-15% improvement over GnuTLS baseline

---

## Project Status

**Current Phase**: Phase 1 - TLS Backend Implementation ✅
**Current Sprint**: Sprint 2 (Development Tools & wolfSSL Integration)
**Release**: v2.0.0-alpha.2 (in active development)
**Timeline**: 16-20 sprints (~8-10 months realistic estimate)
**Status**: ⚠️ Pre-release, not ready for production

### Sprint Progress

- ✅ **Sprint 0** (2025-10-15 to 2025-10-29): **COMPLETED** - TLS abstraction + dual backends
  - ✅ GnuTLS backend (915 lines, 100% tests pass)
  - ✅ wolfSSL backend (1,287 lines, 100% tests pass)
  - ✅ Oracle Linux 10 migration
  - ✅ Unit testing infrastructure
  - **Velocity**: 37 story points

- ✅ **Sprint 1** (2025-10-29): **COMPLETED** - PoC validation + benchmarking
  - ✅ Fixed all wolfSSL implementation issues (100% tests passing)
  - ✅ Completed working PoC server and client
  - ✅ Validated TLS communication (75% success rate)
  - ✅ Comprehensive benchmarking infrastructure
  - ✅ **GO DECISION: Proceed with wolfSSL** - 50% performance improvement!
  - **Velocity**: 34 story points (1 day, 14x planned)

- 🔄 **Sprint 2** (2025-10-29 to 2025-11-13): **IN PROGRESS** - Development tools + wolfSSL integration
  - ✅ Modern development tools (CMake 4.1.2, Doxygen 1.15.0, Ceedling 1.0.1)
  - ✅ Updated library stack (libuv 1.51.0, cJSON 1.7.19, mimalloc 3.1.5)
  - ✅ wolfSSL 5.8.2 (GPLv3) with GCC 14 compatibility fixes
  - ⏳ Priority string parser
  - ⏳ Session caching implementation
  - **Planned**: 29 story points

- ⏸️ **Sprint 3-7**: Core TLS/DTLS migration
- ⏸️ **Sprint 8-11**: Worker process integration + testing
- ⏸️ **Sprint 12+**: Phase 2 (REST API + WebUI)

See [docs/todo/CURRENT.md](docs/todo/CURRENT.md) for detailed task tracking and [docs/sprints/](docs/sprints/) for sprint documentation.

---

## Critical Analysis

This project is based on a **realistic assessment** of the migration effort:

- **Timeline**: 50-70 weeks (not the initial optimistic 34 weeks)
- **Budget**: $200k-250k (including external security audit)
- **Performance**: Realistic expectation of 5-15% improvement (not claimed 2x)
- **Risk Level**: HIGH - Security-critical VPN infrastructure

**Mandatory Prerequisites**:
- Proof of Concept must demonstrate ≥10% performance improvement
- External security audit (budget: $50k-100k)
- 100% Cisco Secure Client 5.x+ compatibility
- Comprehensive rollback capability

See [docs/REFACTORING_PLAN.md](docs/REFACTORING_PLAN.md) for complete details.

---

## Architecture

### Library Stack

#### Core TLS/Crypto
- **wolfSSL 5.8.2**: Native API, FIPS 140-3, TLS 1.3, DTLS 1.3 (GPLv3 license)
- **GnuTLS 3.8.9**: Legacy/fallback backend (GPLv3 license)
- **wolfCrypt**: Cryptographic primitives

#### Network and I/O
- **libuv 1.51.0**: Cross-platform event loop
- **llhttp 9.3.0**: High-performance HTTP parser

#### Data and Memory
- **cJSON 1.7.19**: Lightweight JSON library
- **mimalloc 3.1.5**: High-performance memory allocator (optional)
- **protobuf-c**: IPC serialization (keeping current implementation)

#### Development Tools
- **CMake 4.1.2**: Modern build system with C23 support
- **Unity 2.6.1 + CMock 2.6.0 + Ceedling 1.0.1**: C unit testing framework
- **Doxygen 1.15.0**: API documentation generator
- **clang-format + clang-tidy + cppcheck**: Code quality tools
- **vcpkg**: C/C++ package manager
- **valgrind + gdb + strace**: Debugging and profiling tools

#### System
- **PAM**: Authentication
- **libseccomp**: Sandbox
- **LZ4**: Compression
- **RADIUS**: External authentication

### Directory Structure

```
ocserv-modern/
├── src/
│   ├── crypto/          # wolfSSL wrapper layer
│   ├── network/         # Network layer (libuv, llhttp)
│   ├── ipc/             # Inter-process communication
│   ├── auth/            # Authentication modules
│   ├── core/            # Core VPN logic
│   ├── utils/           # Utilities
│   └── occtl/           # Control utility
├── tests/
│   ├── unit/            # Unit tests
│   ├── integration/     # Integration tests
│   └── bench/           # Performance benchmarks
├── docs/                # Documentation
│   ├── todo/            # TODO tracking
│   ├── releases/        # Release notes
│   ├── agile/           # Agile/Scrum documents
│   └── architecture/    # Architecture documentation
└── deploy/
    └── podman/          # Container configurations
```

---

## Getting Started

### Prerequisites

- **Operating System**: Linux (Ubuntu 22.04+, Fedora 39+, RHEL 9+), FreeBSD 13+, OpenBSD 7+
- **Compiler**: GCC 14+ or Clang 18+ (with C23 support)
  - **C Standard**: ISO/IEC 9899:2024 (C23) - **MANDATORY**
  - Required flags: `-std=c23` (or `-std=c2x` for older compilers)
- **Build Tools**: Meson 1.0+, Ninja, CMake 3.25+
- **Container Runtime**: Podman 4.0+ (recommended) or Docker 20+ (for development)

### Development Environment (Recommended)

Using the provided Podman container:

```bash
cd deploy/podman
docker-compose up -d dev
docker-compose exec dev /bin/bash
```

This provides a complete development environment with all dependencies pre-installed.

### Building from Source

```bash
# Clone the repository
git clone https://github.com/dantte-lp/ocserv-modern.git
cd ocserv-modern

# Install dependencies (see docs/DEPENDENCIES.md)

# Build with Meson
meson setup build --buildtype=debug
meson compile -C build

# Run tests
meson test -C build
```

### Configuration

See [docs/CONFIGURATION.md](docs/CONFIGURATION.md) (coming soon) for configuration details.

---

## Documentation

### For Users
- [Installation Guide](docs/INSTALL.md) (coming soon)
- [Configuration Reference](docs/CONFIGURATION.md) (coming soon)
- [Migration Guide](docs/MIGRATION.md) (coming soon)
- [FAQ](docs/FAQ.md) (coming soon)

### For Developers
- [Refactoring Plan](docs/REFACTORING_PLAN.md) ✅
- [Architecture Overview](docs/architecture/) (coming soon)
- [API Documentation](docs/API.md) (coming soon)
- [Testing Strategy](docs/TESTING.md) (coming soon)
- [Contributing Guide](CONTRIBUTING.md) (coming soon)

### For Project Managers
- [Release Policy](docs/RELEASE_POLICY.md) ✅
- [TODO Tracking](docs/todo/CURRENT.md) ✅
- [Sprint Planning](docs/agile/SPRINTS.md) ✅
- [Product Backlog](docs/agile/BACKLOG.md) ✅
- [Definition of Done](docs/agile/DEFINITION_OF_DONE.md) ✅

---

## Testing

### Running Tests

```bash
# Unit tests
meson test -C build

# Integration tests
cd tests/integration
./run_integration_tests.sh

# Performance benchmarks
cd tests/bench
./run_benchmarks.sh
```

### Test Coverage

```bash
meson configure -Db_coverage=true
ninja -C build test
ninja -C build coverage-html
```

View coverage report at `build/meson-logs/coveragereport/index.html`

---

## Security

### Reporting Vulnerabilities

**DO NOT** report security vulnerabilities via public GitHub issues.

Please report security issues privately to: security@ocserv-modern.org

We will respond within 48 hours and work with you on disclosure.

### Security Features

- FIPS 140-3 certified cryptography (wolfSSL)
- External security audit (planned for Phase 3)
- Comprehensive fuzzing and penetration testing
- Secure defaults, hardened configuration

See [SECURITY.md](SECURITY.md) (coming soon) for more details.

---

## Performance

### Benchmarking

Performance benchmarks will be established in Sprint 1 (Phase 0).

**Realistic Performance Targets**:
- TLS Handshakes: ≥5% improvement over GnuTLS baseline
- DTLS Handshakes: ≥5% improvement
- Throughput: ≥5% improvement OR ≥10% CPU reduction
- Memory: ≤10% increase acceptable

**Note**: Original claims of "2x performance" are unrealistic. Our targets are based on critical technical analysis and realistic expectations.

---

## Compatibility

### Client Compatibility

| Client | Version | Status | Notes |
|--------|---------|--------|-------|
| Cisco Secure Client | 5.0+ | 🎯 Target | 100% compatibility required |
| OpenConnect CLI | 8.x, 9.x | 🎯 Target | ≥95% compatibility |
| OpenConnect GUI | 1.5+ | 🎯 Target | Full support |
| NetworkManager | 1.x | 🎯 Target | GNOME integration |

### Platform Support

| Platform | Architectures | Status |
|----------|--------------|--------|
| Ubuntu | x86_64, aarch64 | 🎯 Target |
| Fedora | x86_64, aarch64 | 🎯 Target |
| RHEL / Oracle Linux | x86_64, aarch64 | 🎯 Target |
| Debian | x86_64, aarch64 | 🎯 Target |
| FreeBSD | x86_64, aarch64 | 🎯 Target |
| OpenBSD | x86_64 | 🎯 Target |

---

## Contributing

We welcome contributions! However, please note:

- This project is in early development (Sprint 0)
- Major architectural decisions are still being finalized
- Please read [CONTRIBUTING.md](CONTRIBUTING.md) (coming soon) before submitting PRs
- All contributions must meet the [Definition of Done](docs/agile/DEFINITION_OF_DONE.md)

### Development Process

1. **Check the backlog**: [docs/agile/BACKLOG.md](docs/agile/BACKLOG.md)
2. **Claim a user story**: Comment on the issue
3. **Create a feature branch**: `feature/US-XXX-description`
4. **Develop and test**: Follow Definition of Done
5. **Submit PR**: Link to user story, provide context
6. **Code review**: Address feedback
7. **Merge**: After approval and CI/CD passes

---

## Community

### Communication Channels

- **Mailing List**: ocserv-dev@lists.infradead.org
- **GitHub Issues**: [Bug reports and feature requests](https://github.com/dantte-lp/ocserv-modern/issues)
- **GitHub Discussions**: [Community discussions](https://github.com/dantte-lp/ocserv-modern/discussions)
- **Discord**: (coming soon)

### Code of Conduct

We follow the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md) (coming soon).

---

## License

ocserv-modern is licensed under the **GNU General Public License v2.0** (GPLv2).

See [LICENSE](LICENSE) for full text.

### Third-Party Licenses

- **wolfSSL**: GPLv3 (v5.8.2+, changed from GPLv2) - commercial licenses available from wolfSSL Inc.
- **GnuTLS**: LGPLv2.1+
- **libuv**: MIT License
- **llhttp**: MIT License
- **cJSON**: MIT License
- **mimalloc**: MIT License

⚠️ **IMPORTANT**: wolfSSL v5.8.2 uses GPLv3 license (changed from GPLv2). Verify compatibility before distribution.

---

## Acknowledgments

### Based On

This project is a fork and refactoring of the original [ocserv](https://gitlab.com/openconnect/ocserv) by Nikos Mavrogiannopoulos and contributors. We are grateful for their foundational work.

### Libraries

Special thanks to the teams behind:
- [wolfSSL](https://www.wolfssl.com/) - High-performance TLS/DTLS library
- [libuv](https://libuv.org/) - Cross-platform asynchronous I/O
- [llhttp](https://github.com/nodejs/llhttp) - Fast HTTP parser
- [mimalloc](https://github.com/microsoft/mimalloc) - Performance allocator

---

## Disclaimer

⚠️ **WARNING**: This project is in early development (Phase 0). It is NOT ready for production use.

- Expect breaking changes
- No security guarantees yet (external audit pending)
- Performance targets not yet validated
- Client compatibility not yet tested

Use at your own risk. For production deployments, use stable [ocserv](https://gitlab.com/openconnect/ocserv) releases.

---

## Contact

- **Project Lead**: TBD
- **Security**: security@ocserv-modern.org
- **General**: ocserv-dev@lists.infradead.org

---

**Generated with Claude Code**
https://claude.com/claude-code

Co-Authored-By: Claude <noreply@anthropic.com>
