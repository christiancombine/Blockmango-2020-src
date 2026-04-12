#!/bin/bash

core_dir='engine-core'
core_root='../../../../../'
core_addr='ssh://git@gitlab001.sandboxol.cn:31006/game/blockmango-engine.git'

head_branch=$(sed -n '1p' ../../../../../.git/HEAD)
engine_branch=${head_branch#*ref: refs/heads/}
echo "engine_branch=${engine_branch}"

core_branch='core_old_'${engine_branch}
echo "core_branch=${core_branch}"

cd $core_root
if [ ! -d $core_dir ];then
  git clone -b ${core_branch} $core_addr $core_dir
  if [ $? != '0' ]
  then
   echo "clone ${core_branch} failed "
   exit 1
  fi
fi

cd $core_dir
git fetch --all
git branch -a | grep ${core_branch}
if [ $? != '0' ]
then
   echo "can't find ${core_branch}"
   exit 1
fi

git checkout -f ${core_branch}
git pull  origin $core_branch

