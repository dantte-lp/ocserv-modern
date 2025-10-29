/*
 * Copyright (C) 2025 ocserv-modern Contributors
 *
 * This file is part of ocserv-modern.
 *
 * ocserv-modern is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * ocserv-modern is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * GnuTLS Priority String Parser Implementation
 *
 * This file implements the four-phase priority string processing:
 * 1. Tokenization: Split string into tokens
 * 2. Parsing: Build configuration from tokens
 * 3. Mapping: Translate to wolfSSL configuration
 * 4. Application: Apply to wolfSSL context
 *
 * Design Principles:
 * - Zero heap allocations (stack-only)
 * - Thread-safe (no global state except thread-local error info)
 * - Comprehensive error reporting
 * - Efficient O(n) algorithms
 */

#include "priority_parser.h"
#include "tls_wolfssl.h"
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdatomic.h>

// C23 standard compliance
#if __STDC_VERSION__ < 202000L
#error "This code requires C23 standard (ISO/IEC 9899:2024) or C2x support (GCC 14+)"
#endif

/* ============================================================================
 * Thread-Local Error Storage
 * ============================================================================ */

// Thread-local storage for error information
static _Thread_local priority_error_info_t tls_last_error = {0};

/**
 * Set last error information (internal helper)
 */
static inline void set_last_error(int error_code,
                                   size_t error_position,
                                   const char *error_token,
                                   const char *error_message)
{
    tls_last_error.error_code = error_code;
    tls_last_error.error_position = error_position;

    if (error_token != nullptr) {
        snprintf(tls_last_error.error_token,
                 sizeof(tls_last_error.error_token),
                 "%s",
                 error_token);
    } else {
        tls_last_error.error_token[0] = '\0';
    }

    if (error_message != nullptr) {
        snprintf(tls_last_error.error_message,
                 sizeof(tls_last_error.error_message),
                 "%s",
                 error_message);
    } else {
        tls_last_error.error_message[0] = '\0';
    }
}

int priority_get_last_error(priority_error_info_t *error_info)
{
    if (error_info == nullptr) {
        return PRIORITY_E_NULL_POINTER;
    }

    *error_info = tls_last_error;
    return PRIORITY_E_SUCCESS;
}

/* ============================================================================
 * Error Message Strings
 * ============================================================================ */

static const char *priority_error_strings[] = {
    [0] = "Success",
    [-PRIORITY_E_SYNTAX_ERROR] = "Invalid priority string syntax",
    [-PRIORITY_E_UNKNOWN_KEYWORD] = "Unknown priority keyword",
    [-PRIORITY_E_UNKNOWN_MODIFIER] = "Unknown priority modifier",
    [-PRIORITY_E_CONFLICT] = "Conflicting priority specifications",
    [-PRIORITY_E_UNSUPPORTED] = "Unsupported priority feature",
    [-PRIORITY_E_TOO_COMPLEX] = "Priority string too complex (too many tokens)",
    [-PRIORITY_E_BUFFER_TOO_SMALL] = "Output buffer too small",
    [-PRIORITY_E_NULL_POINTER] = "nullptr parameter",
    [-PRIORITY_E_INVALID_VERSION] = "Invalid TLS version specification",
    [-PRIORITY_E_INVALID_CIPHER] = "Invalid cipher name",
    [-PRIORITY_E_MAPPER_FAILED] = "Failed to map to wolfSSL configuration",
};

const char* priority_strerror(int error_code)
{
    if (error_code == PRIORITY_E_SUCCESS) {
        return priority_error_strings[0];
    }

    int index = -error_code;
    if (index < 0 || index >= (int)(sizeof(priority_error_strings) / sizeof(char*))) {
        return "Unknown error";
    }

    const char *msg = priority_error_strings[index];
    return (msg != nullptr) ? msg : "Unknown error";
}

/* ============================================================================
 * Token Type Classification
 * ============================================================================ */

static const char *token_type_names[] = {
    [TOKEN_UNKNOWN] = "UNKNOWN",
    [TOKEN_KEYWORD] = "KEYWORD",
    [TOKEN_MODIFIER] = "MODIFIER",
    [TOKEN_VERSION] = "VERSION",
    [TOKEN_CIPHER] = "CIPHER",
    [TOKEN_KX] = "KEY_EXCHANGE",
    [TOKEN_MAC] = "MAC",
    [TOKEN_SIGN] = "SIGNATURE",
    [TOKEN_GROUP] = "GROUP",
    [TOKEN_OPERATOR] = "OPERATOR",
};

const char* priority_token_type_name(token_type_t type)
{
    if (type < 0 || type >= (int)(sizeof(token_type_names) / sizeof(char*))) {
        return "INVALID";
    }
    return token_type_names[type];
}

/**
 * Known base keywords
 */
static const char *base_keywords[] = {
    "NORMAL",
    "PERFORMANCE",
    "SECURE128",
    "SECURE192",
    "SECURE256",
    "PFS",
    "LEGACY",
    "SUITEB128",
    "SUITEB192",
    "NONE",
    "SYSTEM",
    nullptr  // Sentinel
};

/**
 * Known modifiers (% prefix)
 */
static const char *known_modifiers[] = {
    "%SERVER_PRECEDENCE",
    "%COMPAT",
    "%NO_EXTENSIONS",
    "%FORCE_SESSION_HASH",
    "%DUMBFW",
    "%FALLBACK_SCSV",
    "%NO_TICKETS",
    "%DISABLE_SAFE_RENEGOTIATION",
    "%UNSAFE_RENEGOTIATION",
    "%PARTIAL_RENEGOTIATION",
    "%PROFILE_LOW",
    "%PROFILE_MEDIUM",
    "%PROFILE_HIGH",
    "%PROFILE_ULTRA",
    "%PROFILE_FUTURE",
    "%PROFILE_SUITEB128",
    "%PROFILE_SUITEB192",
    nullptr  // Sentinel
};

/**
 * Classify token based on content
 */
static token_type_t classify_token(const char *start, size_t length)
{
    // Create null-terminated string for comparison
    char token[PRIORITY_MAX_TOKEN_LEN];
    if (length >= sizeof(token)) {
        return TOKEN_UNKNOWN;
    }
    memcpy(token, start, length);
    token[length] = '\0';

    // Check if it's a modifier (starts with %)
    if (token[0] == '%') {
        for (const char **mod = known_modifiers; *mod != nullptr; mod++) {
            if (strcmp(token, *mod) == 0) {
                return TOKEN_MODIFIER;
            }
        }
        return TOKEN_UNKNOWN;  // Unknown modifier
    }

    // Check if it's a base keyword
    for (const char **kw = base_keywords; *kw != nullptr; kw++) {
        if (strcmp(token, *kw) == 0) {
            return TOKEN_KEYWORD;
        }
    }

    // Check if it's a version specification (VERS-*)
    if (strncmp(token, "VERS-", 5) == 0) {
        return TOKEN_VERSION;
    }

    // Check if it's a signature specification (SIGN-*)
    if (strncmp(token, "SIGN-", 5) == 0) {
        return TOKEN_SIGN;
    }

    // Check if it's a group specification (GROUP-*)
    if (strncmp(token, "GROUP-", 6) == 0) {
        return TOKEN_GROUP;
    }

    // Check for cipher keywords
    if (strstr(token, "AES") != nullptr ||
        strstr(token, "CHACHA20") != nullptr ||
        strstr(token, "CAMELLIA") != nullptr ||
        strstr(token, "ARCFOUR") != nullptr ||
        strstr(token, "3DES") != nullptr ||
        strstr(token, "NULL") != nullptr ||
        strstr(token, "CIPHER") != nullptr) {
        return TOKEN_CIPHER;
    }

    // Check for key exchange keywords
    if (strstr(token, "ECDHE") != nullptr ||
        strstr(token, "DHE") != nullptr ||
        strstr(token, "RSA") != nullptr ||
        strstr(token, "ECDSA") != nullptr ||
        strstr(token, "PSK") != nullptr ||
        strcmp(token, "KX-ALL") == 0) {
        return TOKEN_KX;
    }

    // Check for MAC keywords
    if (strstr(token, "SHA") != nullptr ||
        strstr(token, "MD5") != nullptr ||
        strcmp(token, "AEAD") == 0 ||
        strcmp(token, "MAC-ALL") == 0) {
        return TOKEN_MAC;
    }

    return TOKEN_UNKNOWN;
}

/* ============================================================================
 * Phase 1: Tokenization
 * ============================================================================ */

int priority_tokenize(const char *priority, token_list_t *tokens)
{
    if (priority == nullptr || tokens == nullptr) {
        set_last_error(PRIORITY_E_NULL_POINTER, 0, nullptr,
                      "nullptr parameter to priority_tokenize");
        return PRIORITY_E_NULL_POINTER;
    }

    // Initialize token list
    memset(tokens, 0, sizeof(*tokens));
    tokens->input = priority;

    const char *p = priority;
    bool is_addition = true;  // Default operator is implicit addition
    bool is_negation = false;

    while (*p != '\0') {
        // Skip whitespace
        while (isspace((unsigned char)*p)) {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        // Check for operator
        if (*p == '+' || *p == '-' || *p == '!' || *p == ':') {
            // Record operator
            if (tokens->count >= PRIORITY_MAX_TOKENS) {
                set_last_error(PRIORITY_E_TOO_COMPLEX, p - priority, nullptr,
                              "Too many tokens in priority string");
                return PRIORITY_E_TOO_COMPLEX;
            }

            // Determine operator semantics
            if (*p == '+') {
                is_addition = true;
                is_negation = false;
            } else if (*p == '-' || *p == '!') {
                is_addition = false;
                is_negation = true;
            } else if (*p == ':') {
                // Colon is a separator, doesn't change addition/negation state
                // but resets to default (addition) for next token
                is_addition = true;
                is_negation = false;
            }

            p++;  // Skip operator
            continue;
        }

        // Read token until next operator, colon, or end
        const char *token_start = p;
        while (*p != '\0' && *p != '+' && *p != '-' && *p != '!' && *p != ':' &&
               !isspace((unsigned char)*p)) {
            p++;
        }

        size_t token_len = (size_t)(p - token_start);
        if (token_len == 0) {
            continue;  // Empty token, skip
        }

        if (token_len >= PRIORITY_MAX_TOKEN_LEN) {
            char long_token[64];
            snprintf(long_token, sizeof(long_token), "%.*s...", (int)40, token_start);
            set_last_error(PRIORITY_E_SYNTAX_ERROR, token_start - priority, long_token,
                          "Token too long");
            return PRIORITY_E_SYNTAX_ERROR;
        }

        // Add token to list
        if (tokens->count >= PRIORITY_MAX_TOKENS) {
            set_last_error(PRIORITY_E_TOO_COMPLEX, token_start - priority, nullptr,
                          "Too many tokens in priority string");
            return PRIORITY_E_TOO_COMPLEX;
        }

        token_t *token = &tokens->tokens[tokens->count++];
        token->start = token_start;
        token->length = token_len;
        token->is_addition = is_addition;
        token->is_negation = is_negation;
        token->type = classify_token(token_start, token_len);

        // Reset operator state to default for next token (implicit addition)
        is_addition = true;
        is_negation = false;
    }

    return PRIORITY_E_SUCCESS;
}

/* ============================================================================
 * Phase 2: Parsing (to be implemented)
 * ============================================================================ */

void priority_config_init(priority_config_t *config)
{
    if (config == nullptr) {
        return;
    }

    memset(config, 0, sizeof(*config));
    config->min_security_bits = 0;  // No minimum by default
}

int priority_parse(const token_list_t *tokens, priority_config_t *config)
{
    // TODO: Implement in next commit
    (void)tokens;
    (void)config;
    return PRIORITY_E_UNSUPPORTED;
}

/* ============================================================================
 * Phase 3: Mapping (to be implemented)
 * ============================================================================ */

void wolfssl_config_init(wolfssl_config_t *wolfssl_cfg)
{
    if (wolfssl_cfg == nullptr) {
        return;
    }

    memset(wolfssl_cfg, 0, sizeof(*wolfssl_cfg));
    wolfssl_cfg->min_version = 0;  // Will be set based on config
    wolfssl_cfg->max_version = 0;
}

int priority_map_to_wolfssl(const priority_config_t *config,
                             wolfssl_config_t *wolfssl_cfg)
{
    // TODO: Implement in next commit
    (void)config;
    (void)wolfssl_cfg;
    return PRIORITY_E_UNSUPPORTED;
}

/* ============================================================================
 * Phase 4: Application (to be implemented)
 * ============================================================================ */

int priority_apply_wolfssl_config(void *wolf_ctx,
                                   const wolfssl_config_t *wolfssl_cfg)
{
    // TODO: Implement in next commit
    (void)wolf_ctx;
    (void)wolfssl_cfg;
    return PRIORITY_E_UNSUPPORTED;
}

/* ============================================================================
 * Public API (to be implemented)
 * ============================================================================ */

int tls_set_priority_string(tls_context_t *ctx, const char *priority)
{
    // TODO: Implement in next commit
    (void)ctx;
    (void)priority;
    return PRIORITY_E_UNSUPPORTED;
}

int tls_validate_priority_string(const char *priority,
                                  char *error_msg,
                                  size_t error_msg_len)
{
    // TODO: Implement in next commit
    (void)priority;
    (void)error_msg;
    (void)error_msg_len;
    return PRIORITY_E_UNSUPPORTED;
}

/* ============================================================================
 * Utility Functions (to be implemented)
 * ============================================================================ */

size_t priority_config_dump(const priority_config_t *config,
                             char *buffer,
                             size_t buffer_len)
{
    // TODO: Implement in next commit
    (void)config;
    (void)buffer;
    (void)buffer_len;
    return 0;
}

size_t wolfssl_config_dump(const wolfssl_config_t *wolfssl_cfg,
                            char *buffer,
                            size_t buffer_len)
{
    // TODO: Implement in next commit
    (void)wolfssl_cfg;
    (void)buffer;
    (void)buffer_len;
    return 0;
}
