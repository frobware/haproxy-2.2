cmake_minimum_required(VERSION 3.20)

# Build setup configured for OpenShift v4.8, haproxy-2.2.13)

project(haproxy)
enable_language(C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

find_package(Git QUIET REQUIRED)

execute_process(COMMAND
  "${GIT_EXECUTABLE}" config --get remote.origin.url
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_ORIGIN_URL
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --dirty
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE res
    OUTPUT_VARIABLE HAPROXY_GIT_VERSION
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)

set_property(GLOBAL APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/.git/index")

option(ENABLE_COVERAGE "Build with code coverage" OFF)

if(ENABLE_COVERAGE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif()

string(TIMESTAMP TODAY "%Y-%m-%dT%H:%M:%S")

add_compile_options(-g -fno-strict-aliasing -Wdeclaration-after-statement -fwrapv -Wno-address-of-packed-member -Wformat -DCONFIG_HAP_LINUX_TPROXY)
add_compile_options(-DUSE_EPOLL -DUSE_NETFILTER -DUSE_PCRE -DUSE_POLL -DUSE_THREAD -DUSE_BACKTRACE -DUSE_TPROXY -DUSE_LINUX_TPROXY -DUSE_LINUX_SPLICE -DUSE_LIBCRYPT -DUSE_CRYPT_H -DUSE_GETADDRINFO -DUSE_OPENSSL -DUSE_FUTEX -DUSE_ACCEPT4  -DUSE_ZLIB  -DUSE_CPU_AFFINITY -DUSE_TFO -DUSE_DL -DUSE_RT -DUSE_PRCTL -DUSE_THREAD_DUMP -DUSE_PCRE)

add_compile_options(-DCONFIG_HAPROXY_VERSION="${HAPROXY_GIT_VERSION} ${GIT_ORIGIN_URL}")
add_compile_options(-DCONFIG_HAPROXY_DATE="${TODAY}")

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wno-padded -Wno-unused-parameter")
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(ENABLE_ASAN "Enable Address Sanitizer" ON)

if(ENABLE_ASAN)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(HAPROXY_SRCS
  src/mux_fcgi.c
  src/mux_h1.c
  src/mux_h2.c
  src/backend.c
  src/cfgparse.c
  src/cli.c
  src/cfgparse-listen.c
  src/stats.c
  src/http_ana.c
  src/stream.c
  src/check.c
  src/sample.c
  src/tools.c
  src/server.c
  src/listener.c
  src/tcpcheck.c
  src/pattern.c
  src/log.c
  src/stick_table.c
  src/flt_spoe.c
  src/stream_interface.c
  src/filters.c
  src/http_fetch.c
  src/map.c
  src/session.c
  src/sink.c
  src/flt_http_comp.c
  src/debug.c
  src/tcp_rules.c
  src/haproxy.c
  src/peers.c
  src/flt_trace.c
  src/queue.c
  src/proxy.c
  src/http_htx.c
  src/dns.c
  src/raw_sock.c
  src/pool.c
  src/http_act.c
  src/http_rules.c
  src/compression.c
  src/cfgparse-global.c
  src/payload.c
  src/signal.c
  src/activity.c
  src/mworker.c
  src/cache.c
  src/proto_uxst.c
  src/lb_chash.c
  src/connection.c
  src/proto_tcp.c
  src/http_conv.c
  src/arg.c
  src/lb_fas.c
  src/xprt_handshake.c
  src/fcgi-app.c
  src/applet.c
  src/acl.c
  src/task.c
  src/ring.c
  src/vars.c
  src/trace.c
  src/mux_pt.c
  src/xxhash.c
  src/mworker-prog.c
  src/h1_htx.c
  src/frontend.c
  src/extcheck.c
  src/channel.c
  src/action.c
  src/mailers.c
  src/proto_sockpair.c
  src/ebmbtree.c
  src/thread.c
  src/lb_fwrr.c
  src/time.c
  src/regex.c
  src/lb_fwlc.c
  src/htx.c
  src/h2.c
  src/hpack-tbl.c
  src/lru.c
  src/wdt.c
  src/lb_map.c
  src/eb32sctree.c
  src/ebistree.c
  src/h1.c
  src/sha1.c
  src/http.c
  src/fd.c
  src/ev_select.c
  src/chunk.c
  src/hash.c
  src/hpack-dec.c
  src/freq_ctr.c
  src/http_acl.c
  src/dynbuf.c
  src/uri_auth.c
  src/protocol.c
  src/auth.c
  src/ebsttree.c
  src/pipe.c
  src/hpack-enc.c
  src/fcgi.c
  src/eb64tree.c
  src/dict.c
  src/shctx.c
  src/ebimtree.c
  src/eb32tree.c
  src/ebtree.c
  src/dgram.c
  src/hpack-huff.c
  src/base64.c
  src/version.c
  # USE_OPENSSL
  src/ssl_sample.c
  src/ssl_sock.c
  src/ssl_crtlist.c
  src/ssl_ckch.c
  src/ssl_utils.c
  src/cfgparse-ssl.c

  src/ev_epoll.c
  src/ev_poll.c
  )

set(OPENSHIFT_SRCS
  # src/openshift/close.c
  # src/openshift/connect.c
  # src/openshift/logconn.c
  # src/openshift/ocprintf.c
  )

set_source_files_properties(SOURCE ${OPENSHIFT_SRCS} PROPERTY COMPILE_FLAGS "-Wall -Werror -Wformat")

add_executable(haproxy ${HAPROXY_SRCS} ${OPENSHIFT_SRCS})
target_link_libraries(haproxy -lcrypt -lz -ldl -lrt -lpthread -Wl,--export-dynamic -lssl -lcrypto -ldl -L/usr/lib -lpcreposix -lpcre)

set(LLVM_LINKER_DETECTED YES CACHE INTERNAL "")

