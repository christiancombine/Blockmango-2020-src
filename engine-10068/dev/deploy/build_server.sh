#!/bin/bash
set -o errexit
set -o nounset

JOB_NUM=2
if [ $# -eq 1 ]; then
    echo "recv job_num arg, num: $1"
    JOB_NUM=$1
fi

echo  -e "\033[32m[$0 compile job num :${JOB_NUM}]\033[0m" 

SCRIPT_PATH=$(cd "$(dirname "$0")"; pwd)
WORKING_DIR=$(cd $SCRIPT_PATH/..; pwd)
ENGINE_CORE_DIR="$WORKING_DIR/../engine-core/dev"
echo  -e "\033[32m[$0 current working dir:${WORKING_DIR}]\033[0m" 

export CC='ccache gcc'
export CXX='ccache g++'

mkdir -p $ENGINE_CORE_DIR/engine/build
cd $ENGINE_CORE_DIR/engine/build
cmake -DCMAKE_BUILD_TYPE=Debug -DDEBUG=1 ..
make -j$JOB_NUM VERBOSE=1
cd -

mkdir -p $WORKING_DIR/logic/build
cd $WORKING_DIR/logic/build
cmake -DCMAKE_BUILD_TYPE=Debug -DDEBUG=1 ..
make -j$JOB_NUM VERBOSE=1
cd -

mkdir -p $WORKING_DIR/server/build
cd $WORKING_DIR/server/build
rm -rf ../Bin/Debug
cmake -DCMAKE_BUILD_TYPE=Debug -DDEBUG=1 ..
make -j$JOB_NUM VERBOSE=1
cd -

echo -e "\033[32m[build done!!!!!!]\033[0m"
cp -f $WORKING_DIR/server/Bin/Debug/GameServer_d $WORKING_DIR/../res/client/


