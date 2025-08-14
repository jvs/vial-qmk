#!/usr/bin/env bash

git fetch && git pull && ./util/docker_build.sh lily58/rev1:jvs -e CONVERT_TO=rp2040_ce
