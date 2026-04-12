#!/bin/bash

set -o errexit
set -o nounset

SCRIPT_PATH=$(cd "$(dirname "$0")"; pwd)
WORKING_DIR=$(cd $SCRIPT_PATH/..; pwd)
ENGINE_CORE_DIR="$WORKING_DIR/../engine-core/dev"

echo  -e "\033[32m[$0 currently not used]\033[0m" 
exit 0

echo  -e "\033[32m[$0 current working dir:${WORKING_DIR}]\033[0m" 

engine_build_dir="$ENGINE_CORE_DIR/engine/build_release"
if [ ! -d $engine_build_dir ] ; then
    mkdir -p $engine_build_dir
fi
cd $engine_build_dir
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4 VERBOSE=1
cd -


logic_build_dir="$WORKING_DIR/logic/build_release"
if [ ! -d $logic_build_dir ] ; then
    mkdir -p $logic_build_dir
fi
cd $logic_build_dir
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4 VERBOSE=1
cd -

server_build_dir="$WORKING_DIR/server/build_release"
if [ ! -d $server_build_dir ] ; then
    mkdir -p $server_build_dir
fi
cd $server_build_dir
rm -f ../Bin/Release/GameServer
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4 VERBOSE=1
cd -

echo "build done!!!!!!"
cp -rf $WORKING_DIR/server/Bin/Release/GameServer $WORKING_DIR/../res/client/

