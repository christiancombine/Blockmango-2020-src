#!/bin/bash
set -o errexit
set -o nounset

SCRIPT_PATH=$(cd "$(dirname "$0")"; pwd)

git co master && git pull && git co test && git merge master && git push && ./build_and_deploy.sh test current

