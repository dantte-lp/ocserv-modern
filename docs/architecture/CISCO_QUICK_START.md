# Cisco Compatibility Quick Start Guide

**Target**: Development team implementing Cisco Secure Client compatibility
**Time to read**: 10 minutes
**Prerequisites**: Basic understanding of VPN protocols, TLS/DTLS, HTTP

---

## TL;DR - What You Need to Know

Cisco Secure Client uses:
1. **Custom HTTP headers** starting with `X-CSTP-*` and `X-DTLS-*`
2. **XML-based authentication** called "aggregate authentication"
3. **Session cookies** named `webvpn=<token>`
4. **DTLS** shares master secret with TLS tunnel
5. **Always-On VPN** has strict requirements (no proxy, no untrusted certs)

---

## Minimal Working Implementation

### 1. HTTP Server with Custom Headers

```go
// Handler for initial connection
func handleConnect(w http.ResponseWriter, r *http.Request) {
    // Set required Cisco headers
    w.Header().Set("X-CSTP-Version", "1")
    w.Header().Set("X-CSTP-Protocol", "Copyright (c) 2004 Cisco Systems, Inc.")
    w.Header().Set("X-CSTP-Address-Type", "IPv6,IPv4")
    w.Header().Set("X-CSTP-MTU", "1406")
    w.Header().Set("X-CSTP-Base-MTU", "1500")

    // ... handle authentication and tunnel setup
}
```

### 2. Authentication Flow

```go
// Step 1: Client requests authentication
// POST /auth with credentials

// Step 2: Server validates and generates session
session := &Session{
    UserID:    user.ID,
    Username:  user.Username,
    IssuedAt:  time.Now(),
    ExpiresAt: time.Now().Add(8 * time.Hour),
}

// Step 3: Create signed cookie
cookie := CreateSessionCookie(session, secretKey)

// Step 4: Send XML response
xml := fmt.Sprintf(`<?xml version="1.0" encoding="UTF-8"?>
<auth id="COMPLETE">
    <session-token>%s</session-token>
    <config>
        <client-ip-address>192.168.1.10</client-ip-address>
        <dns><server>8.8.8.8</server></dns>
    </config>
</auth>`, session.Token)

w.Header().Set("Content-Type", "text/xml")
w.Header().Set("Set-Cookie", fmt.Sprintf("webvpn=%s; Secure; HttpOnly", cookie))
w.Write([]byte(xml))
```

### 3. Tunnel Establishment

```go
// Client sends: CONNECT /tunnel
// Cookie: webvpn=<session-token>

func handleTunnel(w http.ResponseWriter, r *http.Request) {
    // Validate session cookie
    cookie, err := r.Cookie("webvpn")
    if err != nil {
        http.Error(w, "No session", http.StatusUnauthorized)
        return
    }

    session := ValidateSessionCookie(cookie.Value, secretKey)
    if session == nil {
        http.Error(w, "Invalid session", http.StatusUnauthorized)
        return
    }

    // Upgrade to TLS tunnel
    hijacker, ok := w.(http.Hijacker)
    if !ok {
        http.Error(w, "Cannot hijack", http.StatusInternalServerError)
        return
    }

    conn, _, err := hijacker.Hijack()
    if err != nil {
        return
    }

    // Send success and start tunnel
    conn.Write([]byte("HTTP/1.1 200 OK\r\n\r\n"))

    // Handle tunnel data
    go handleTunnelData(conn, session)
}
```

### 4. DTLS Setup (Simplified)

```go
// After TLS tunnel established, share master secret for DTLS

func setupDTLS(tlsConn *tls.Conn, client *Client) error {
    // Extract TLS master secret
    masterSecret := extractMasterSecret(tlsConn)

    // Send to client via HTTP header
    client.SendHeader("X-DTLS-Master-Secret", hex.EncodeToString(masterSecret))

    // Configure DTLS with same secret
    dtlsConfig := &DTLSConfig{
        MasterSecret: masterSecret,
        CipherSuite:  tlsConn.CipherSuite,
    }

    // Listen for DTLS connection
    dtlsConn, err := listenDTLS(443, dtlsConfig)
    if err != nil {
        return err
    }

    // Handle cookie exchange
    return handleDTLSHandshake(dtlsConn)
}
```

---

## Critical Requirements Checklist

### Must Have (P0)
- [ ] HTTP headers: `X-CSTP-Version`, `X-CSTP-Protocol`, `X-CSTP-Address-Type`
- [ ] Session cookie: `webvpn=<token>`
- [ ] XML authentication responses
- [ ] TLS tunnel (TCP port 443)
- [ ] Client IP assignment

### Should Have (P1)
- [ ] DTLS tunnel (UDP port 443)
- [ ] Master secret sharing (TLS → DTLS)
- [ ] Certificate validation
- [ ] Always-On VPN detection
- [ ] Basic reconnection

### Nice to Have (P2)
- [ ] Split DNS
- [ ] Split tunneling
- [ ] DPD (standard)
- [ ] MTU optimization
- [ ] Compression

---

## Common Pitfalls

### ❌ Wrong
```go
// Missing copyright string
w.Header().Set("X-CSTP-Protocol", "1")
```

### ✅ Right
```go
// Exact copyright string required
w.Header().Set("X-CSTP-Protocol", "Copyright (c) 2004 Cisco Systems, Inc.")
```

---

### ❌ Wrong
```go
// Cookie without proper flags
w.Header().Set("Set-Cookie", "webvpn="+token)
```

### ✅ Right
```go
// Secure cookie with HttpOnly
w.Header().Set("Set-Cookie",
    fmt.Sprintf("webvpn=%s; Secure; HttpOnly; SameSite=Strict", token))
```

---

### ❌ Wrong
```go
// Missing session token in config
xml := `<auth id="COMPLETE"><config>...</config></auth>`
```

### ✅ Right
```go
// Session token required
xml := `<auth id="COMPLETE">
    <session-token>abc123</session-token>
    <config>...</config>
</auth>`
```

---

## Testing Your Implementation

### Test 1: Basic Connection
```bash
# Using OpenConnect client (easier for initial testing)
echo "password" | openconnect --protocol=anyconnect \
    --user=testuser \
    --passwd-on-stdin \
    vpn.example.com
```

### Test 2: Cisco Client
```bash
# Linux
/opt/cisco/secureclient/bin/vpn connect vpn.example.com

# Logs
tail -f /opt/cisco/secureclient/log/vpnagentd.log
```

### Test 3: Wireshark Capture
```bash
# Capture VPN traffic
sudo tcpdump -i any -w cisco-vpn.pcap "port 443"

# Analyze in Wireshark with filter:
# http or tls or dtls
```

---

## When Something Goes Wrong

### Client won't connect

**Check**:
1. Are all required HTTP headers present?
2. Is the copyright string exact?
3. Is the XML well-formed?
4. Is the session cookie being sent?

**Debug**:
```bash
# Enable debug logging
export CISCO_DEBUG=1
/opt/cisco/secureclient/bin/vpn -d connect vpn.example.com
```

### Authentication fails

**Check**:
1. Is XML response `<auth id="COMPLETE">`?
2. Does response include `<session-token>`?
3. Is cookie format correct?

**Debug**:
```go
// Log authentication flow
log.Printf("Auth request: %+v", authRequest)
log.Printf("Session created: %+v", session)
log.Printf("Cookie sent: %s", cookie)
```

### DTLS won't establish

**Check**:
1. Is UDP port 443 open?
2. Is master secret being shared?
3. Are cipher suites compatible?

**Debug**:
```bash
# Test UDP connectivity
nc -u vpn.example.com 443

# Check DTLS in Wireshark
udp.port == 443 && dtls
```

---

## Quick Reference

### Required HTTP Headers
```
X-CSTP-Version: 1
X-CSTP-Protocol: Copyright (c) 2004 Cisco Systems, Inc.
X-CSTP-Address-Type: IPv6,IPv4
X-CSTP-MTU: 1406
```

### XML Response Template
```xml
<?xml version="1.0" encoding="UTF-8"?>
<auth id="COMPLETE">
    <session-token>SESSION_TOKEN_HERE</session-token>
    <config>
        <client-ip-address>192.168.1.10</client-ip-address>
        <dns>
            <server>8.8.8.8</server>
        </dns>
        <mtu>1406</mtu>
    </config>
</auth>
```

### Error Response Template
```xml
<?xml version="1.0" encoding="UTF-8"?>
<auth id="AUTH_FAILED">
    <error>Invalid credentials</error>
</auth>
```

---

## Next Steps

1. **Read Full Docs**:
   - [CISCO_COMPATIBILITY_GUIDE.md](CISCO_COMPATIBILITY_GUIDE.md) - Complete implementation guide
   - [PROTOCOL_REFERENCE.md](PROTOCOL_REFERENCE.md) - Protocol specification

2. **Review Analysis**:
   - `/opt/projects/repositories/cisco-secure-client/analysis/REVERSE_ENGINEERING_FINDINGS.md`
   - All protocol details and binary analysis

3. **Start Implementation**:
   - Begin with Phase 1: Core Protocol
   - Implement HTTP headers and basic auth
   - Test with OpenConnect first, then Cisco client

4. **Dynamic Testing**:
   - Capture real Cisco client traffic
   - Compare with your implementation
   - Adjust based on actual behavior

---

## Getting Help

**Documentation**:
- This guide: Quick start
- Compatibility guide: Full implementation details
- Reverse engineering findings: Complete analysis

**Testing**:
- Use OpenConnect client first (simpler, open source)
- Test with Cisco client 5.1.2.42 (matches analysis)
- Capture and analyze traffic with Wireshark

**Common Issues**:
- Check Troubleshooting section in CISCO_COMPATIBILITY_GUIDE.md
- Review error codes in REVERSE_ENGINEERING_FINDINGS.md
- Compare packet captures with expected format

---

**Good luck with the implementation! 🚀**

---

*For detailed implementation guidance, see: [CISCO_COMPATIBILITY_GUIDE.md](CISCO_COMPATIBILITY_GUIDE.md)*
