# CMakeLists.txt for ocserv-modern
# Modern C23 VPN server with TLS abstraction layer
cmake_minimum_required(VERSION 3.20)

project(ocserv-modern
    VERSION 2.0.0
    DESCRIPTION "Modern C23 VPN server with TLS abstraction layer"
    LANGUAGES C
)

# Require C23 standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build options
option(USE_WOLFSSL "Use wolfSSL backend instead of GnuTLS" ON)
option(BUILD_TESTING "Build unit tests" ON)
option(BUILD_POC "Build proof-of-concept server/client" ON)
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(BUILD_DOCUMENTATION "Build API documentation with Doxygen" ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-strong")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -march=native -mtune=native -DNDEBUG -D_FORTIFY_SOURCE=2")

# Linker flags for security hardening
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro -Wl,-z,now")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro -Wl,-z,now")

# Sanitizers
if(ENABLE_SANITIZERS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
endif()

# Code coverage
if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")
endif()

# Find required packages
find_package(PkgConfig REQUIRED)

# TLS backend selection
if(USE_WOLFSSL)
    pkg_check_modules(WOLFSSL REQUIRED wolfssl)
    set(TLS_BACKEND "wolfssl")
    set(TLS_DEFINITIONS USE_WOLFSSL)
    set(TLS_INCLUDE_DIRS ${WOLFSSL_INCLUDE_DIRS})
    set(TLS_LIBRARIES ${WOLFSSL_LIBRARIES})
    set(TLS_BACKEND_SOURCE src/crypto/tls_wolfssl.c)
    message(STATUS "Using wolfSSL backend")
else()
    pkg_check_modules(GNUTLS REQUIRED gnutls>=3.8.0)
    set(TLS_BACKEND "gnutls")
    set(TLS_DEFINITIONS USE_GNUTLS)
    set(TLS_INCLUDE_DIRS ${GNUTLS_INCLUDE_DIRS})
    set(TLS_LIBRARIES ${GNUTLS_LIBRARIES})
    set(TLS_BACKEND_SOURCE src/crypto/tls_gnutls.c)
    message(STATUS "Using GnuTLS backend")
endif()

# Find other dependencies
pkg_check_modules(JSON_C REQUIRED json-c)
pkg_check_modules(PAM REQUIRED pam)
pkg_check_modules(LIBSECCOMP REQUIRED libseccomp)
pkg_check_modules(LZ4 REQUIRED liblz4)
pkg_check_modules(LIBNL3 REQUIRED libnl-3.0)
pkg_check_modules(KRB5 REQUIRED krb5)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/crypto
    ${TLS_INCLUDE_DIRS}
    ${JSON_C_INCLUDE_DIRS}
    ${PAM_INCLUDE_DIRS}
    ${LIBSECCOMP_INCLUDE_DIRS}
    ${LZ4_INCLUDE_DIRS}
    ${LIBNL3_INCLUDE_DIRS}
    ${KRB5_INCLUDE_DIRS}
)

# TLS abstraction library
add_library(tls_abstract STATIC
    src/crypto/tls_abstract.c
    ${TLS_BACKEND_SOURCE}
)

target_compile_definitions(tls_abstract PRIVATE ${TLS_DEFINITIONS})
target_link_libraries(tls_abstract PRIVATE ${TLS_LIBRARIES})

# Install library and headers
install(TARGETS tls_abstract
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES src/crypto/tls_abstract.h
    DESTINATION include/ocserv-modern
)

# Proof of Concept binaries
if(BUILD_POC)
    add_executable(poc-server tests/poc/tls_poc_server.c)
    target_link_libraries(poc-server PRIVATE tls_abstract ${TLS_LIBRARIES})
    target_compile_definitions(poc-server PRIVATE ${TLS_DEFINITIONS})

    add_executable(poc-client tests/poc/tls_poc_client.c)
    target_link_libraries(poc-client PRIVATE tls_abstract ${TLS_LIBRARIES})
    target_compile_definitions(poc-client PRIVATE ${TLS_DEFINITIONS})

    message(STATUS "Building PoC server and client")
endif()

# Unit tests
if(BUILD_TESTING)
    enable_testing()

    # Find Unity testing framework
    find_path(UNITY_INCLUDE_DIR unity/unity.h
        PATHS /usr/local/include /usr/include
    )
    find_path(UNITY_SRC_DIR unity.c
        PATHS /usr/local/src/unity /usr/src/unity
    )

    if(UNITY_INCLUDE_DIR AND UNITY_SRC_DIR)
        message(STATUS "Found Unity testing framework")

        # Create Unity library
        add_library(unity STATIC ${UNITY_SRC_DIR}/unity.c)
        target_include_directories(unity PUBLIC ${UNITY_INCLUDE_DIR})

        # Unit tests for TLS backends
        if(USE_WOLFSSL)
            add_executable(test_tls_wolfssl tests/unit/test_tls_wolfssl.c)
            target_link_libraries(test_tls_wolfssl PRIVATE tls_abstract unity ${TLS_LIBRARIES})
            target_compile_definitions(test_tls_wolfssl PRIVATE ${TLS_DEFINITIONS})
            add_test(NAME test_tls_wolfssl COMMAND test_tls_wolfssl)
        else()
            add_executable(test_tls_gnutls tests/unit/test_tls_gnutls.c)
            target_link_libraries(test_tls_gnutls PRIVATE tls_abstract unity ${TLS_LIBRARIES})
            target_compile_definitions(test_tls_gnutls PRIVATE ${TLS_DEFINITIONS})
            add_test(NAME test_tls_gnutls COMMAND test_tls_gnutls)
        endif()

        message(STATUS "Building unit tests with Unity framework")
    else()
        message(WARNING "Unity testing framework not found - skipping unit tests")
    endif()
endif()

# Doxygen documentation
if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # Check if Doxyfile.in exists, otherwise use default Doxyfile
        if(EXISTS ${DOXYGEN_IN})
            configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
            set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
        endif()

        add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )

        message(STATUS "Doxygen documentation target enabled - run 'make doc'")
    else()
        message(WARNING "Doxygen not found - documentation will not be built")
    endif()
endif()

# Code formatting target
add_custom_target(format
    COMMAND find ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests -name "*.c" -o -name "*.h" | xargs clang-format -i
    COMMENT "Formatting code with clang-format"
    VERBATIM
)

# Static analysis target
add_custom_target(lint
    COMMAND clang-tidy ${CMAKE_SOURCE_DIR}/src/crypto/*.c -p ${CMAKE_BINARY_DIR} -- -I${CMAKE_SOURCE_DIR}/src/crypto ${TLS_INCLUDE_DIRS}
    COMMENT "Running static analysis with clang-tidy"
    VERBATIM
)

# Cppcheck target
add_custom_target(cppcheck
    COMMAND cppcheck --enable=all --inconclusive --std=c11 --suppress=missingIncludeSystem
        -I${CMAKE_SOURCE_DIR}/src/crypto ${CMAKE_SOURCE_DIR}/src
    COMMENT "Running cppcheck static analysis"
    VERBATIM
)

# Print configuration summary
message(STATUS "")
message(STATUS "ocserv-modern Configuration Summary")
message(STATUS "===================================")
message(STATUS "Version:           ${PROJECT_VERSION}")
message(STATUS "Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "TLS backend:       ${TLS_BACKEND}")
message(STATUS "C compiler:        ${CMAKE_C_COMPILER}")
message(STATUS "C flags:           ${CMAKE_C_FLAGS}")
message(STATUS "Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Build options:")
message(STATUS "  Build tests:     ${BUILD_TESTING}")
message(STATUS "  Build PoC:       ${BUILD_POC}")
message(STATUS "  Sanitizers:      ${ENABLE_SANITIZERS}")
message(STATUS "  Coverage:        ${ENABLE_COVERAGE}")
message(STATUS "  Documentation:   ${BUILD_DOCUMENTATION}")
message(STATUS "")
