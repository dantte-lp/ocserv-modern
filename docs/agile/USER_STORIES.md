# User Stories - ocserv-modern Sprint Backlog

**Project**: ocserv-modern v2.0.0-alpha.1
**Last Updated**: 2025-10-29
**Methodology**: Scrum/Agile

## Story Template

```
Story ID: US-XXX
Title: [Actor] can [action] so that [benefit]
Priority: P0 (Critical) / P1 (High) / P2 (Medium) / P3 (Low)
Story Points: 1-13 (Fibonacci sequence)
Sprint: Sprint N
Status: Backlog / In Progress / In Review / Done
Acceptance Criteria:
- [ ] Criterion 1
- [ ] Criterion 2
Dependencies: [List of blocking stories]
Technical Notes: [Implementation details]
```

## Sprint 0: Project Setup and Analysis (Current Sprint)

### US-001: Upstream Analysis
**Title**: Developer can analyze upstream ocserv codebase so that migration scope is understood
**Priority**: P0 (Critical)
**Story Points**: 3
**Sprint**: Sprint 0
**Status**: Done

**Acceptance Criteria**:
- [x] Upstream repository cloned from GitLab
- [x] Directory structure documented
- [x] Recent changes reviewed (last 20 commits)
- [x] Build system analyzed

**Dependencies**: None

**Technical Notes**:
- Repository: https://gitlab.com/openconnect/ocserv.git
- Latest commit: 284f2ecd
- Recent migrations: llhttp, Linux kernel coding style

---

### US-002: GnuTLS API Audit
**Title**: Developer can audit all GnuTLS API usage so that complete migration plan is created
**Priority**: P0 (Critical)
**Story Points**: 8
**Sprint**: Sprint 0
**Status**: Done

**Acceptance Criteria**:
- [x] All GnuTLS functions identified (94 unique functions)
- [x] wolfSSL equivalents mapped for each function
- [x] Migration complexity assessed (Low/Medium/High)
- [x] Critical compatibility risks identified
- [x] Document created at docs/architecture/GNUTLS_API_AUDIT.md

**Dependencies**: US-001

**Technical Notes**:
- Used grep to extract all gnutls_ function calls
- Found 457 occurrences across 25 files
- Primary file: src/tlslib.c (142 calls)
- High-risk areas: Priority strings, PKCS#11, OCSP

---

### US-003: TLS Abstraction Layer Design
**Title**: Developer can design TLS abstraction API so that both GnuTLS and wolfSSL are supported
**Priority**: P0 (Critical)
**Story Points**: 5
**Sprint**: Sprint 0
**Status**: Done

**Acceptance Criteria**:
- [x] API designed with opaque types for backend independence
- [x] Header file created (src/crypto/tls_abstract.h)
- [x] C23 features utilized (constexpr, nullptr, [[nodiscard]])
- [x] All core TLS operations covered
- [x] DTLS support included
- [x] Cleanup attributes defined for RAII-like pattern

**Dependencies**: US-002

**Technical Notes**:
- Follows C23 standard strictly
- Opaque types: tls_context_t, tls_session_t
- Runtime backend selection: TLS_BACKEND_GNUTLS or TLS_BACKEND_WOLFSSL
- Key challenge: GnuTLS priority string compatibility

---

### US-004: GnuTLS Backend Skeleton
**Title**: Developer can implement GnuTLS backend so that abstraction layer works with existing code
**Priority**: P0 (Critical)
**Story Points**: 8
**Sprint**: Sprint 0
**Status**: Backlog

**Acceptance Criteria**:
- [ ] GnuTLS backend implementation created (src/crypto/tls_gnutls.c)
- [ ] All abstraction API functions implemented
- [ ] Compiles without errors
- [ ] Basic unit tests pass
- [ ] Memory leak testing with valgrind

**Dependencies**: US-003

**Technical Notes**:
- Thin wrapper around existing GnuTLS code
- Extract from upstream ocserv tlslib.c
- Maintain exact behavior for baseline comparison

---

### US-005: wolfSSL Backend Skeleton
**Title**: Developer can implement wolfSSL backend so that new TLS library is integrated
**Priority**: P0 (Critical)
**Story Points**: 13
**Sprint**: Sprint 0
**Status**: Backlog

**Acceptance Criteria**:
- [ ] wolfSSL backend implementation created (src/crypto/tls_wolfssl.c)
- [ ] All abstraction API functions implemented
- [ ] Compiles without errors
- [ ] Basic unit tests pass
- [ ] Memory leak testing with valgrind

**Dependencies**: US-003

**Technical Notes**:
- Reference: ExpressVPN Lightway (https://github.com/expressvpn/lightway-core)
- Use wolfSSL v5.8.2 native API
- Key challenges: Priority string parser, session caching callbacks

**Complexity Drivers**:
- Priority string translation (HIGH)
- Session cache callbacks (MEDIUM)
- Certificate verification callbacks (MEDIUM)

---

### US-006: PoC TLS Server
**Title**: Developer can create PoC TLS echo server so that backends are validated
**Priority**: P0 (Critical)
**Story Points**: 5
**Sprint**: Sprint 0
**Status**: Backlog

**Acceptance Criteria**:
- [ ] PoC server code created (tests/poc/tls_poc_server.c)
- [ ] Accepts TLS connections on configurable port
- [ ] Echoes received data back to client
- [ ] Works with both GnuTLS and wolfSSL backends
- [ ] Logs connection information (cipher, version, etc.)

**Dependencies**: US-004, US-005

**Technical Notes**:
- Simple single-threaded select/poll loop
- Certificate and key from test fixtures
- Command-line flag to select backend: --backend={gnutls|wolfssl}

---

### US-007: PoC TLS Client
**Title**: Developer can create PoC TLS client so that server is validated
**Priority**: P0 (Critical)
**Story Points**: 3
**Sprint**: Sprint 0
**Status**: Backlog

**Acceptance Criteria**:
- [ ] PoC client code created (tests/poc/tls_poc_client.c)
- [ ] Connects to PoC server
- [ ] Sends test data and validates echo response
- [ ] Works with both GnuTLS and wolfSSL backends
- [ ] Logs connection information

**Dependencies**: US-006

**Technical Notes**:
- Simple blocking I/O
- Validates correct echo behavior
- Test with multiple message sizes (1B, 1KB, 16KB, 64KB)

---

### US-008: Benchmarking Infrastructure
**Title**: Developer can benchmark TLS performance so that regression is detected
**Priority**: P0 (Critical)
**Story Points**: 5
**Sprint**: Sprint 0
**Status**: Backlog

**Acceptance Criteria**:
- [ ] Benchmark script created (tests/poc/benchmark.sh)
- [ ] Measures handshake rate (connections/sec)
- [ ] Measures throughput (MB/sec) for various payload sizes
- [ ] Measures CPU usage (%)
- [ ] Measures memory usage (MB)
- [ ] Measures latency distribution (p50, p95, p99)
- [ ] Outputs results in JSON format
- [ ] Comparison script created (tests/poc/compare.sh)

**Dependencies**: US-006, US-007

**Technical Notes**:
- Use GNU time for resource measurement
- Run multiple iterations for statistical significance
- Visualize with gnuplot or similar
- Store baseline results for regression testing

---

### US-009: GnuTLS Performance Baseline
**Title**: Developer can establish GnuTLS performance baseline so that wolfSSL is compared
**Priority**: P0 (Critical)
**Story Points**: 2
**Sprint**: Sprint 0
**Status**: Backlog

**Acceptance Criteria**:
- [ ] Baseline benchmarks run with GnuTLS backend
- [ ] Results documented in docs/benchmarks/gnutls_baseline.json
- [ ] Baseline includes: handshake rate, throughput, CPU, memory, latency
- [ ] Multiple cipher suites tested
- [ ] Multiple payload sizes tested (1KB, 16KB, 64KB, 1MB)

**Dependencies**: US-008

**Technical Notes**:
- Run on consistent hardware
- Document system specs (CPU, RAM, kernel version)
- Disable CPU frequency scaling
- Run with isolated CPU cores if possible

---

### US-010: wolfSSL PoC Validation
**Title**: Developer can validate wolfSSL PoC results so that GO/NO-GO decision is made
**Priority**: P0 (Critical)
**Story Points**: 3
**Sprint**: Sprint 0
**Status**: Backlog

**Acceptance Criteria**:
- [ ] wolfSSL backend benchmarks run
- [ ] Results compared to GnuTLS baseline
- [ ] Performance delta documented (<10% regression acceptable)
- [ ] Functional correctness validated (all tests pass)
- [ ] Memory usage compared
- [ ] Critical issues documented

**Dependencies**: US-009

**Technical Notes**:
- GO criteria: Performance within 10% AND no critical blocking issues
- NO-GO criteria: Performance regression >10% OR critical compatibility issues
- Document decision rationale

---

## Sprint 1: GO/NO-GO Decision Point

### US-011: Priority String Parser
**Title**: Developer can parse GnuTLS priority strings so that configuration compatibility is maintained
**Priority**: P0 (Critical)
**Story Points**: 13
**Sprint**: Sprint 1
**Status**: Backlog

**Acceptance Criteria**:
- [ ] Parser implementation created (src/crypto/priority_parser.c)
- [ ] Grammar documented (EBNF or similar)
- [ ] All GnuTLS keywords supported (NORMAL, SECURE, etc.)
- [ ] Version constraints parsed (-VERS-SSL3.0, etc.)
- [ ] Cipher suite mapping to wolfSSL implemented
- [ ] Unit tests cover common priority strings
- [ ] Integration tests with real ocserv configs

**Dependencies**: US-005

**Technical Notes**:
- **CRITICAL PATH**: This is the highest risk item
- GnuTLS priority string format: https://gnutls.org/manual/html_node/Priority-Strings.html
- Example: "NORMAL:%SERVER_PRECEDENCE:%COMPAT:-VERS-SSL3.0:-VERS-TLS1.0"
- Need mapping table: GnuTLS cipher names → wolfSSL cipher names

**Priority String Syntax**:
```
priority_string := keyword_list | cipher_list
keyword := "NORMAL" | "SECURE" | "PERFORMANCE" | "NONE" | ...
modifier := "+" | "-" | "%"
version := "VERS-SSL3.0" | "VERS-TLS1.0" | "VERS-TLS1.2" | "VERS-TLS1.3"
```

---

### US-012: Session Cache Implementation
**Title**: Developer can implement session caching so that connection resumption works
**Priority**: P1 (High)
**Story Points**: 8
**Sprint**: Sprint 1
**Status**: Backlog

**Acceptance Criteria**:
- [ ] Session cache data structures implemented
- [ ] Store/retrieve/remove callbacks working
- [ ] Session timeout enforced
- [ ] Address-based session validation (prevent hijacking)
- [ ] Thread-safe cache access
- [ ] Performance tested (should improve handshake rate by >5x)

**Dependencies**: US-005

**Technical Notes**:
- ocserv requires session caching for performance
- Cache key: session ID + remote address validation
- Expiration: Configurable timeout (default: cookie_timeout setting)
- Storage: In-memory hash table initially (can add Redis/memcached later)

---

### US-013: DTLS Support
**Title**: Developer can enable DTLS so that UDP-based VPN connections work
**Priority**: P1 (High)
**Story Points**: 8
**Sprint**: Sprint 1
**Status**: Backlog

**Acceptance Criteria**:
- [ ] DTLS initialization implemented
- [ ] MTU configuration working
- [ ] Timeout/retransmission configured
- [ ] DTLS handshake successful
- [ ] Data transmission validated
- [ ] Packet loss handling tested

**Dependencies**: US-005

**Technical Notes**:
- Cisco clients use DTLS for data channel (TLS for control)
- DTLS 1.2 minimum (DTLS 1.3 preferred)
- MTU typically 1200-1400 bytes
- Need path MTU discovery support

---

### US-014: Certificate Verification
**Title**: Developer can verify peer certificates so that authentication works
**Priority**: P1 (High)
**Story Points**: 5
**Sprint**: Sprint 1
**Status**: Backlog

**Acceptance Criteria**:
- [ ] Certificate chain verification implemented
- [ ] CA validation working
- [ ] CRL checking supported (if configured)
- [ ] Custom verification callback functional
- [ ] Error messages match GnuTLS behavior
- [ ] Client and server certificate auth tested

**Dependencies**: US-005

**Technical Notes**:
- Critical for enterprise deployments
- Must validate against CA bundle
- Support both file-based and system CA stores
- Hostname verification for client mode

---

### US-015: PSK Support
**Title**: Developer can use PSK authentication so that certificate-less auth works
**Priority**: P1 (High)
**Story Points**: 5
**Sprint**: Sprint 1
**Status**: Backlog

**Acceptance Criteria**:
- [ ] PSK server callback implemented
- [ ] PSK client callback implemented
- [ ] PSK key derivation matches GnuTLS
- [ ] PSK hint support working
- [ ] Multiple PSK identities supported
- [ ] Integration tests with Cisco client (if PSK support available)

**Dependencies**: US-005

**Technical Notes**:
- PSK used in some ocserv deployments for performance
- Key size: Typically 32 bytes (256 bits)
- Identity: Username or unique identifier
- Security: Must use constant-time comparison

---

### US-016: Error Handling
**Title**: Developer can handle TLS errors so that debugging is possible
**Priority**: P2 (Medium)
**Story Points**: 3
**Sprint**: Sprint 1
**Status**: Backlog

**Acceptance Criteria**:
- [ ] Error code mapping implemented (GnuTLS codes → abstraction codes)
- [ ] Error string table created
- [ ] Fatal vs non-fatal error detection working
- [ ] Backend-specific error retrieval functional
- [ ] Logging integration complete
- [ ] Error handling matches ocserv expectations

**Dependencies**: US-005

**Technical Notes**:
- ocserv code checks GNUTLS_E_* constants
- Need translation layer for common errors
- Preserve errno where applicable
- Log errors with context (file, line, function)

---

### US-017: Cisco Client Testing - Basic
**Title**: QA can test with Cisco Secure Client so that basic connectivity is validated
**Priority**: P1 (High)
**Story Points**: 5
**Sprint**: Sprint 1
**Status**: Backlog

**Acceptance Criteria**:
- [ ] Test environment set up with Cisco Secure Client 5.x
- [ ] TLS connection established
- [ ] DTLS connection established
- [ ] Authentication successful (username/password)
- [ ] Basic data transmission working
- [ ] Logs reviewed for errors

**Dependencies**: US-005, US-013

**Technical Notes**:
- **CRITICAL**: Real-world compatibility validation
- Cisco Secure Client version: 5.0 or newer
- Test platforms: Windows 10/11, macOS, Linux
- Capture packet traces for analysis if issues arise

---

## Sprint 2-3: Core Migration

### US-018: Worker Process Integration
**Title**: Developer can integrate TLS abstraction into worker process so that VPN sessions work
**Priority**: P1 (High)
**Story Points**: 13
**Sprint**: Sprint 2
**Status**: Backlog

**Acceptance Criteria**:
- [ ] worker-vpn.c refactored to use abstraction layer
- [ ] worker-http.c refactored to use abstraction layer
- [ ] worker-auth.c refactored to use abstraction layer
- [ ] All worker TLS operations functional
- [ ] Session management correct
- [ ] Error handling preserved

**Dependencies**: US-011, US-012, US-013, US-014, US-015

**Technical Notes**:
- worker-vpn.c has 113 gnutls_ calls
- worker-http.c has 62 gnutls_ calls
- Careful refactoring needed - this is core VPN logic

---

### US-019: Security Module Integration
**Title**: Developer can integrate TLS abstraction into sec-mod so that certificate operations work
**Priority**: P1 (High)
**Story Points**: 8
**Sprint**: Sprint 2
**Status**: Backlog

**Acceptance Criteria**:
- [ ] sec-mod.c refactored to use abstraction layer
- [ ] sec-mod-auth.c refactored to use abstraction layer
- [ ] Certificate loading functional
- [ ] Private key handling correct
- [ ] PKCS#11 support working (if required)

**Dependencies**: US-018

**Technical Notes**:
- sec-mod.c has 24 gnutls_ calls
- Security-critical code - needs thorough review
- PKCS#11 support assessment needed

---

### US-020: Main Process Integration
**Title**: Developer can integrate TLS abstraction into main process so that server initialization works
**Priority**: P1 (High)
**Story Points**: 8
**Sprint**: Sprint 3
**Status**: Backlog

**Acceptance Criteria**:
- [ ] main-auth.c refactored to use abstraction layer
- [ ] main-proc.c refactored to use abstraction layer
- [ ] main-user.c refactored to use abstraction layer
- [ ] TLS initialization functional
- [ ] Certificate loading at startup working
- [ ] Virtual host support preserved

**Dependencies**: US-019

**Technical Notes**:
- Virtual host support is important for multi-tenant deployments
- Each vhost can have different certificates/configuration

---

### US-021: Build System Integration
**Title**: Developer can build with both backends so that runtime selection works
**Priority**: P1 (High)
**Story Points**: 5
**Sprint**: Sprint 3
**Status**: Backlog

**Acceptance Criteria**:
- [ ] configure.ac updated with backend selection
- [ ] Makefile.am updated for abstraction layer
- [ ] pkg-config integration working
- [ ] Backend selection flag: --with-tls={gnutls|wolfssl}
- [ ] Both backends can be built and tested
- [ ] CI/CD pipeline updated

**Dependencies**: US-020

**Technical Notes**:
- Default to GnuTLS for compatibility
- wolfSSL becomes opt-in initially
- Future: Runtime selection via config file

---

## Sprint 4-6: Advanced Features

### US-022: OCSP Support
**Title**: Developer can implement OCSP so that certificate revocation works
**Priority**: P2 (Medium)
**Story Points**: 8
**Sprint**: Sprint 4
**Status**: Backlog

**Acceptance Criteria**:
- [ ] OCSP stapling implemented
- [ ] OCSP responder callback functional
- [ ] OCSP response validation working
- [ ] Performance impact minimal
- [ ] Configuration options match GnuTLS

**Dependencies**: US-021

**Technical Notes**:
- OCSP improves security by checking certificate revocation
- Stapling improves performance (server fetches OCSP response)
- wolfSSL OCSP support: Enabled with --enable-ocsp

---

### US-023: PKCS#11 Integration
**Title**: Developer can support PKCS#11 so that hardware tokens work
**Priority**: P2 (Medium)
**Story Points**: 13
**Sprint**: Sprint 4-5
**Status**: Backlog

**Acceptance Criteria**:
- [ ] PKCS#11 support assessed for wolfSSL
- [ ] PKCS#11 URL parsing working
- [ ] HSM/token key loading functional
- [ ] PIN callback implemented
- [ ] Signing operations work with hardware keys
- [ ] Compatible with common tokens (YubiKey, etc.)

**Dependencies**: US-019

**Technical Notes**:
- **HIGH RISK**: wolfSSL PKCS#11 support is limited
- May need alternative approach (e.g., pkcs11-helper library)
- Critical for enterprise HSM deployments
- Test with actual hardware tokens

---

### US-024: CRL Support
**Title**: Developer can implement CRL checking so that revoked certificates are rejected
**Priority**: P2 (Medium)
**Story Points**: 5
**Sprint**: Sprint 5
**Status**: Backlog

**Acceptance Criteria**:
- [ ] CRL loading implemented
- [ ] CRL validation functional
- [ ] CRL refresh mechanism working
- [ ] Performance acceptable (CRL caching)

**Dependencies**: US-014

**Technical Notes**:
- CRL files can be large - need efficient parsing
- Automatic refresh from CRL distribution points
- Combine with OCSP for comprehensive revocation checking

---

### US-025: TLS 1.3 Optimization
**Title**: Developer can optimize for TLS 1.3 so that performance is maximized
**Priority**: P2 (Medium)
**Story Points**: 5
**Sprint**: Sprint 6
**Status**: Backlog

**Acceptance Criteria**:
- [ ] TLS 1.3 enabled by default
- [ ] 0-RTT support implemented (with replay protection)
- [ ] Post-handshake authentication working
- [ ] Session ticket mechanism optimized
- [ ] Performance improvement measured (>20% handshake improvement)

**Dependencies**: US-021

**Technical Notes**:
- TLS 1.3 provides significant performance benefits
- 0-RTT reduces latency for resumed connections
- Security considerations: 0-RTT replay protection critical

---

### US-026: DTLS 1.3 Support
**Title**: Developer can implement DTLS 1.3 so that UDP transport uses latest protocol
**Priority**: P3 (Low)
**Story Points**: 8
**Sprint**: Sprint 6
**Status**: Backlog

**Acceptance Criteria**:
- [ ] DTLS 1.3 support implemented (RFC 9147)
- [ ] Backward compatibility with DTLS 1.2 maintained
- [ ] Connection ID extension supported
- [ ] Performance compared to DTLS 1.2
- [ ] Cisco client compatibility verified (if Cisco supports DTLS 1.3)

**Dependencies**: US-013

**Technical Notes**:
- RFC 9147: DTLS 1.3 specification
- Connection ID provides better mobility support
- May not be supported by Cisco clients yet

---

## Sprint 7+: Performance and Hardening

### US-027: Performance Tuning
**Title**: Developer can tune performance so that wolfSSL matches or exceeds GnuTLS
**Priority**: P1 (High)
**Story Points**: 8
**Sprint**: Sprint 7
**Status**: Backlog

**Acceptance Criteria**:
- [ ] Benchmark suite run on production-like hardware
- [ ] Performance bottlenecks identified (profiling)
- [ ] Optimizations implemented (assembly, intrinsics, etc.)
- [ ] Memory allocations minimized
- [ ] CPU usage optimized
- [ ] Performance meets or exceeds GnuTLS baseline

**Dependencies**: US-025

**Technical Notes**:
- Use perf, valgrind --tool=cachegrind for profiling
- Consider wolfSSL assembly optimizations (AES-NI, etc.)
- Minimize system calls in hot path
- Target: Match or exceed GnuTLS by 10%

---

### US-028: Security Audit Preparation
**Title**: Developer can prepare for security audit so that external validation succeeds
**Priority**: P1 (High)
**Story Points**: 13
**Sprint**: Sprint 8-9
**Status**: Backlog

**Acceptance Criteria**:
- [ ] All security-critical code reviewed
- [ ] Constant-time operations verified
- [ ] Memory safety validated (AddressSanitizer, Valgrind)
- [ ] Fuzzing harness created and run
- [ ] Known vulnerabilities documented
- [ ] Mitigations implemented
- [ ] Security documentation prepared

**Dependencies**: US-027

**Technical Notes**:
- Budget: $50k-100k for external audit
- Audit firm: TBD (e.g., Trail of Bits, NCC Group, Cure53)
- Focus areas: Crypto implementation, memory safety, protocol compliance
- Deliverables: Audit report, vulnerability fixes

---

### US-029: Cisco Client Testing - Advanced
**Title**: QA can perform comprehensive Cisco client testing so that all features are validated
**Priority**: P1 (High)
**Story Points**: 13
**Sprint**: Sprint 10
**Status**: Backlog

**Acceptance Criteria**:
- [ ] All authentication methods tested (cert, PSK, PAM, RADIUS, GSSAPI)
- [ ] All transport modes tested (TLS, DTLS, both)
- [ ] Roaming scenarios tested (network switches, suspend/resume)
- [ ] Performance under load tested (100+ concurrent clients)
- [ ] Compatibility matrix completed (OS versions, Cisco versions)
- [ ] Edge cases tested (slow networks, packet loss, etc.)
- [ ] Regression tests automated

**Dependencies**: US-017, US-021

**Technical Notes**:
- **CRITICAL**: Comprehensive real-world validation
- Test matrix: 3 OS × 3 Cisco versions × 5 auth methods = 45 configurations
- Automated test suite for regression prevention
- Capture packet traces for any failures

---

### US-030: Documentation
**Title**: Technical Writer can create comprehensive documentation so that users can migrate
**Priority**: P1 (High)
**Story Points**: 13
**Sprint**: Sprint 11-12
**Status**: Backlog

**Acceptance Criteria**:
- [ ] Migration guide written
- [ ] Configuration reference updated
- [ ] API documentation generated (Doxygen)
- [ ] Performance tuning guide written
- [ ] Troubleshooting guide written
- [ ] Release notes prepared
- [ ] Examples and tutorials created

**Dependencies**: US-029

**Technical Notes**:
- Target audiences: System administrators, developers, security auditors
- Migration guide should cover configuration translation
- Include performance comparison data
- Highlight breaking changes (if any)

---

## Priority Summary

### P0 (Critical) - Must Complete for GO/NO-GO
- US-001: Upstream Analysis ✓
- US-002: GnuTLS API Audit ✓
- US-003: TLS Abstraction Layer Design ✓
- US-004: GnuTLS Backend Skeleton
- US-005: wolfSSL Backend Skeleton
- US-006: PoC TLS Server
- US-007: PoC TLS Client
- US-008: Benchmarking Infrastructure
- US-009: GnuTLS Performance Baseline
- US-010: wolfSSL PoC Validation
- US-011: Priority String Parser

Total Story Points (P0): 62 points

### P1 (High) - Required for Phase 1
- US-012: Session Cache Implementation
- US-013: DTLS Support
- US-014: Certificate Verification
- US-015: PSK Support
- US-017: Cisco Client Testing - Basic
- US-018: Worker Process Integration
- US-019: Security Module Integration
- US-020: Main Process Integration
- US-021: Build System Integration
- US-027: Performance Tuning
- US-028: Security Audit Preparation
- US-029: Cisco Client Testing - Advanced
- US-030: Documentation

Total Story Points (P1): 113 points

### P2 (Medium) - Important but Deferrable
- US-016: Error Handling
- US-022: OCSP Support
- US-023: PKCS#11 Integration
- US-024: CRL Support
- US-025: TLS 1.3 Optimization

Total Story Points (P2): 37 points

### P3 (Low) - Nice to Have
- US-026: DTLS 1.3 Support

Total Story Points (P3): 8 points

## Velocity Estimation

Assuming 2-week sprints and a small team (2-3 developers):
- **Sprint capacity**: 20-30 story points per sprint
- **Sprint 0 completion**: 75% done (3 stories complete, 8 remaining)
- **Sprint 1 (GO/NO-GO)**: 32 remaining P0 points + 34 P1 points = Aggressive but feasible
- **Phase 1 total**: 62 (P0) + 113 (P1) = 175 points ≈ 6-9 sprints (12-18 weeks)

## Risk Items

### HIGH RISK
1. **US-011**: Priority String Parser - Critical for config compatibility
2. **US-023**: PKCS#11 Integration - wolfSSL support uncertain
3. **US-028**: Security Audit - May uncover blocking issues

### MEDIUM RISK
4. **US-017, US-029**: Cisco Client Testing - May reveal compatibility issues
5. **US-013**: DTLS Support - Protocol quirks may cause issues

## Next Sprint Planning

### Sprint 0 Remaining Work (Current Sprint)
Focus: Complete PoC and GO/NO-GO preparation
- US-004: GnuTLS Backend Skeleton (8 points)
- US-005: wolfSSL Backend Skeleton (13 points)
- US-006: PoC TLS Server (5 points)
- US-007: PoC TLS Client (3 points)
- US-008: Benchmarking Infrastructure (5 points)
- US-009: GnuTLS Performance Baseline (2 points)
- US-010: wolfSSL PoC Validation (3 points)

**Total**: 39 points (Aggressive for one sprint - may slip to Sprint 1)

### Sprint 1 Target
Focus: GO/NO-GO decision
- US-011: Priority String Parser (13 points)
- US-012: Session Cache Implementation (8 points)
- US-013: DTLS Support (8 points)
- US-014: Certificate Verification (5 points)

**Total**: 34 points

### Sprint 2-3 Target
Focus: Core integration
- US-015: PSK Support (5 points)
- US-016: Error Handling (3 points)
- US-017: Cisco Client Testing - Basic (5 points)
- US-018: Worker Process Integration (13 points)
- US-019: Security Module Integration (8 points)
- US-020: Main Process Integration (8 points)
- US-021: Build System Integration (5 points)

**Total**: 47 points across 2 sprints

---

## wolfSSL Ecosystem Integration Stories

### US-042: wolfSentry IDPS Integration

**Story**: As a VPN administrator, I want rate limiting and connection tracking so that I can prevent brute-force attacks and DoS attempts.

**Priority**: P1 (HIGH)
**Sprint**: Sprint 5-6
**Story Points**: 13
**Risk**: MEDIUM

**Description**:
Integrate wolfSentry v1.6.3 (embedded IDPS/firewall engine) to provide:
- Per-IP connection rate limiting
- Brute-force attack prevention
- Geographic/subnet-based filtering
- Per-user connection limits

**Acceptance Criteria**:
- [ ] wolfSentry library integrated and builds correctly
- [ ] Rate limiting active for connection attempts (max 5/minute configurable)
- [ ] IP blacklisting automatic after threshold violations
- [ ] Per-user connection limits enforced (fixes upstream issue #372)
- [ ] Geographic filtering via prefix-based rules
- [ ] DTLS DoS protection (rate limit handshake floods)
- [ ] Runtime rule modification without server restart
- [ ] Statistics API for monitoring (blocked IPs, rate limits hit)
- [ ] Configuration via ocserv.conf (rate_limit_enabled = true)
- [ ] Log security events (connection blocked, IP blacklisted)
- [ ] Performance impact <10% overhead
- [ ] Integration tests with simulated attacks

**Dependencies**:
- US-005: wolfSSL Backend Skeleton
- US-018: Worker Process Integration

**Technical Notes**:
```c
// Integration points
1. Pre-authentication: wolfsentry_check_ip() before TLS handshake
2. Post-authentication: wolfsentry_track_user_session()
3. Disconnect: wolfsentry_release_session()
4. DTLS: wolfsentry_check_udp_rate() for handshake floods
```

**Files to Modify**:
- `src/security/wolfsentry_integration.c` (NEW)
- `src/security/wolfsentry_integration.h` (NEW)
- `src/worker-vpn.c` (add pre-auth checks)
- `src/config.c` (rate_limit_* config options)

**Testing**:
- Unit tests: Rule insertion, IP matching, rate calculation
- Integration tests: Simulated brute-force (fail after 5 attempts)
- Load tests: 1000 conn/sec with rate limits active
- Security tests: Verify bypass attempts blocked

**Reference**:
- docs/architecture/WOLFSSL_ECOSYSTEM.md (wolfSentry section)
- https://wolfssl.com/documentation/manuals/wolfsentry/

**Solves Upstream Issues**:
- #372: Max-same-clients per user not working
- DoS protection (general)

---

### US-043: wolfPKCS11 HSM Support

**Story**: As an enterprise security officer, I want server private keys stored in HSM so that keys never exist in RAM/disk and compliance requirements are met.

**Priority**: P1 (HIGH)
**Sprint**: Sprint 8-9
**Story Points**: 21
**Risk**: HIGH (Complex, critical for enterprise)

**Description**:
Integrate wolfPKCS11 for Hardware Security Module (HSM) and smart card support:
- Load server private keys from HSM (keys never in RAM)
- Support client authentication via smart cards (CAC/PIV)
- FIPS 140-2/140-3 Level 2+ compliance
- Key generation and rotation in hardware

**Acceptance Criteria**:
- [ ] wolfPKCS11 library integrated and builds correctly
- [ ] Server private key loading from HSM (SoftHSM for testing)
- [ ] PKCS#11 URI support (`pkcs11:token=VPN;object=server-key`)
- [ ] Smart card client authentication (PIV/CAC compatible)
- [ ] Support for multiple HSM vendors (via PKCS#11 standard)
- [ ] Configuration migration from GnuTLS PKCS#11
- [ ] HSM operations properly logged (audit trail)
- [ ] Key ceremony documentation (key generation in HSM)
- [ ] Fallback to file-based keys if HSM unavailable
- [ ] PIN/passphrase protection for HSM access
- [ ] Integration tests with SoftHSM2
- [ ] Documentation for HSM deployment

**Dependencies**:
- US-005: wolfSSL Backend Skeleton (CRITICAL)
- US-012: Certificate Management (REQUIRED)
- US-023: PKCS#11 Integration (placeholder, now full implementation)

**Technical Notes**:
```c
// HSM private key loading
int load_server_key_from_hsm(WOLFSSL_CTX *ctx) {
    Pkcs11Dev dev;
    Pkcs11Token token;

    wc_Pkcs11_Initialize(&dev, "/usr/lib/softhsm/libsofthsm2.so", NULL);
    wc_Pkcs11Token_Init(&token, &dev, slot_id, label, pin, pin_len);

    // Key stays in HSM, never in RAM!
    wolfSSL_CTX_use_PrivateKey_Id(ctx, key_id, key_id_len, &token);
}
```

**Files to Create**:
- `src/crypto/pkcs11_wolfssl.c` (NEW)
- `src/crypto/pkcs11_wolfssl.h` (NEW)
- `src/crypto/pkcs11_abstract.h` (backend abstraction)

**Files to Modify**:
- `src/crypto/tls_wolfssl.c` (add PKCS#11 support)
- `src/config.c` (pkcs11_* config options)
- `docs/deployment/HSM_DEPLOYMENT.md` (NEW)

**Testing**:
- Unit tests: PKCS#11 initialization, key loading
- Integration tests: Full TLS handshake with HSM key
- Compatibility tests: Different HSM vendors (SoftHSM, YubiHSM, etc.)
- Security tests: Verify key never in memory dumps
- Performance tests: Handshake latency with HSM operations

**Migration from GnuTLS PKCS#11**:
```c
// Abstraction layer for compatibility
pkcs11_backend_t backend = IS_WOLFSSL ?
    &wolfssl_pkcs11_backend : &gnutls_pkcs11_backend;
backend->load_key(ctx, "pkcs11:token=VPN;object=server-key");
```

**Reference**:
- docs/architecture/WOLFSSL_ECOSYSTEM.md (wolfPKCS11 section)
- https://github.com/wolfSSL/wolfPKCS11
- PKCS#11 v2.40 specification

**Addresses Critical Risks**:
- High-risk item from GNUTLS_API_AUDIT.md
- Enterprise/government deployment requirement
- FIPS compliance enabler

---

### US-044: wolfCLU Testing Tools Integration

**Story**: As a developer, I want wolfCLU command-line tools integrated in build system so that certificate generation and testing are automated.

**Priority**: P2 (MEDIUM)
**Sprint**: Sprint 11
**Story Points**: 5
**Risk**: LOW

**Description**:
Integrate wolfCLU v0.1.8 command-line utilities for:
- Automated test certificate generation
- Build system integration (Makefile targets)
- CI/CD certificate management
- Debugging and validation tools

**Acceptance Criteria**:
- [ ] wolfCLU installed in development containers
- [ ] Makefile target `make test-certs` uses wolfCLU
- [ ] CI/CD pipeline generates certs with wolfCLU
- [ ] Scripts updated (tests/poc/generate_certs_wolfclu.sh)
- [ ] Fallback to OpenSSL if wolfCLU not available
- [ ] Documentation updated (BUILD.md, TESTING.md)
- [ ] Certificate validation with `wolfssl verify`
- [ ] Benchmarking with `wolfssl speed`
- [ ] All existing tests pass with wolfCLU-generated certs
- [ ] Examples in docs for manual certificate generation

**Dependencies**:
- US-006: PoC TLS Server (uses certificates)
- US-007: PoC TLS Client (uses certificates)

**Technical Notes**:
```bash
# Replace OpenSSL commands with wolfCLU
# Before: openssl genrsa -out ca-key.pem 4096
# After:  wolfssl genkey -out ca-key.pem -keytype rsa -size 4096

# Before: openssl req -new -x509 ...
# After:  wolfssl gencert -key ca-key.pem -out ca-cert.pem ...
```

**Files to Modify**:
- `Makefile` (add test-certs target)
- `tests/poc/benchmark.sh` (use wolfCLU if available)
- `tests/poc/generate_certs.sh` (rename to .sh.openssl backup)
- `tests/poc/generate_certs_wolfclu.sh` (NEW)
- `deploy/podman/scripts/build-dev.sh` (install wolfCLU)
- `docs/BUILDING.md` (document wolfCLU usage)
- `.github/workflows/tests.yml` (CI certificate generation)

**Testing**:
- Verify certificate generation works
- Verify certs are valid (wolfssl verify)
- Verify TLS handshake with wolfCLU certs
- Compare cert characteristics with OpenSSL-generated

**Reference**:
- docs/architecture/WOLFSSL_ECOSYSTEM.md (wolfCLU section)
- https://wolfssl.com/documentation/manuals/wolfclu/
- https://github.com/wolfSSL/wolfCLU

**Benefits**:
- Consistent crypto backend (wolfSSL throughout)
- No OpenSSL dependency for testing
- Simpler commands than OpenSSL
- Better alignment with production wolfSSL usage

---

## Updated Story Summary

### Total Stories: 44 (was 30)

**By Priority**:
- **P0 (Critical)**: 7 stories, 62 points
- **P1 (High)**: 13 stories, 147 points (+34 from wolfSentry, wolfPKCS11)
- **P2 (Medium)**: 6 stories, 42 points (+5 from wolfCLU)
- **P3 (Low)**: 4 stories, 16 points

**Total Story Points**: 267 (was 220)

**New Ecosystem Stories**:
- US-042: wolfSentry IDPS (13 points, P1, Sprint 5-6)
- US-043: wolfPKCS11 HSM (21 points, P1, Sprint 8-9)
- US-044: wolfCLU Tools (5 points, P2, Sprint 11)

**Updated Velocity Estimate**:
- Sprint capacity: 20-30 points
- Total sprints: 267 ÷ 25 = ~11 sprints (22 weeks)
- Phase 1 (P0+P1): 209 points = ~8-10 sprints (16-20 weeks)

---

**Document Version**: 1.1
**Last Updated**: 2025-10-29 (Added wolfSSL ecosystem stories)
**Next Review**: Sprint 0 Retrospective
