#!/usr/bin/env bash

set -eu

make -f Makefile -j $(nproc) \
     CPU="generic" \
     TARGET="linux-glibc" \
     USE_REGPARM=1 \
     USE_OPENSSL=1 \
     USE_PCRE=1 \
     USE_ZLIB=1 \
     USE_CRYPT_H=1 \
     USE_LINUX_TPROXY=1 \
     USE_GETADDRINFO=1 \
     "$@"
