# TODO Tracking - ocserv-modern

**Last Updated**: 2025-10-29
**Current Sprint**: Sprint 0 (Project Setup)
**Active Development Version**: 2.0.0-alpha.1

---

## Released Versions

### v1.x.x Series (Legacy - Not Part of This Project)
- Original ocserv with GnuTLS
- No changes tracked in this project

---

## Active Development

### v2.0.0 - Major Refactoring with wolfSSL (Target: Q3 2026)

This is the first major release of ocserv-modern, representing a complete migration from GnuTLS to wolfSSL native API with modern C library stack.

**Status**: Planning and Setup Phase
**Expected Timeline**: 50-70 weeks (realistic estimate per critical analysis)
**Risk Level**: HIGH

#### Phase 0: Preparation and Critical Analysis (Weeks 1-3) - IN PROGRESS

**Status**: 🔄 In Progress
**Sprint**: Sprint 0
**Assignee**: Team
**Priority**: CRITICAL

##### Completed
- [x] Repository initialization
- [x] GitHub repository created
- [x] Project structure defined
- [x] Podman container environments configured
- [x] Release policy documented
- [x] Documentation templates created

##### In Progress
- [ ] Upstream ocserv code analysis
- [ ] GnuTLS API usage audit (identify all touchpoints)
- [ ] Performance baseline establishment
  - [ ] Set up benchmarking infrastructure
  - [ ] Profile current ocserv with GnuTLS
  - [ ] Identify actual bottlenecks (is TLS really the problem?)
  - [ ] Document baseline metrics

##### Pending
- [ ] Proof of Concept development
  - [ ] Minimal TLS migration PoC
  - [ ] Performance comparison (GnuTLS vs wolfSSL)
  - [ ] Validate claimed 2x performance improvement
  - [ ] Decision gate: GO/NO-GO based on PoC results
- [ ] Security audit planning
  - [ ] Budget allocation ($50k-100k for external consultant)
  - [ ] Identify security audit vendor
  - [ ] Schedule audit phases
- [ ] Risk assessment and mitigation planning
  - [ ] Document all identified risks from critical analysis
  - [ ] Create risk register
  - [ ] Define mitigation strategies
  - [ ] Establish rollback procedures
- [ ] Development environment setup
  - [ ] All team members have working dev containers
  - [ ] CI/CD pipeline basic structure
  - [ ] Testing framework selection

**Exit Criteria**:
- PoC demonstrates measurable performance improvement (minimum 10%)
- Security audit plan approved and funded
- All risks documented with mitigation plans
- Development environment operational
- GO/NO-GO decision made by project stakeholders

**Risks**:
- ⚠️ CRITICAL: PoC may show <10% improvement, questioning entire migration
- ⚠️ HIGH: Security audit may reveal fundamental flaws in approach
- ⚠️ MEDIUM: Development environment issues may delay team onboarding

---

#### Phase 1: Infrastructure and Abstraction Layer (Weeks 4-6) - PENDING

**Status**: ⏸️ Pending (blocked by Phase 0 completion)
**Priority**: HIGH

##### TODO
- [ ] Design TLS library abstraction layer
  - [ ] Define abstract interface for TLS operations
  - [ ] Support both GnuTLS and wolfSSL backends
  - [ ] Enable runtime switching via feature flags
- [ ] Implement dual-build capability
  - [ ] Build system modifications (Meson)
  - [ ] Conditional compilation support
  - [ ] Testing with both backends
- [ ] Set up comprehensive testing framework
  - [ ] Unit test infrastructure (Check framework)
  - [ ] Integration test framework
  - [ ] Performance benchmarking automation
  - [ ] Client compatibility test suite
- [ ] Establish performance baselines
  - [ ] Document current GnuTLS metrics
  - [ ] Define success criteria for migration
  - [ ] Set up continuous performance monitoring

**Exit Criteria**:
- Abstraction layer compiles and links
- Dual-build system functional
- Tests run against both GnuTLS and wolfSSL
- Performance monitoring operational

**Risks**:
- ⚠️ MEDIUM: Abstraction layer overhead may negate performance gains
- ⚠️ MEDIUM: Complexity of dual-build may introduce maintenance burden

---

#### Phase 2: Core TLS Migration (Weeks 7-18) - PENDING

**Status**: ⏸️ Pending
**Priority**: HIGH

##### TODO

**2.1 wolfSSL Wrapper Layer (Weeks 7-9)**
- [ ] Implement core TLS functions
  - [ ] Context initialization and configuration
  - [ ] Session management
  - [ ] Certificate handling
  - [ ] Error handling and logging
- [ ] I/O callback implementation
  - [ ] Non-blocking I/O support
  - [ ] Buffer management
  - [ ] Read/write operations
- [ ] Unit tests for wrapper layer

**2.2 TLS Connection Handling (Weeks 10-13)**
- [ ] Migrate worker-vpn.c TLS code
  - [ ] Handshake implementation
  - [ ] Data read/write paths
  - [ ] Session resumption
  - [ ] Cipher suite configuration
- [ ] Migrate tls.c/tls.h
  - [ ] Replace GnuTLS priority strings
  - [ ] wolfSSL cipher configuration
  - [ ] TLS 1.3 support
- [ ] Integration testing with OpenConnect client

**2.3 Certificate Authentication (Weeks 14-16)**
- [ ] Certificate loading and validation
  - [ ] PEM/DER format support
  - [ ] Certificate chain building
  - [ ] Trust anchor configuration
- [ ] PKCS#11 integration
  - [ ] Smart card support
  - [ ] Hardware token support
- [ ] TPM integration (if needed)

**2.4 DTLS Support (Weeks 17-18)**
- [ ] DTLS 1.2 migration
  - [ ] Cookie handling for DoS protection
  - [ ] Fragmentation and reassembly
  - [ ] UDP-specific handling
- [ ] DTLS 1.3 implementation
  - [ ] Connection ID (CID) support
  - [ ] Performance optimization
  - [ ] Testing under packet loss

**Exit Criteria**:
- All TLS/DTLS code migrated from GnuTLS to wolfSSL
- OpenConnect clients can connect successfully
- All existing authentication methods work
- Performance targets met (defined in Phase 0)
- No regressions in functionality

**Risks**:
- 🔴 CRITICAL: Certificate validation differences may break existing deployments
- 🔴 CRITICAL: DTLS behavior changes may cause client disconnections
- ⚠️ HIGH: Thread safety issues in wolfSSL usage
- ⚠️ HIGH: Session resumption incompatibilities

---

#### Phase 3: Testing and Validation (Weeks 19-26) - PENDING

**Status**: ⏸️ Pending
**Priority**: CRITICAL

##### TODO

**3.1 Unit Testing (Weeks 19-20)**
- [ ] Comprehensive unit test coverage
  - [ ] Target: ≥80% code coverage
  - [ ] All crypto operations tested
  - [ ] Error paths validated
- [ ] Memory leak testing
  - [ ] Valgrind clean runs
  - [ ] AddressSanitizer testing
  - [ ] Memory usage profiling

**3.2 Integration Testing (Weeks 21-22)**
- [ ] Full connection lifecycle testing
  - [ ] Initial connection
  - [ ] Session resumption
  - [ ] Reconnection handling
  - [ ] Graceful disconnect
- [ ] Multi-user scenarios
  - [ ] Concurrent connections
  - [ ] Load testing
  - [ ] Stress testing
- [ ] Authentication method testing
  - [ ] Password authentication
  - [ ] Certificate authentication
  - [ ] RADIUS integration
  - [ ] OIDC/JWT (with limitations noted)

**3.3 Security Testing (Weeks 23-24)**
- [ ] Security audit (external consultant)
  - [ ] TLS implementation review
  - [ ] Cryptographic operations audit
  - [ ] Attack surface analysis
  - [ ] Vulnerability assessment
- [ ] Penetration testing
  - [ ] TLS downgrade attacks
  - [ ] Certificate validation bypass attempts
  - [ ] DoS resilience testing
- [ ] Fuzzing campaign
  - [ ] TLS handshake fuzzing
  - [ ] HTTP request fuzzing
  - [ ] Configuration file fuzzing

**3.4 Client Compatibility Testing (Weeks 25-26)**
- [ ] Cisco Secure Client 5.x
  - [ ] All versions from 5.0 to latest
  - [ ] All authentication methods
  - [ ] DTLS and TLS modes
- [ ] OpenConnect CLI
  - [ ] Versions 8.x and 9.x
  - [ ] All platforms (Linux, macOS, Windows)
- [ ] OpenConnect GUI
  - [ ] Latest stable version
  - [ ] Configuration migration
- [ ] NetworkManager plugin
  - [ ] Various Linux distributions
- [ ] Platform testing
  - [ ] Linux (Ubuntu, Fedora, RHEL, Debian)
  - [ ] FreeBSD
  - [ ] OpenBSD

**Exit Criteria**:
- Zero critical security findings
- <3 high-severity security findings (all mitigated)
- 100% client compatibility with Cisco Secure Client 5.x+
- All tests passing
- Security audit report approved

**Risks**:
- 🔴 CRITICAL: Security audit may find fundamental vulnerabilities
- 🔴 CRITICAL: Client compatibility issues may block release
- ⚠️ HIGH: Fuzzing may reveal crash bugs

---

#### Phase 4: Optimization and Bug Fixing (Weeks 27-32) - PENDING

**Status**: ⏸️ Pending
**Priority**: HIGH

##### TODO

**4.1 Performance Optimization (Weeks 27-28)**
- [ ] Profile and optimize hot paths
  - [ ] CPU profiling (perf, flamegraphs)
  - [ ] Memory profiling
  - [ ] Lock contention analysis
- [ ] Benchmark verification
  - [ ] TLS handshakes/sec vs baseline
  - [ ] Throughput (Gbps) vs baseline
  - [ ] CPU usage vs baseline
  - [ ] Memory usage vs baseline
- [ ] Algorithm selection tuning
  - [ ] Cipher suite optimization
  - [ ] Buffer size tuning
  - [ ] Session cache tuning

**4.2 Bug Fixing (Weeks 29-31)**
- [ ] Address all issues from testing phases
  - [ ] Prioritize by severity
  - [ ] Fix and verify
  - [ ] Regression testing
- [ ] Code review and refactoring
  - [ ] Peer review all migration code
  - [ ] Refactor complex sections
  - [ ] Improve code clarity

**4.3 Final Security Review (Week 32)**
- [ ] Re-audit after bug fixes
- [ ] Verify all security findings addressed
- [ ] Final penetration test
- [ ] Security documentation review

**Exit Criteria**:
- Performance targets achieved (5-15% minimum improvement)
- All critical and high-severity bugs fixed
- Code review complete
- Final security review passed

**Risks**:
- ⚠️ HIGH: Performance targets not met may require rearchitecture
- ⚠️ MEDIUM: Bug fixes may introduce new issues

---

#### Phase 5: Documentation and Release Preparation (Weeks 33-37) - PENDING

**Status**: ⏸️ Pending
**Priority**: MEDIUM

##### TODO

**5.1 Documentation (Weeks 33-34)**
- [ ] Developer documentation
  - [ ] wolfSSL integration guide
  - [ ] API migration guide (GnuTLS → wolfSSL)
  - [ ] Architecture documentation
  - [ ] Code contribution guide
- [ ] Administrator documentation
  - [ ] Installation guide
  - [ ] Migration guide (v1.x → v2.0)
  - [ ] Configuration reference
  - [ ] Troubleshooting guide
  - [ ] Security best practices
- [ ] User documentation
  - [ ] Quick start guide
  - [ ] FAQ
  - [ ] Client setup guides

**5.2 Release Preparation (Week 35)**
- [ ] Package preparation
  - [ ] Source tarball
  - [ ] RPM packages (Fedora, RHEL, Oracle Linux)
  - [ ] DEB packages (Debian, Ubuntu)
  - [ ] FreeBSD port
  - [ ] Docker/Podman images
- [ ] Release notes
  - [ ] Complete release notes using template
  - [ ] Migration guide
  - [ ] Known issues documentation
- [ ] Announcement preparation
  - [ ] Blog post draft
  - [ ] Social media posts
  - [ ] Mailing list announcement

**5.3 Beta/RC Releases (Weeks 36-37)**
- [ ] v2.0.0-beta.1 release
  - [ ] Public testing announcement
  - [ ] Feedback collection
  - [ ] Issue tracking
- [ ] v2.0.0-rc.1 release
  - [ ] Final testing
  - [ ] No new features
  - [ ] Bug fixes only

**Exit Criteria**:
- All documentation complete and reviewed
- Packages built and tested
- Release notes finalized
- RC passes 1 week without critical issues

**Risks**:
- ⚠️ MEDIUM: Documentation gaps may confuse users
- ⚠️ LOW: Package build issues on some platforms

---

#### Phase 6: Stable Release and Post-Release (Weeks 38-40+) - PENDING

**Status**: ⏸️ Pending
**Priority**: HIGH

##### TODO

**6.1 v2.0.0 Stable Release (Week 38)**
- [ ] Final release
  - [ ] Tag v2.0.0
  - [ ] Upload all packages
  - [ ] Publish release notes
  - [ ] Update website
- [ ] Announcements
  - [ ] Mailing list
  - [ ] Social media
  - [ ] Blog post
  - [ ] Distribution maintainers notification

**6.2 Post-Release Monitoring (Weeks 39-40+)**
- [ ] Issue tracking and triage
  - [ ] Monitor GitHub issues
  - [ ] Prioritize regression reports
  - [ ] Rapid response to critical issues
- [ ] Feedback collection
  - [ ] User surveys
  - [ ] Performance reports
  - [ ] Migration experiences
- [ ] Hotfix planning
  - [ ] Prepare v2.0.1 if needed
  - [ ] Fast-track critical fixes

**Exit Criteria**:
- Release published and announced
- No critical issues reported in first 2 weeks
- User feedback generally positive

**Risks**:
- ⚠️ HIGH: Critical issues post-release may require hotfix
- ⚠️ MEDIUM: Negative user feedback may require rapid iteration

---

### v2.1.0 - Feature Enhancements (Target: Q1 2027) - PLANNING

**Status**: 📋 Planning
**Priority**: LOW (future)

##### TODO (Tentative)
- [ ] QUIC protocol support
  - [ ] Integration with ngtcp2
  - [ ] Performance testing
  - [ ] Client compatibility
- [ ] HTTP/3 support
- [ ] Performance optimizations based on v2.0 feedback
- [ ] Additional authentication methods
- [ ] Enhanced monitoring and metrics

---

## Deferred / Not Planned for v2.0

### IPC Layer Changes - DO NOT IMPLEMENT
- [❌] Cap'n Proto migration - **Rejected per critical analysis**
- [❌] nanopb migration - **Not needed, keep protobuf-c**
- **Rationale**: IPC is not a performance bottleneck, migration adds risk without benefit

### JWT/JOSE Enhancements - LIMITED
- [⚠️] Full OIDC support - **Deferred to v2.1+ due to wolfSSL limitations**
- **Rationale**: wolfSSL JWT support is immature, requires additional development

### Build System Complete Rewrite
- [⚠️] Full Meson migration - **Incremental approach preferred**
- **Rationale**: Can be done gradually, no need to block v2.0

---

## Risk Register

### Top 10 Risks for v2.0.0

1. **[CRITICAL]** Performance PoC fails to show ≥10% improvement
   - Mitigation: GO/NO-GO gate at Phase 0
   - Impact: Entire project viability

2. **[CRITICAL]** Security audit reveals fundamental vulnerabilities
   - Mitigation: External audit, early security testing
   - Impact: Major rework or project cancellation

3. **[CRITICAL]** Client compatibility breaks with Cisco Secure Client
   - Mitigation: Extensive compatibility testing in Phase 3
   - Impact: Release blocker

4. **[HIGH]** Timeline underestimated (34 weeks vs 50-70 actual)
   - Mitigation: Realistic planning, 30% contingency buffer
   - Impact: Delayed release, resource allocation issues

5. **[HIGH]** wolfSSL API changes introduce subtle bugs
   - Mitigation: Comprehensive testing, dual-build approach
   - Impact: Production issues, emergency patches

6. **[HIGH]** Certificate validation differences break deployments
   - Mitigation: Extensive certificate testing, migration guide
   - Impact: User complaints, support burden

7. **[MEDIUM]** DTLS 1.3 implementation issues
   - Mitigation: Thorough testing, packet loss simulation
   - Impact: Degraded UDP performance

8. **[MEDIUM]** Memory leaks or race conditions in wolfSSL usage
   - Mitigation: Valgrind, sanitizers, stress testing
   - Impact: Server crashes, instability

9. **[MEDIUM]** JWT/JOSE regression for OIDC users
   - Mitigation: Document limitations, provide workarounds
   - Impact: User dissatisfaction

10. **[LOW]** Package build failures on some platforms
    - Mitigation: Multi-platform CI/CD testing
    - Impact: Limited platform support

---

## Sprint Planning

### Sprint 0: Project Setup (Current Sprint)

**Duration**: 2025-10-29 to 2025-11-12 (2 weeks)
**Goals**:
- Complete project initialization
- Set up development infrastructure
- Begin upstream code analysis

**Sprint Backlog**:
- [x] Create GitHub repository
- [x] Set up project structure
- [x] Create Podman environments
- [x] Document release policy
- [ ] Analyze upstream ocserv code
- [ ] Set up CI/CD pipeline basics
- [ ] Begin performance baseline establishment

**Sprint Review**: 2025-11-12
**Sprint Retrospective**: 2025-11-12

### Sprint 1: Critical Analysis and PoC (Planned)

**Duration**: 2025-11-13 to 2025-11-27 (2 weeks)
**Goals**:
- Complete Phase 0 critical analysis
- Develop and test Proof of Concept
- Make GO/NO-GO decision

---

## Velocity Tracking

**Estimated Total Effort**: 50-70 person-weeks
**Team Size**: 2 developers (assumed)
**Calendar Duration**: 25-35 weeks (6-8 months)

**Current Progress**: <5% (setup phase only)

---

## Decision Log

Key decisions will be tracked here as the project progresses.

### Decision 001: wolfSSL Native API vs Compatibility Layer
- **Date**: 2025-10-29
- **Decision**: Use wolfSSL native API, not OpenSSL compatibility layer
- **Rationale**: Better performance, smaller footprint, cleaner integration
- **Impact**: More migration work, but better long-term results

### Decision 002: Do Not Migrate IPC Layer
- **Date**: 2025-10-29
- **Decision**: Keep protobuf-c for IPC, do not migrate to Cap'n Proto or nanopb
- **Rationale**: Not a bottleneck, adds risk without proven benefit
- **Impact**: Reduced scope, faster delivery

### Decision 003: External Security Audit Required
- **Date**: 2025-10-29
- **Decision**: Budget for external security audit ($50k-100k)
- **Rationale**: VPN server is security-critical, need independent validation
- **Impact**: Increased cost but better security assurance

---

## Research & Future Exploration

### ExpressVPN Lightway Protocol Investigation

**Priority**: MEDIUM (Research)
**Timeline**: Post v2.0.0 release
**Status**: ⏸️ Deferred until after core wolfSSL migration

#### Background

ExpressVPN's **Lightway** is a modern VPN protocol built on wolfSSL, designed as a lightweight, fast, and secure alternative to legacy VPN protocols. It uses:
- **wolfSSL** for cryptographic operations (same as our project!)
- Modern cryptographic primitives
- Optimized for mobile and unreliable networks
- Open source implementation available

#### Resources

- **Core Library**: https://github.com/expressvpn/lightway-core
- **Rust Implementation**: https://github.com/expressvpn/lightway (reference with docs)
- **Documentation**: https://github.com/expressvpn/lightway/tree/main/docs

#### Investigation Tasks

- [ ] Review Lightway protocol specification
  - [ ] Understand protocol design and message flow
  - [ ] Analyze cryptographic choices and justification
  - [ ] Compare with OpenConnect/AnyConnect protocol

- [ ] Study lightway-core C implementation
  - [ ] Review wolfSSL integration patterns
  - [ ] Analyze error handling approaches
  - [ ] Study session management and resumption
  - [ ] Evaluate thread safety model

- [ ] Study Rust implementation and documentation
  - [ ] Read design documentation
  - [ ] Understand architectural decisions
  - [ ] Extract best practices applicable to ocserv-modern

- [ ] Evaluate integration opportunities
  - [ ] Could ocserv-modern support Lightway as alternative protocol?
  - [ ] Are there wolfSSL usage patterns we should adopt?
  - [ ] Can we reuse any abstraction layers?
  - [ ] Compatibility considerations with Cisco ecosystem

#### Key Questions to Answer

1. **Performance**: How does Lightway compare to OpenConnect protocol?
2. **Security**: What security advantages does Lightway offer?
3. **Compatibility**: Could Lightway coexist with AnyConnect protocol support?
4. **Code Reuse**: Can we reuse any Lightway components or patterns?
5. **wolfSSL Usage**: What can we learn from their wolfSSL integration?
6. **Mobile Optimization**: What mobile-specific optimizations does Lightway use?

#### Potential Outcomes

**Option A: Learn Best Practices**
- Adopt wolfSSL integration patterns
- Use similar error handling approaches
- Apply mobile optimization techniques
- Maintain AnyConnect protocol compatibility

**Option B: Dual Protocol Support**
- Add Lightway as optional alternative protocol
- Clients can choose OpenConnect or Lightway
- Requires significant additional development
- Market positioning: "Best of both worlds"

**Option C: Lightway-Inspired Enhancements**
- Enhance OpenConnect protocol with Lightway ideas
- Maintain backward compatibility
- Selective feature adoption

#### Dependencies

**Blocked By**:
- v2.0.0 release (wolfSSL migration must be complete and stable)
- Performance validation of wolfSSL migration
- Team capacity post-launch

**Prerequisites**:
- Completed wolfSSL integration experience
- Stable production deployment
- Community feedback on v2.0.0

#### Estimated Effort

**Research Phase**: 2-3 weeks
- Protocol analysis: 1 week
- Code review: 1 week
- Feasibility assessment: 1 week

**If Pursuing Integration**:
- Lightweight (pattern adoption): 2-4 weeks
- Medium (inspired enhancements): 8-12 weeks
- Full (dual protocol): 16-24 weeks

#### Decision Date

**No Earlier Than**: Q4 2026 (post v2.0.0 release)
**Review Trigger**: After v2.0.0 has been in production for 3+ months

#### Notes

- Lightway is GPLv2 licensed (same as wolfSSL 5.7.x and earlier)
  - **IMPORTANT**: Check Lightway's wolfSSL version requirements
  - May use older wolfSSL version with GPLv2 (not v5.8.2 with GPLv3)
- ExpressVPN actively maintains the project
- Growing community adoption
- Designed by security experts with VPN domain expertise
- Could provide valuable insights even if we don't adopt the protocol

---

## Contact

**Project Lead**: TBD
**Security Lead**: TBD
**DevOps Lead**: TBD

**Mailing List**: ocserv-dev@lists.infradead.org
**GitHub**: https://github.com/dantte-lp/ocserv-modern
**Discord**: TBD

---

**Document Version**: 1.0
**Last Updated**: 2025-10-29
**Next Review**: 2025-11-12 (Sprint 0 retrospective)
