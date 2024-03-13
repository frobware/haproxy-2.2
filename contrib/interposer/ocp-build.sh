#!/usr/bin/env bash

set -eux

: "${DEBUG_CFLAGS:=""}"
VERSION=$(cat VERSION)

if [[ ${USE_CONTRIB_INTERPOSER_ACCEPT:-0} -eq 1 ]]; then
    DEBUG_CFLAGS+=" -DUSE_CONTRIB_INTERPOSER_ACCEPT"
    VERSION+="-accept-interposer"
fi

if [[ ${USE_CONTRIB_INTERPOSER_MALLOC:-0} -eq 1 ]]; then
    DEBUG_CFLAGS+=" -DUSE_CONTRIB_INTERPOSER_MALLOC"
    VERSION+="-malloc-interposer"
fi

export VERSION

if [[ ! -z "${VERSION:-}" ]]; then
    export IGNOREGIT=1
    export VERDATE="$(date +%FT%H-%M-%S-%Z)"
    export VERSION
fi

make CPU="generic" \
     TARGET="linux-glibc" \
     USE_OPENSSL=1 \
     USE_PCRE=1 \
     USE_ZLIB=1 \
     USE_CRYPT_H=1 \
     USE_LINUX_TPROXY=1 \
     USE_GETADDRINFO=1 \
     V=1 \
     EXTRA_OBJS="contrib/interposer/accept.o" \
     DEBUG_CFLAGS="$DEBUG_CFLAGS" \
     "$@" \

# Everything after V=1 are custom additions; OpenShift does not build
# with debug or the contrib extras.
