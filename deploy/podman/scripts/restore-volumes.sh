#!/bin/bash
set -euo pipefail

# restore-volumes.sh - Restore volumes from backup
# Restores named volumes from compressed backup archives
#
# Usage:
#   ./restore-volumes.sh [backup-directory]
#   ./restore-volumes.sh latest  # Restore from latest backup
#
# Default: Restore from latest backup

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKUP_BASE="${SCRIPT_DIR}/../volumes/backups"

# Determine backup directory
if [ $# -eq 0 ] || [ "$1" = "latest" ]; then
    BACKUP_DIR="${BACKUP_BASE}/latest"
    log_info "Using latest backup"
else
    BACKUP_DIR="$1"
fi

# Check if backup directory exists
if [ ! -d "$BACKUP_DIR" ]; then
    log_error "Backup directory not found: $BACKUP_DIR"
    log_error "Available backups:"
    ls -1 "$BACKUP_BASE" 2>/dev/null || echo "  No backups found"
    exit 1
fi

log_info "Restoring from backup: $BACKUP_DIR"

# Display backup manifest if available
if [ -f "$BACKUP_DIR/manifest.txt" ]; then
    log_info "Backup manifest:"
    cat "$BACKUP_DIR/manifest.txt" | head -15
    echo ""
fi

# Find all backup archives
BACKUP_FILES=($(find "$BACKUP_DIR" -name "*.tar.gz" -type f))

if [ ${#BACKUP_FILES[@]} -eq 0 ]; then
    log_error "No backup files found in: $BACKUP_DIR"
    exit 1
fi

log_info "Found ${#BACKUP_FILES[@]} backup archive(s)"

# Confirmation prompt
echo ""
log_warn "WARNING: This will overwrite existing volume data!"
read -p "Continue with restore? [y/N] " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    log_error "Restore aborted by user"
    exit 1
fi

# Function to restore a single volume
restore_volume() {
    local backup_file=$1
    local volume_name=$(basename "$backup_file" .tar.gz)

    log_info "Restoring volume: $volume_name"
    log_info "  From: $backup_file"

    # Create volume if it doesn't exist
    if ! podman volume exists "$volume_name" 2>/dev/null; then
        log_info "  Creating volume: $volume_name"
        podman volume create "$volume_name" || {
            log_error "Failed to create volume: $volume_name"
            return 1
        }
    else
        log_warn "  Volume already exists, will overwrite data"

        # Remove existing data
        log_info "  Removing existing data..."
        podman run --rm \
            -v "$volume_name":/volume:Z \
            registry.access.redhat.com/ubi9/ubi-minimal:latest \
            sh -c 'rm -rf /volume/* /volume/.[!.]* /volume/..?*' 2>/dev/null || true
    fi

    # Restore data from backup
    log_info "  Restoring data..."
    podman run --rm \
        -v "$volume_name":/volume:Z \
        -v "$backup_file":/backup.tar.gz:Z,ro \
        registry.access.redhat.com/ubi9/ubi-minimal:latest \
        tar xzf /backup.tar.gz -C /volume || {
        log_error "Failed to restore volume: $volume_name"
        return 1
    }

    log_info "  Volume restored successfully: $volume_name"
    return 0
}

# Restore all volumes
RESTORE_COUNT=0
FAILED_COUNT=0

for backup_file in "${BACKUP_FILES[@]}"; do
    if restore_volume "$backup_file"; then
        ((RESTORE_COUNT++))
    else
        ((FAILED_COUNT++))
    fi
done

# Summary
echo ""
echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}Restore Summary${NC}"
echo -e "${BLUE}======================================${NC}"
echo "Backup source:      $BACKUP_DIR"
echo "Volumes restored:   $RESTORE_COUNT"
echo "Failed restores:    $FAILED_COUNT"
echo ""

if [ $FAILED_COUNT -eq 0 ]; then
    log_info "${GREEN}All volumes restored successfully!${NC}"
    log_info "You can now start containers with the restored data"
    exit 0
else
    log_warn "Some volumes failed to restore"
    exit 1
fi
