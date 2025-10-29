# Podman Container Environments for ocserv-modern

This directory contains container configurations for development, testing, and building ocserv-modern.

## Prerequisites

- Podman or Docker with docker-compose support
- At least 4GB of free disk space
- Root or sudo access (for TUN device access in development)

## Environments

### Development Environment

The development environment includes all build dependencies and development tools:

- wolfSSL 5.7.4 (native API)
- libuv 1.48.0
- llhttp 9.2.1
- cJSON 1.7.18
- mimalloc 2.1.7
- All standard development tools (gcc, gdb, valgrind, etc.)

**Start the development environment:**

```bash
cd deploy/podman
docker-compose up -d dev
docker-compose exec dev /bin/bash
```

**Build the project:**

```bash
# Inside the container
cd /workspace
meson setup build
meson compile -C build
```

### Testing Environment

The testing environment is optimized for running tests:

```bash
docker-compose up test
```

### Build Environment

The build environment is for creating release builds:

```bash
docker-compose up -d build
docker-compose exec build /bin/bash
```

## Volume Mounts

- Project root is mounted at `/workspace` in all containers
- Development cache is persisted in `dev-cache` volume
- Build artifacts are stored in `build-artifacts` volume

## Network Configuration

All containers share the `ocserv-net` network bridge for inter-container communication.

## Security Considerations

- Development container has `NET_ADMIN` and `SYS_ADMIN` capabilities for TUN device access
- Test and build containers have minimal privileges
- All containers run as root by default (change as needed for production)

## Maintenance

**Rebuild containers after dependency changes:**

```bash
docker-compose build --no-cache
```

**Clean up all containers and volumes:**

```bash
docker-compose down -v
```

## Troubleshooting

### TUN device not available

If you see "Cannot open TUN/TAP device" errors:

```bash
# On host
sudo modprobe tun
sudo chmod 666 /dev/net/tun
```

### Library not found errors

Rebuild the development container:

```bash
docker-compose build --no-cache dev
```
