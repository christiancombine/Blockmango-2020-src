#!/bin/bash -x
#set -e


#if [ $# -gt 3 ];then
WORKING_DIR="$1"
targetDir="$2"
old_or_new="$3"
engineversion="$4"
useCmd="$5"
#else
#WORKING_DIR="$1"
#targetDir="$2"
#useCmd="$3"
#fi


if [ "$old_or_new" == "new" ];then
    engineversionjson=`cat $WORKING_DIR/res/Media/engineVersion.json|grep engineVersion|awk -F: '{print $NF}'|tr -d " ,"`
else
    engineversionjson=`cat $WORKING_DIR/res/client/Media/engineVersion.json|grep engineVersion|awk -F: '{print $NF}'|tr -d " ,"`
fi

if [ -n "$engineversion" ];then
    if [ -n "$useCmd" ] && [ "$engineversion" != "$engineversionjson" ];then
        echo "error input invalid engine version $engineversion "
        exit 1
    elif [ -n "$userCmd" ];then
        engineversion=${engineversionjson}
    fi
    #sed -i 's/[0-9]\+/${engineversion}/g' ${targetDir}/${engineversion}/engineVersion.json
fi
mkdir -p ${targetDir}/${engineversion}
rm -rf ${targetDir}/${engineversion}/*

if [ "$old_or_new" == "new" ];then
    #copy new engine files
    cp $WORKING_DIR/res/GameServer_d ${targetDir}/${engineversion}
    cp $WORKING_DIR/res/resource.cfg ${targetDir}/${engineversion}
    cp $WORKING_DIR/res/Media/engineVersion.json ${targetDir}/${engineversion}
    cp -r $WORKING_DIR/res/Media/Setting/*.csv  ${targetDir}/${engineversion}
    cp -r $WORKING_DIR/res/Media/Setting/words.txt ${targetDir}/${engineversion}
    cp -r $WORKING_DIR/res/lua ${targetDir}/${engineversion}
elif [ "$old_or_new" == "old" ];then
    #copy old engine files
    cp $WORKING_DIR/res/client/GameServer_d ${targetDir}/${engineversion}
    cp $WORKING_DIR/res/client/resource.cfg ${targetDir}/${engineversion}
    cp $WORKING_DIR/res/client/Media/engineVersion.json ${targetDir}/${engineversion}
    cp $WORKING_DIR/res/client/Media/Scripts/ScriptSetting.csv ${targetDir}/${engineversion}
    cp -r $WORKING_DIR/res/client/Media/Scripts/GameSetting ${targetDir}/${engineversion}
    cp -r $WORKING_DIR/res/client/Media/Setting/*.csv  ${targetDir}/${engineversion}
    cp -r $WORKING_DIR/res/client/Media/Setting/GameTip ${targetDir}/${engineversion}
    cp -r $WORKING_DIR/res/client/recipe ${targetDir}/${engineversion}
    cp -r $WORKING_DIR/res/client/bt ${targetDir}/${engineversion}
else
    echo "cannot verify new engine or old engine $old_or_new"
    exit 1
fi
sed -i "s/[0-9]\+/${engineversion}/g" ${targetDir}/${engineversion}/engineVersion.json
   
if [ "$useCmd" == "true" ]
then
    sed -i 's/true/false/g' ${targetDir}/${engineversion}/engineVersion.json
fi
cd ${targetDir}
tar czf  ${engineversion}.tar.gz ${engineversion} 
echo "${targetDir}/${engineversion}.tar.gz"
