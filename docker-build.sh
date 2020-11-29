#!/bin/bash
set -e
BASE=$(realpath $(dirname $0))
git fetch --unshallow
docker run --rm -v $BASE:/root/suite -w /root/suite devkitpro/devkitarm make
mkdir -p deploy
zip deploy/suite-v0-r$(git rev-list --count HEAD).zip suite.gba
docker run --rm -v $BASE:/root/suite -w /root/suite devkitpro/devkitarm make clean
