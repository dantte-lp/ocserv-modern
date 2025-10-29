#!/bin/bash
set -euo pipefail

# verify-rootless.sh - Verify rootless container support
# Checks system configuration for rootless Podman

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_pass() {
    echo -e "${GREEN}[PASS]${NC} $*"
}

log_fail() {
    echo -e "${RED}[FAIL]${NC} $*"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

log_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

CHECKS_PASSED=0
CHECKS_FAILED=0
CHECKS_WARNED=0

# Check 1: Rootless Podman is running as non-root
echo ""
log_info "Checking rootless mode..."
if [ "$EUID" -eq 0 ]; then
    log_fail "Running as root - rootless mode requires non-root user"
    ((CHECKS_FAILED++))
else
    log_pass "Running as non-root user (UID: $EUID)"
    ((CHECKS_PASSED++))
fi

# Check 2: User namespaces enabled
echo ""
log_info "Checking user namespace support..."
if [ -f /proc/sys/user/max_user_namespaces ]; then
    MAX_NAMESPACES=$(cat /proc/sys/user/max_user_namespaces)
    if [ "$MAX_NAMESPACES" -gt 0 ]; then
        log_pass "User namespaces enabled (max: $MAX_NAMESPACES)"
        ((CHECKS_PASSED++))
    else
        log_fail "User namespaces disabled (max_user_namespaces = 0)"
        log_info "Enable with: sudo sysctl -w user.max_user_namespaces=28633"
        ((CHECKS_FAILED++))
    fi
else
    log_warn "Cannot determine user namespace support"
    ((CHECKS_WARNED++))
fi

# Check 3: subuid and subgid configured
echo ""
log_info "Checking subuid/subgid configuration..."
if [ -f /etc/subuid ] && [ -f /etc/subgid ]; then
    if grep -q "^$(whoami):" /etc/subuid && grep -q "^$(whoami):" /etc/subgid; then
        SUBUID_RANGE=$(grep "^$(whoami):" /etc/subuid | cut -d: -f2-3)
        SUBGID_RANGE=$(grep "^$(whoami):" /etc/subgid | cut -d: -f2-3)
        log_pass "subuid configured: $SUBUID_RANGE"
        log_pass "subgid configured: $SUBGID_RANGE"
        ((CHECKS_PASSED+=2))
    else
        log_fail "No subuid/subgid entries for user: $(whoami)"
        log_info "Configure with: sudo usermod --add-subuids 100000-165535 --add-subgids 100000-165535 $(whoami)"
        ((CHECKS_FAILED+=2))
    fi
else
    log_fail "subuid/subgid files not found"
    ((CHECKS_FAILED+=2))
fi

# Check 4: crun runtime available
echo ""
log_info "Checking crun runtime..."
if command -v crun >/dev/null 2>&1; then
    CRUN_VERSION=$(crun --version | head -1)
    log_pass "crun installed: $CRUN_VERSION"
    ((CHECKS_PASSED++))
else
    log_warn "crun not found (will fall back to runc)"
    log_info "Install with: sudo dnf install crun"
    ((CHECKS_WARNED++))
fi

# Check 5: fuse-overlayfs available
echo ""
log_info "Checking fuse-overlayfs..."
if command -v fuse-overlayfs >/dev/null 2>&1; then
    log_pass "fuse-overlayfs installed"
    ((CHECKS_PASSED++))
else
    log_fail "fuse-overlayfs not found (required for rootless overlay storage)"
    log_info "Install with: sudo dnf install fuse-overlayfs"
    ((CHECKS_FAILED++))
fi

# Check 6: slirp4netns for networking
echo ""
log_info "Checking slirp4netns..."
if command -v slirp4netns >/dev/null 2>&1; then
    log_pass "slirp4netns installed"
    ((CHECKS_PASSED++))
else
    log_warn "slirp4netns not found (needed for rootless networking)"
    log_info "Install with: sudo dnf install slirp4netns"
    ((CHECKS_WARNED++))
fi

# Check 7: XDG_RUNTIME_DIR set
echo ""
log_info "Checking XDG_RUNTIME_DIR..."
if [ -n "${XDG_RUNTIME_DIR:-}" ] && [ -d "$XDG_RUNTIME_DIR" ]; then
    log_pass "XDG_RUNTIME_DIR set: $XDG_RUNTIME_DIR"
    ((CHECKS_PASSED++))
else
    log_fail "XDG_RUNTIME_DIR not set or invalid"
    log_info "Usually set automatically by systemd --user"
    ((CHECKS_FAILED++))
fi

# Check 8: Podman storage configuration
echo ""
log_info "Checking Podman storage..."
if [ -f "$HOME/.config/containers/storage.conf" ] || [ -f "/etc/containers/storage.conf" ]; then
    log_pass "Podman storage configured"
    ((CHECKS_PASSED++))
else
    log_warn "No storage.conf found (will use defaults)"
    ((CHECKS_WARNED++))
fi

# Check 9: Test rootless container
echo ""
log_info "Testing rootless container execution..."
if podman run --rm registry.access.redhat.com/ubi9/ubi-micro:latest echo "Rootless test successful" >/dev/null 2>&1; then
    log_pass "Successfully ran rootless container"
    ((CHECKS_PASSED++))
else
    log_fail "Failed to run rootless container"
    log_info "Check: podman info for diagnostics"
    ((CHECKS_FAILED++))
fi

# Check 10: cgroups v2
echo ""
log_info "Checking cgroups version..."
if [ -f /sys/fs/cgroup/cgroup.controllers ]; then
    log_pass "Using cgroups v2 (recommended)"
    ((CHECKS_PASSED++))
else
    log_warn "Using cgroups v1 (v2 recommended for better rootless support)"
    ((CHECKS_WARNED++))
fi

# Summary
echo ""
echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}Rootless Verification Summary${NC}"
echo -e "${BLUE}======================================${NC}"
echo -e "${GREEN}Passed:${NC}  $CHECKS_PASSED"
echo -e "${YELLOW}Warnings:${NC} $CHECKS_WARNED"
echo -e "${RED}Failed:${NC}  $CHECKS_FAILED"
echo ""

if [ $CHECKS_FAILED -eq 0 ]; then
    log_pass "System is properly configured for rootless Podman!"
    exit 0
else
    log_fail "Some checks failed - rootless mode may not work correctly"
    log_info "Review the failed checks above and follow the suggested fixes"
    exit 1
fi
