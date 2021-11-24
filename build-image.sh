#!/usr/bin/env bash

set -eu

username=amcdermo
imagename=haproxy-ocp48-bz2021495
tagname=latest

#toolbox run --container centos7 make
podman build -t ${username}/${imagename}:${tagname} -f Dockerfile .

if [[ $# -ge 1 ]]; then
    podman tag ${username}/${imagename}:${tagname} quay.io/${username}/${imagename}:${tagname}
    podman push quay.io/${username}/${imagename}:${tagname}
fi
