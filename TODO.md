# TODO - ocserv-modern Development Tracker

**Last Updated**: 2025-10-29
**Current Sprint**: Sprint 1 (Starting 2025-10-30)

---

## Sprint 0: Foundation ✅ COMPLETED (2025-10-15 to 2025-10-29)

### Completed Tasks ✅
- [x] **US-001**: Upstream analysis - ocserv GitLab repository
- [x] **US-002**: GnuTLS API audit - 94 functions mapped
- [x] **US-003**: TLS abstraction layer design (C23)
- [x] **US-004**: GnuTLS backend implementation (915 lines, 100% tests pass)
- [x] **US-005**: wolfSSL backend implementation (1,287 lines, 82% tests pass)
- [x] Oracle Linux 10 migration (container build environment)
- [x] GitHub Actions update (self-hosted runners)
- [x] Phase 2 documentation (REST API User Stories)
- [x] Unit testing infrastructure (Makefile + tests)
- [x] Test certificate generation

### Sprint 0 Deliverables
- ✅ TLS abstraction API (`src/crypto/tls_abstract.h`)
- ✅ GnuTLS backend (`src/crypto/tls_gnutls.{c,h}`)
- ✅ wolfSSL backend (`src/crypto/tls_wolfssl.{c,h}`)
- ✅ Unit tests (23 tests per backend)
- ✅ Build environment (Oracle Linux 10 container)
- ✅ Sprint summary (`docs/sprints/sprint-0/SPRINT_SUMMARY.md`)

**Sprint 0 Velocity**: 37 story points completed

---

## Sprint 1: PoC Validation and Benchmarking 🔄 IN PROGRESS

**Sprint Goal**: Fix wolfSSL issues, complete PoC testing, establish performance baseline

**Sprint Duration**: 2025-10-30 to 2025-11-12 (2 weeks)

**Planned Story Points**: 34 points

### High Priority Tasks (P0) 🔴

#### 1. Fix wolfSSL Session Creation (8 points) - US-005 Refinement
- [ ] **Fix session creation** - Allocate SSL object correctly
  - Issue: `tls_session_new()` returns NULL
  - Root cause: Missing `wolfSSL_new()` call
  - Location: `src/crypto/tls_wolfssl.c:tls_session_new()`
  - Tests affected: 4 (session_creation, session_set_get_ptr, context_set_session_timeout, dtls_set_get_mtu)

- [ ] **Verify session pointer storage**
  - Fix `tls_session_set_ptr()` / `tls_session_get_ptr()`
  - Use `wolfSSL_set_app_data()` / `wolfSSL_get_app_data()`

- [ ] **Fix session timeout storage**
  - Store timeout in context structure
  - `wolfSSL_CTX_get_timeout()` doesn't return set value

- [ ] **Re-run wolfSSL unit tests**
  - Target: 22/22 tests passing (100%)
  - Verify valgrind clean (no leaks)

**Acceptance Criteria**:
- wolfSSL test suite: 22/22 pass (100%)
- No compilation warnings
- No memory leaks (valgrind)
- Session creation works for TLS and DTLS

---

#### 2. Complete PoC Server (5 points) - US-006
- [ ] **Fix `usleep()` portability**
  - Issue: `usleep()` is POSIX, not C23
  - Solution: Use `nanosleep()` or `thrd_sleep()`
  - Location: `tests/poc/tls_poc_server.c`

- [ ] **Build PoC server with both backends**
  ```bash
  make poc-server BACKEND=gnutls
  make poc-server BACKEND=wolfssl
  ```

- [ ] **Test PoC server functionality**
  - Start server on port 4433
  - Verify listening socket
  - Accept connection test
  - Echo functionality test

- [ ] **Generate test certificates**
  - CA certificate (RSA 2048)
  - Server certificate + key
  - Client certificate + key
  - Store in `tests/certs/`

**Acceptance Criteria**:
- PoC server compiles with both backends
- Server accepts connections
- Echo functionality works
- No crashes or hangs

---

#### 3. Complete PoC Client (3 points) - US-007
- [ ] **Build PoC client with both backends**
  ```bash
  make poc-client BACKEND=gnutls
  make poc-client BACKEND=wolfssl
  ```

- [ ] **Test client-server communication**
  - Client connects to server
  - Send test data
  - Verify echo response
  - Clean disconnect

- [ ] **Test with multiple message sizes**
  - 1 byte
  - 1 KB
  - 16 KB
  - 64 KB

**Acceptance Criteria**:
- PoC client compiles with both backends
- Client-server handshake successful
- Data echo works correctly
- All message sizes handled

---

### Medium Priority Tasks (P1) 🟡

#### 4. Benchmarking Infrastructure (5 points) - US-008
- [ ] **Create benchmark.sh script**
  - Location: `tests/poc/benchmark.sh`
  - Metrics to measure:
    - Handshake rate (connections/sec)
    - Throughput (MB/sec) for various payload sizes
    - CPU usage (%)
    - Memory usage (MB)
    - Latency distribution (p50, p95, p99)

- [ ] **Output format: JSON**
  ```json
  {
    "backend": "gnutls",
    "version": "3.8.9",
    "handshake_rate": 1234.5,
    "throughput": {
      "1KB": 123.4,
      "16KB": 456.7,
      "64KB": 789.0
    },
    "cpu_usage_pct": 45.2,
    "memory_mb": 12.3,
    "latency_ms": {
      "p50": 1.2,
      "p95": 3.4,
      "p99": 5.6
    }
  }
  ```

- [ ] **Create comparison script**
  - Location: `tests/poc/compare.sh`
  - Compare GnuTLS vs wolfSSL results
  - Calculate delta percentages

**Acceptance Criteria**:
- Script runs without errors
- All metrics collected
- JSON output valid
- Repeatable results

---

#### 5. GnuTLS Performance Baseline (2 points) - US-009
- [ ] **Run benchmarks with GnuTLS backend**
  ```bash
  ./tests/poc/benchmark.sh --backend=gnutls --output=gnutls_baseline.json
  ```

- [ ] **Document baseline results**
  - Save to: `docs/benchmarks/gnutls_baseline.json`
  - Document system specs:
    - CPU model, cores, frequency
    - RAM size
    - Kernel version
    - Compiler version

- [ ] **Test multiple cipher suites**
  - ECDHE-RSA-AES128-GCM-SHA256
  - ECDHE-RSA-AES256-GCM-SHA384
  - ECDHE-RSA-CHACHA20-POLY1305

- [ ] **Test multiple payload sizes**
  - 1 KB
  - 16 KB
  - 64 KB
  - 1 MB

**Acceptance Criteria**:
- Baseline results documented
- System specs recorded
- Multiple scenarios tested
- Results reproducible

---

#### 6. wolfSSL PoC Validation (3 points) - US-010
- [ ] **Run benchmarks with wolfSSL backend**
  ```bash
  ./tests/poc/benchmark.sh --backend=wolfssl --output=wolfssl_results.json
  ```

- [ ] **Compare to GnuTLS baseline**
  ```bash
  ./tests/poc/compare.sh gnutls_baseline.json wolfssl_results.json
  ```

- [ ] **Document performance delta**
  - Handshake rate: wolfSSL vs GnuTLS %
  - Throughput: wolfSSL vs GnuTLS %
  - CPU usage: wolfSSL vs GnuTLS %
  - Memory: wolfSSL vs GnuTLS %

- [ ] **Make GO/NO-GO decision**
  - **GO Criteria**: Performance within 10% AND no critical blocking issues
  - **NO-GO Criteria**: Performance regression >10% OR critical compatibility issues

**Acceptance Criteria**:
- All benchmarks completed
- Comparison data documented
- GO/NO-GO decision made
- Decision rationale documented

---

### Optional Tasks (P2) 🟢

#### 7. Rebuild wolfSSL with PSK Support
- [ ] **Rebuild wolfSSL in container**
  ```bash
  ./configure --enable-psk --enable-tls13 --enable-dtls --enable-dtls13 \
              --enable-session-ticket --enable-ocsp
  make && make install
  ```

- [ ] **Verify PSK tests pass**
  - Uncomment PSK tests in `test_tls_wolfssl.c`
  - Re-run unit tests

- [ ] **Update build script**
  - Add PSK flags to `deploy/podman/scripts/build-dev.sh`

---

#### 8. CI/CD Testing
- [ ] **Trigger GitHub Actions workflow**
  - Push changes to trigger build
  - Verify matrix build (GnuTLS + wolfSSL)
  - Check self-hosted runner execution

- [ ] **Verify artifact upload**
  - Container images pushed
  - Test results uploaded

---

## Sprint 2: Priority String Parser (Planned)

**Sprint Goal**: Implement GnuTLS priority string parser for wolfSSL compatibility

**Sprint Duration**: 2025-11-13 to 2025-11-26 (2 weeks)

### Planned Tasks

#### 1. Priority String Parser (13 points) - US-011
- [ ] Design parser grammar (EBNF)
- [ ] Implement lexer/tokenizer
- [ ] Implement parser logic
- [ ] Create mapping table: GnuTLS → wolfSSL cipher names
- [ ] Support keywords: NORMAL, SECURE, PERFORMANCE, NONE, etc.
- [ ] Support modifiers: +, -, %
- [ ] Support version constraints: -VERS-SSL3.0, etc.
- [ ] Unit tests for parser
- [ ] Integration tests with real priority strings

#### 2. Session Cache Implementation (8 points) - US-012
- [ ] Design session cache data structures
- [ ] Implement store/retrieve/remove callbacks
- [ ] Session timeout enforcement
- [ ] Address-based validation (prevent hijacking)
- [ ] Thread-safe cache access
- [ ] Performance testing (>5x handshake improvement)

#### 3. DTLS Support Enhancement (8 points) - US-013
- [ ] DTLS initialization
- [ ] MTU configuration
- [ ] Timeout/retransmission
- [ ] Packet loss handling tests
- [ ] Path MTU discovery

---

## Backlog (Future Sprints)

### Phase 1: Core TLS Migration
- [ ] **US-014**: Certificate Verification (5 points)
- [ ] **US-015**: PSK Support (5 points)
- [ ] **US-016**: Error Handling (3 points)
- [ ] **US-017**: Cisco Client Testing - Basic (5 points)
- [ ] **US-018**: Worker Process Integration (13 points)
- [ ] **US-019**: Security Module Integration (8 points)
- [ ] **US-020**: Main Process Integration (8 points)
- [ ] **US-021**: Build System Integration (5 points)

### Phase 1: Advanced Features
- [ ] **US-022**: OCSP Support (8 points)
- [ ] **US-023**: PKCS#11 Integration (13 points)
- [ ] **US-024**: CRL Support (5 points)
- [ ] **US-025**: TLS 1.3 Optimization (5 points)
- [ ] **US-026**: DTLS 1.3 Support (8 points)

### Phase 1: Finalization
- [ ] **US-027**: Performance Tuning (8 points)
- [ ] **US-028**: Security Audit Preparation (13 points)
- [ ] **US-029**: Cisco Client Testing - Advanced (13 points)
- [ ] **US-030**: Documentation (13 points)

### wolfSSL Ecosystem
- [ ] **US-042**: wolfSentry IDPS Integration (13 points)
- [ ] **US-043**: wolfPKCS11 HSM Support (21 points)
- [ ] **US-044**: wolfCLU Testing Tools Integration (5 points)

### Phase 2: REST API & WebUI
- [ ] **US-045**: REST API Architecture Design (5 points)
- [ ] **US-046**: Basic REST API Implementation (21 points)
- [ ] **US-047**: JWT Authentication (13 points)
- [ ] **US-048**: mTLS Client Certificate Auth (8 points)
- [ ] **US-049**: RBAC Authorization (13 points)
- [ ] **US-050**: Rate Limiting (8 points)
- [ ] **US-051**: Audit Logging (5 points)
- [ ] **US-052**: WebSocket Real-time Monitoring (13 points)
- [ ] **US-053**: WebUI Backend (Go) (21 points)
- [ ] **US-054**: WebUI Frontend (Vue.js 3) (34 points)

---

## Known Issues

### HIGH Priority 🔴
1. **wolfSSL Session Creation Failures**
   - **Impact**: Blocks PoC testing
   - **Status**: Root cause identified
   - **Fix Planned**: Sprint 1, Task 1
   - **Assignee**: ocserv-crypto-refactor agent

### MEDIUM Priority 🟡
2. **usleep() Portability**
   - **Impact**: PoC server won't compile on strict C23
   - **Status**: Fix identified (use nanosleep)
   - **Fix Planned**: Sprint 1, Task 2

3. **wolfSSL PSK Support Disabled**
   - **Impact**: PSK authentication tests fail
   - **Status**: Workaround applied (#ifdef NO_PSK)
   - **Fix Planned**: Sprint 1, Optional Task 7

### LOW Priority 🟢
4. **C23 Auto Keyword Unsupported**
   - **Impact**: Can't use modern C23 syntax
   - **Status**: Workaround applied (explicit types)
   - **Fix Planned**: Wait for GCC updates

---

## Testing Status

### Unit Tests
- **GnuTLS**: ✅ 10/10 (100%)
- **wolfSSL**: ⚠️ 18/22 (82%)

### Integration Tests
- **PoC Server**: ⏳ Not yet tested
- **PoC Client**: ⏳ Not yet tested
- **Benchmarks**: ⏳ Not yet run

### Performance Tests
- **GnuTLS Baseline**: ⏳ Pending
- **wolfSSL Comparison**: ⏳ Pending

---

## Build Status

### Container Images
- ✅ `localhost/ocserv-modern-dev:latest` - Development (Oracle Linux 10)
- ⏳ `localhost/ocserv-modern-test:latest` - Testing (Planned)
- ⏳ `localhost/ocserv-modern-build:latest` - CI/CD Build (Planned)
- ⏳ `localhost/ocserv-modern-ci:latest` - CI/CD Testing (Planned)

### CI/CD Status
- ✅ GitHub Actions workflow configured
- ✅ Self-hosted runners configured
- ⏳ Matrix testing (GnuTLS + wolfSSL) - Not yet triggered
- ⏳ Artifact upload - Not yet tested

---

## Documentation Status

### Completed ✅
- `docs/agile/USER_STORIES.md` - 54 User Stories (399 points total)
- `docs/sprints/sprint-0/SPRINT_SUMMARY.md` - Sprint 0 summary
- `docs/sprints/sprint-0/artifacts/test_results.md` - Unit test report
- `docs/architecture/TLS_ABSTRACTION.md` - TLS abstraction design
- `README.md` - Project overview

### In Progress ⏳
- `TODO.md` - This file
- `docs/ROADMAP.md` - Project roadmap (needs update)

### Planned 📝
- `docs/benchmarks/PERFORMANCE_ANALYSIS.md` - Performance comparison
- `docs/sprints/sprint-1/SPRINT_SUMMARY.md` - Sprint 1 summary
- `docs/architecture/PRIORITY_STRINGS.md` - Priority string parser design
- `docs/deployment/BUILDING.md` - Build instructions

---

## Quick Links

### Documentation
- [User Stories](docs/agile/USER_STORIES.md)
- [Sprint 0 Summary](docs/sprints/sprint-0/SPRINT_SUMMARY.md)
- [Test Results](docs/sprints/sprint-0/artifacts/test_results.md)
- [TLS Abstraction](docs/architecture/TLS_ABSTRACTION.md)

### Source Code
- [TLS Abstract API](src/crypto/tls_abstract.h)
- [GnuTLS Backend](src/crypto/tls_gnutls.c)
- [wolfSSL Backend](src/crypto/tls_wolfssl.c)
- [Unit Tests](tests/unit/)
- [PoC Applications](tests/poc/)

### Build & CI/CD
- [Makefile](Makefile)
- [GitHub Actions](.github/workflows/containers.yml)
- [Build Scripts](deploy/podman/scripts/)

### Git Repository
- GitHub: https://github.com/dantte-lp/ocserv-modern
- Branch: master
- Latest Commit: 41b4d5c

---

**Notes**:
- This TODO is managed manually and should be updated at the end of each sprint
- For real-time task tracking, see GitHub Issues/Projects
- Story points use Fibonacci sequence: 1, 2, 3, 5, 8, 13, 21, 34
