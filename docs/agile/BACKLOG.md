# Product Backlog - ocserv-modern

**Project**: ocserv-modern v2.0.0
**Product Owner**: TBD
**Last Updated**: 2025-10-29

---

## Backlog Management

### Priority Levels

- **P0 (Critical)**: Must have for release, blocks other work
- **P1 (High)**: Should have for release, important features
- **P2 (Medium)**: Nice to have for release, can be deferred
- **P3 (Low)**: Future consideration, not for v2.0.0

### Story Point Scale (Fibonacci)

- **1 point**: Few hours of work, well-understood
- **2 points**: Half day to 1 day
- **3 points**: 1-2 days
- **5 points**: 2-4 days
- **8 points**: 1 week
- **13 points**: 2 weeks (should be split)
- **21+ points**: Too large, must be split into smaller stories

---

## Epic 1: Project Foundation (Sprint 0-1)

### User Stories

#### US-001: As a developer, I need a working development environment
**Priority**: P0 (Critical)
**Story Points**: 5
**Status**: IN PROGRESS
**Sprint**: Sprint 0

**Description**:
Set up complete development infrastructure including repository, containers, CI/CD, and documentation structure.

**Acceptance Criteria**:
- [ ] GitHub repository created with proper permissions
- [x] Directory structure matches architecture design
- [x] Podman containers build successfully
- [ ] CI/CD pipeline runs basic build
- [x] Documentation templates in place
- [ ] All team members can build and run project

**Dependencies**: None

**Notes**: Foundation for all future work

---

#### US-002: As a project manager, I need a realistic project plan
**Priority**: P0 (Critical)
**Story Points**: 8
**Status**: IN PROGRESS
**Sprint**: Sprint 0

**Description**:
Create comprehensive project plan based on critical analysis, addressing unrealistic expectations and defining clear milestones.

**Acceptance Criteria**:
- [x] Critical analysis document reviewed
- [x] Realistic timeline documented (50-70 weeks)
- [x] Risk register created with mitigations
- [x] Release policy defined
- [x] Success metrics established
- [ ] Stakeholder approval obtained

**Dependencies**: None

**Notes**: Must address overly optimistic original timeline

---

#### US-003: As a team, we need to understand current ocserv architecture
**Priority**: P0 (Critical)
**Story Points**: 13
**Status**: DONE
**Sprint**: Sprint 0

**Description**:
Analyze upstream ocserv codebase to understand architecture, GnuTLS usage, and potential migration challenges.

**Acceptance Criteria**:
- [x] All GnuTLS API calls identified and documented (94 unique functions, 457 occurrences)
- [x] Certificate handling flows mapped
- [x] DTLS implementation understood
- [x] Multi-process architecture documented
- [x] IPC patterns analyzed
- [x] Potential migration issues identified

**Dependencies**: US-001 (need dev environment)

**Completed**: 2025-10-29
**Deliverables**:
- docs/architecture/GNUTLS_API_AUDIT.md (comprehensive audit)
- Upstream analysis complete
- Migration complexity assessed

**Notes**: Critical for accurate effort estimation - COMPLETED

---

#### US-004: As a team, we need performance baselines before starting
**Priority**: P0 (Critical)
**Story Points**: 8
**Status**: PLANNED
**Sprint**: Sprint 1

**Description**:
Establish performance baselines for current GnuTLS-based ocserv to validate migration benefits.

**Acceptance Criteria**:
- [ ] Benchmarking framework implemented
- [ ] TLS handshake rate measured
- [ ] DTLS handshake rate measured
- [ ] Throughput (Gbps) measured
- [ ] CPU usage profiled
- [ ] Memory usage documented
- [ ] Bottlenecks identified

**Dependencies**: US-001, US-003

**Notes**: Essential for GO/NO-GO decision

---

#### US-005: As a stakeholder, I need proof that migration is worthwhile
**Priority**: P0 (Critical)
**Story Points**: 13
**Status**: PLANNED
**Sprint**: Sprint 1

**Description**:
Develop Proof of Concept demonstrating wolfSSL performance improvements over GnuTLS baseline.

**Acceptance Criteria**:
- [ ] Minimal TLS connection PoC implemented
- [ ] wolfSSL handshake benchmarked
- [ ] Performance comparison report created
- [ ] ≥10% improvement demonstrated (or PoC fails)
- [ ] Security review of PoC approach completed
- [ ] GO/NO-GO decision made

**Dependencies**: US-004 (need baseline)

**Notes**: **PROJECT GATE**: If PoC fails, project stops

---

## Epic 2: TLS Abstraction and Infrastructure (Sprint 2-3)

### User Stories

#### US-006: As a developer, I need a TLS abstraction layer
**Priority**: P0 (Critical)
**Story Points**: 13
**Status**: PLANNED
**Sprint**: Sprint 2

**Description**:
Create TLS library abstraction enabling dual-build with both GnuTLS and wolfSSL backends.

**Acceptance Criteria**:
- [ ] Abstract TLS interface defined
- [ ] GnuTLS backend implemented (wraps existing)
- [ ] wolfSSL backend implemented (basic)
- [ ] Dual-build system functional
- [ ] Unit tests pass with both backends
- [ ] Performance overhead <5%

**Dependencies**: US-005 (PoC approved)

**Notes**: Foundation for safe migration

---

#### US-007: As a developer, I need comprehensive testing infrastructure
**Priority**: P0 (Critical)
**Story Points**: 8
**Status**: PLANNED
**Sprint**: Sprint 3

**Description**:
Establish testing framework for unit, integration, security, and compatibility testing.

**Acceptance Criteria**:
- [ ] Unit test framework (Check) integrated
- [ ] Integration test harness created
- [ ] Performance test automation working
- [ ] Client compatibility test suite defined
- [ ] CI/CD runs all tests automatically
- [ ] Code coverage reporting enabled

**Dependencies**: US-006

**Notes**: Critical for quality assurance

---

## Epic 3: Core TLS Migration (Sprint 4-7)

### User Stories

#### US-008: As a user, I want TLS connections to use wolfSSL
**Priority**: P0 (Critical)
**Story Points**: 21 (SPLIT INTO SUB-STORIES)
**Status**: PLANNED
**Sprint**: Sprint 4-7

**Description**:
Migrate all TLS connection handling from GnuTLS to wolfSSL native API.

**Sub-Stories**:
- US-008a: wolfSSL wrapper layer implementation (8 SP)
- US-008b: worker-vpn.c TLS migration (8 SP)
- US-008c: Certificate authentication migration (5 SP)

**Acceptance Criteria**:
- [ ] All TLS code uses wolfSSL (via abstraction)
- [ ] OpenConnect clients can connect
- [ ] All authentication methods work
- [ ] Session resumption functional
- [ ] TLS 1.3 enabled and tested
- [ ] No functional regressions

**Dependencies**: US-006, US-007

**Notes**: Core migration work

---

#### US-009: As a user, I want DTLS 1.3 support
**Priority**: P0 (Critical)
**Story Points**: 13
**Status**: PLANNED
**Sprint**: Sprint 6-7

**Description**:
Migrate DTLS to wolfSSL, enabling DTLS 1.3 support (major new feature).

**Acceptance Criteria**:
- [ ] DTLS 1.2 migrated and functional
- [ ] DTLS 1.3 implemented and tested
- [ ] Cookie handling for DoS protection
- [ ] Connection ID (CID) support
- [ ] Tested under packet loss conditions
- [ ] Performance acceptable under network issues

**Dependencies**: US-008

**Notes**: Key differentiator vs GnuTLS

---

## Epic 4: Testing and Validation (Sprint 8-11)

### User Stories

#### US-010: As a security engineer, I need external security audit
**Priority**: P0 (Critical)
**Story Points**: 21
**Status**: PLANNED
**Sprint**: Sprint 9-10

**Description**:
Conduct external security audit of wolfSSL migration by qualified security firm.

**Acceptance Criteria**:
- [ ] Security audit vendor contracted
- [ ] Audit scope defined
- [ ] Full code review completed
- [ ] Penetration testing performed
- [ ] Fuzzing campaign executed (72+ hours)
- [ ] Audit report received
- [ ] Zero critical vulnerabilities
- [ ] <3 high-severity issues (all mitigated)

**Dependencies**: US-008, US-009 (migration complete)

**Notes**: MANDATORY, budgeted at $50k-100k

---

#### US-011: As a Cisco user, I need my Cisco Secure Client to work
**Priority**: P0 (Critical)
**Story Points**: 13
**Status**: PLANNED
**Sprint**: Sprint 10-11

**Description**:
Validate 100% compatibility with Cisco Secure Client 5.x and newer.

**Acceptance Criteria**:
- [ ] Cisco Secure Client 5.0-5.5 tested
- [ ] All authentication methods work
- [ ] TLS and DTLS modes functional
- [ ] Windows, macOS, Linux clients tested
- [ ] Split tunneling works
- [ ] Always-on VPN works
- [ ] 100% compatibility achieved

**Dependencies**: US-008, US-009

**Notes**: **RELEASE BLOCKER** if fails

---

#### US-012: As a developer, I need all platforms tested
**Priority**: P1 (High)
**Story Points**: 8
**Status**: PLANNED
**Sprint**: Sprint 11

**Description**:
Test ocserv-modern on all supported platforms to ensure portability.

**Acceptance Criteria**:
- [ ] Ubuntu 22.04, 24.04 tested
- [ ] Fedora 39, 40 tested
- [ ] RHEL 9.x tested
- [ ] Debian 12 tested
- [ ] FreeBSD 13.x, 14.x tested
- [ ] OpenBSD 7.x tested
- [ ] All tests pass on all platforms

**Dependencies**: US-008, US-009

**Notes**: Platform support matrix

---

## Epic 5: Optimization and Quality (Sprint 12-14)

### User Stories

#### US-013: As a performance engineer, I need optimized code
**Priority**: P1 (High)
**Story Points**: 8
**Status**: PLANNED
**Sprint**: Sprint 12

**Description**:
Profile and optimize wolfSSL integration to meet performance targets.

**Acceptance Criteria**:
- [ ] Hot paths profiled (perf, flamegraphs)
- [ ] Critical paths optimized
- [ ] Benchmarks run and validated
- [ ] ≥5% handshake improvement achieved
- [ ] ≥5% throughput improvement OR ≥10% CPU reduction
- [ ] No memory usage increase

**Dependencies**: US-008, US-009

**Notes**: Performance targets from realistic estimates

---

#### US-014: As a developer, I need all bugs fixed
**Priority**: P0 (Critical)
**Story Points**: 13
**Status**: PLANNED
**Sprint**: Sprint 13-14

**Description**:
Address all issues found during testing phases.

**Acceptance Criteria**:
- [ ] All critical bugs fixed
- [ ] All high-severity bugs fixed or mitigated
- [ ] Medium/low bugs fixed or deferred
- [ ] Regression testing passed
- [ ] Code review complete
- [ ] No known release blockers

**Dependencies**: US-010, US-011, US-012 (testing complete)

**Notes**: Bug fixing and stabilization

---

## Epic 6: Documentation and Release (Sprint 15-17)

### User Stories

#### US-015: As an administrator, I need complete documentation
**Priority**: P0 (Critical)
**Story Points**: 13
**Status**: PLANNED
**Sprint**: Sprint 15-16

**Description**:
Create comprehensive documentation for installation, configuration, and migration.

**Acceptance Criteria**:
- [ ] Installation guide (all platforms)
- [ ] Migration guide (v1.x → v2.0)
- [ ] Configuration reference
- [ ] Troubleshooting guide
- [ ] Security best practices
- [ ] Developer documentation
- [ ] All documentation reviewed

**Dependencies**: US-014 (stable code)

**Notes**: User-facing and developer documentation

---

#### US-016: As a user, I want to test beta/RC releases
**Priority**: P1 (High)
**Story Points**: 8
**Status**: PLANNED
**Sprint**: Sprint 17

**Description**:
Release beta and RC versions for public testing.

**Acceptance Criteria**:
- [ ] v2.0.0-beta.1 released
- [ ] Public testing feedback collected
- [ ] Issues addressed
- [ ] v2.0.0-rc.1 released
- [ ] 1-week soak period with no critical issues
- [ ] Ready for stable release

**Dependencies**: US-015

**Notes**: Public testing and validation

---

#### US-017: As a user, I want a stable v2.0.0 release
**Priority**: P0 (Critical)
**Story Points**: 5
**Status**: PLANNED
**Sprint**: Sprint 17

**Description**:
Release ocserv-modern v2.0.0 stable with all packages and announcements.

**Acceptance Criteria**:
- [ ] v2.0.0 tag created
- [ ] All packages built (RPM, DEB, source, Docker)
- [ ] Release notes published
- [ ] Website updated
- [ ] Announcements sent (mailing list, social media)
- [ ] Distribution maintainers notified

**Dependencies**: US-016 (RC approved)

**Notes**: MAJOR MILESTONE

---

## Epic 7: Additional Library Migrations (Parallel Work)

### User Stories

#### US-018: As a developer, I want modern event loop (libuv)
**Priority**: P1 (High)
**Story Points**: 13
**Status**: PLANNED
**Sprint**: TBD (can be parallel)

**Description**:
Migrate from libev to libuv for better cross-platform support.

**Acceptance Criteria**:
- [ ] Event loop abstraction created
- [ ] main.c migrated to libuv
- [ ] worker.c migrated to libuv
- [ ] All timers and I/O watchers ported
- [ ] Integration tests pass
- [ ] Performance validated

**Dependencies**: US-006 (abstraction pattern)

**Notes**: Can be done in parallel with TLS migration

---

#### US-019: As a developer, I want modern HTTP parser (llhttp)
**Priority**: P1 (High)
**Story Points**: 5
**Status**: PLANNED
**Sprint**: TBD

**Description**:
Replace http-parser with llhttp for better performance and security.

**Acceptance Criteria**:
- [ ] llhttp integrated in worker-http.c
- [ ] All HTTP parsing functional
- [ ] HTTP protocol compliance tested
- [ ] Security tested (fuzzing)
- [ ] Performance improvement verified

**Dependencies**: None (independent)

**Notes**: Low risk, high value

---

#### US-020: As a developer, I want lightweight JSON library (cJSON)
**Priority**: P2 (Medium)
**Story Points**: 3
**Status**: PLANNED
**Sprint**: TBD

**Description**:
Replace jansson with cJSON for simpler, lighter JSON handling.

**Acceptance Criteria**:
- [ ] cJSON integrated
- [ ] Configuration parsing works
- [ ] OIDC JSON handling works
- [ ] All tests pass

**Dependencies**: None (independent)

**Notes**: Nice to have, not critical

---

#### US-021: As a developer, I want high-performance allocator (mimalloc)
**Priority**: P2 (Medium)
**Story Points**: 5
**Status**: PLANNED
**Sprint**: TBD

**Description**:
Replace libtalloc with mimalloc for better performance and security.

**Acceptance Criteria**:
- [ ] Memory abstraction layer created
- [ ] All allocations go through abstraction
- [ ] mimalloc integrated
- [ ] Memory leak testing clean
- [ ] Performance improvement measured

**Dependencies**: None (independent)

**Notes**: Performance optimization

---

#### US-022: As a developer, I want simple CLI library (linenoise)
**Priority**: P3 (Low)
**Story Points**: 2
**Status**: PLANNED
**Sprint**: TBD

**Description**:
Replace libreadline with linenoise in occtl for simpler dependency.

**Acceptance Criteria**:
- [ ] linenoise integrated in occtl
- [ ] Interactive CLI works
- [ ] History functional
- [ ] Tab completion works (if needed)

**Dependencies**: None (independent)

**Notes**: Minor improvement, low priority

---

## Deferred / Not Planned

### ❌ DO NOT IMPLEMENT

#### ~~US-023~~: ~~As a developer, I want Cap'n Proto for IPC~~
**Priority**: ❌ REJECTED
**Story Points**: N/A
**Status**: REJECTED

**Rationale**: IPC is not a performance bottleneck. Migration adds risk without proven benefit. Keep protobuf-c.

**Decision**: FINAL - Not open for reconsideration in v2.0

---

## Backlog Grooming

### Grooming Schedule

- **Weekly Backlog Review**: Every Monday, 1 hour
- **Major Grooming Session**: Every 4 weeks (between epics)

### Grooming Activities

- **Refinement**: Break down large stories (>13 SP)
- **Estimation**: Re-estimate stories as understanding improves
- **Prioritization**: Adjust priorities based on learnings
- **Dependencies**: Update dependency graph
- **Acceptance Criteria**: Clarify and expand as needed

---

## Backlog Health Metrics

### Current State

- **Total Stories**: 23 (22 approved, 1 rejected)
- **Total Estimated SP**: ~250 SP
- **P0 (Critical)**: 13 stories
- **P1 (High)**: 6 stories
- **P2 (Medium)**: 3 stories
- **P3 (Low)**: 1 story

### Velocity Projection

Assuming team velocity of ~15 SP/sprint (2 developers, 2 weeks):
- **Estimated Sprints**: ~17 sprints
- **Estimated Duration**: ~34 calendar weeks

With contingency (30%): **~45-50 weeks** (aligns with realistic estimate)

---

## Definition of Ready (DoR)

Before a story enters a sprint, it must meet:

- [ ] Story is clearly defined and understood
- [ ] Acceptance criteria are specific and testable
- [ ] Story is estimated (story points assigned)
- [ ] Dependencies are identified and documented
- [ ] Priority is assigned
- [ ] Technical approach is understood
- [ ] No blocking dependencies

---

## Definition of Done (DoD)

For a story to be marked complete, it must meet:

- [ ] All acceptance criteria met
- [ ] Code written and reviewed
- [ ] Unit tests written and passing (≥80% coverage)
- [ ] Integration tests passing (if applicable)
- [ ] Documentation updated
- [ ] No critical bugs introduced
- [ ] Peer reviewed and approved
- [ ] Merged to main branch
- [ ] Deployed to test environment and validated

---

**Document Version**: 1.0
**Last Updated**: 2025-10-29
**Next Review**: 2025-11-05 (weekly grooming)
