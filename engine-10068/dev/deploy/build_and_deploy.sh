#!/bin/bash
set -o errexit
set -o nounset

SCRIPT_PATH=$(cd "$(dirname "$0")"; pwd)
WORKING_DIR=$(cd $SCRIPT_PATH/..; pwd)
echo  -e "\033[32m[$0 current working dir:${WORKING_DIR}]\033[0m"
branch_array=("test" "test_1" "test_2" "test_3" "test_4" "test_5" "test_102" "test_103" "release_china" "release_seoul" "ios")


show_usage(){
    ba="["
    for br in ${branch_array[@]}  
    do  
        ba=${ba}${br}"|" 
    done 
    ba=${ba}"]"
    echo "Usage: $0 ${ba} [current|pull]"
}

copy_file(){
    echo "the deploy dir is ${deployDir}"
    if [ "${deployServer}" != "" ]; then
        server="${deployServer}:"
        ssh ${deployServer} "mkdir -p ${deployDir}"
    else
       mkdir -p ${deployDir}
    fi

    scp  $WORKING_DIR/../res/client/GameServer_d ${server}${deployDir}
    scp  $WORKING_DIR/../res/client/resource.cfg ${server}${deployDir}
    scp  $WORKING_DIR/../res/client/Media/engineVersion.json ${server}${deployDir}
    scp  $WORKING_DIR/../res/client/Media/Scripts/ScriptSetting.csv ${server}${deployDir}
    scp  $WORKING_DIR/../res/client/Media/Setting/*.csv ${server}${deployDir}
    scp -r $WORKING_DIR/../res/client/Media/Scripts/GameSetting ${server}${deployDir}
    scp -r $WORKING_DIR/../res/client/Media/Setting/GameTip ${server}${deployDir}
    scp -r $WORKING_DIR/../res/client/recipe ${server}${deployDir}
    scp -r $WORKING_DIR/../res/client/bt ${server}${deployDir}
}

tar_file(){
    SSSBucket="blockmango-res"
    SSSObject="engine-files-cn-northwest-1-dev"
    echo "begin tar file, deploy server:[${deployServer}], deploy dir:[${deployDir}]"
    if [ "${deployServer}" != "" ]; then
        ssh ${deployServer} "cd ${deployDir}/.. && tar zcvf ${engineVersion}.tar.gz ${engineVersion} && ~/.local/bin/aws s3 cp ${engineVersion}.tar.gz s3://${SSSBucket}/${SSSObject}/${engineVersion}.tar.gz"
    else
        cd ${deployDir}/.. && tar zcvf ${engineVersion}.tar.gz ${engineVersion}
    fi
	echo "tar file done, deploy server:[${deployServer}], deploy dir:[${deployDir}]"
}

if [ $# -ne 2 ]; then
    echo "args num error, args num: $#"
    show_usage
    exit 1
fi

isArg1Valid=0
for br in ${branch_array[@]}  
do  
    if [ "${br}" == "$1" ]; then
        isArg1Valid=1
    fi  
done

if [ ${isArg1Valid} -ne 1 ]; then
    echo "region arg error: $1"
    show_usage
    exit 1
fi

if [ $2 == "pull" ]; then
    if [ $1 == "ios" ]; then
        git checkout release_seoul
    else
        git checkout $1
    fi
    git pull
elif [ $2 == "current" ]; then
    echo "use current source code"
else
    echo "update arg error: $2"
    show_usage
    exit 1
fi

branch=$(git symbolic-ref --short HEAD)
engineVersion=$(cat $WORKING_DIR/../res/client/Media/engineVersion.json | grep "engineVersion" | awk -F ": " '{print $2}' | awk -F "," '{print $1}')
if [ $1 == "test" ] ; then
    engineVersion="90900"
elif [ $1 == "test_1" ]; then
    engineVersion="90001"
elif [ $1 == "test_2" ]; then
    engineVersion="90002"
elif [ $1 == "test_3" ]; then
    engineVersion="90003"
elif [ $1 == "test_4" ]; then
    engineVersion="90004"
elif [ $1 == "test_5" ]; then
    engineVersion="90005"
elif [ $1 == "test_102" ]; then
    engineVersion="90102"
elif [ $1 == "test_103" ]; then
    engineVersion="90103"
fi

server=""
deployDir=""
deployServer="console@172.31.0.17"
echo "current branch is $branch, engineVersion is $engineVersion"

case $1 in
    test)
        docker run -it --rm -v /data/test/blockmango-client:/data/code/blockmango -v /data/ccache:/ccache_data game_server_build
        deployDir="~/ansible/files/gamedata/${engineVersion}"
        copy_file
        ssh ${deployServer} "sed -i 's/[0-9]\+/${engineVersion}/g' ${deployDir}/engineVersion.json"
        tar_file
        scp  $WORKING_DIR/deploy/make_room.sh ${deployServer}:~/ansible
        ssh ${deployServer} "cd ~/ansible && chmod +x make_room.sh && ./make_room.sh"
    ;;
    test_1)
        docker run -it --rm -v /data/dev_1/blockmango-client:/data/code/blockmango -v /data/ccache:/ccache_data game_server_build
        deployDir="~/ansible/files/gamedata/${engineVersion}"
        copy_file
        ssh ${deployServer} "sed -i 's/[0-9]\+/${engineVersion}/g' ${deployDir}/engineVersion.json"
        tar_file
        scp  $WORKING_DIR/deploy/make_room.sh ${deployServer}:~/ansible
        ssh ${deployServer} "cd ~/ansible && chmod +x make_room.sh && ./make_room.sh"
    ;;
    test_2)
        docker run -it --rm -v /data/dev_2/blockmango-client:/data/code/blockmango -v /data/ccache:/ccache_data game_server_build
        deployDir="~/ansible/files/gamedata/${engineVersion}"
        copy_file
        ssh ${deployServer} "sed -i 's/[0-9]\+/${engineVersion}/g' ${deployDir}/engineVersion.json"
        tar_file
        scp  $WORKING_DIR/deploy/make_room.sh ${deployServer}:~/ansible
        ssh ${deployServer} "cd ~/ansible && chmod +x make_room.sh && ./make_room.sh"
    ;;
    test_3)
        docker run -it --rm -v /data/dev_3/blockmango-client:/data/code/blockmango -v /data/ccache:/ccache_data game_server_build
        deployDir="~/ansible/files/gamedata/${engineVersion}"
        copy_file
        ssh ${deployServer} "sed -i 's/[0-9]\+/${engineVersion}/g' ${deployDir}/engineVersion.json"
        tar_file
        scp  $WORKING_DIR/deploy/make_room.sh ${deployServer}:~/ansible
        ssh ${deployServer} "cd ~/ansible && chmod +x make_room.sh && ./make_room.sh"
    ;;
    test_4)
        docker run -it --rm -v /data/dev_4/blockmango-client:/data/code/blockmango -v /data/ccache:/ccache_data game_server_build
        deployDir="~/ansible/files/gamedata/${engineVersion}"
        copy_file
        ssh ${deployServer} "sed -i 's/[0-9]\+/${engineVersion}/g' ${deployDir}/engineVersion.json"
        tar_file
        scp  $WORKING_DIR/deploy/make_room.sh ${deployServer}:~/ansible
        ssh ${deployServer} "cd ~/ansible && chmod +x make_room.sh && ./make_room.sh"
    ;;
    test_5)
        docker run -it --rm -v /data/dev_5/blockmango-client:/data/code/blockmango -v /data/ccache:/ccache_data game_server_build
        deployDir="~/ansible/files/gamedata/${engineVersion}"
        copy_file
        ssh ${deployServer} "sed -i 's/[0-9]\+/${engineVersion}/g' ${deployDir}/engineVersion.json"
        tar_file
        scp  $WORKING_DIR/deploy/make_room.sh ${deployServer}:~/ansible
        ssh ${deployServer} "cd ~/ansible && chmod +x make_room.sh && ./make_room.sh"
    ;;
    test_102)
        docker run -it --rm -v /data/dev_102/blockmango-client:/data/code/blockmango -v /data/ccache:/ccache_data game_server_build
        deployDir="~/ansible/files/gamedata/${engineVersion}"
        copy_file
        ssh ${deployServer} "sed -i 's/[0-9]\+/${engineVersion}/g' ${deployDir}/engineVersion.json"
        tar_file
        scp  $WORKING_DIR/deploy/make_room.sh ${deployServer}:~/ansible
        ssh ${deployServer} "cd ~/ansible && chmod +x make_room.sh && ./make_room.sh"
    ;;    
    test_103)
        docker run -it --rm -v /data/dev_103/blockmango-client:/data/code/blockmango -v /data/ccache:/ccache_data game_server_build
        deployDir="~/ansible/files/gamedata/${engineVersion}"
        copy_file
        ssh ${deployServer} "sed -i 's/[0-9]\+/${engineVersion}/g' ${deployDir}/engineVersion.json"
        tar_file
        scp  $WORKING_DIR/deploy/make_room.sh ${deployServer}:~/ansible
        ssh ${deployServer} "cd ~/ansible && chmod +x make_room.sh && ./make_room.sh"
    ;;
    ios)
        docker run -it --rm -v /data/test/blockmango-client::/data/code/blockmango -v /data/ccache:/ccache_data game_server_build
        deployDir="/home/console/ansible/files/gamedata/${engineVersion}"
        copy_file
        ssh ${deployServer} "sed -i 's/true/false/g' ${deployDir}/engineVersion.json"
        ssh ${deployServer} "sed -i 's/debug/warning/g' ${deployDir}/engineVersion.json"
        tar_file
    ;;
    release_china)
        docker run -it --rm -v /data/test/blockmango-client:/data/code/blockmango -v /data/ccache:/ccache_data game_server_build
        deployDir="/home/console/ansible/files/gamedata/${engineVersion}"
        copy_file
        ssh ${deployServer} "sed -i 's/true/false/g' ${deployDir}/engineVersion.json"
        ssh ${deployServer} "sed -i 's/debug/warning/g' ${deployDir}/engineVersion.json"
        tar_file
    ;;
    release_seoul)
        docker run -i --rm -v /deploy/code/blockmango-client:/data/code/blockmango -v /home/console/ccache:/ccache_data game_server_build 16
        deployDir="/home/console/ansible/files/gamedata_stage/${engineVersion}"
        deployServer="console@10.0.1.182"
        copy_file
        ssh ${deployServer} "sed -i 's/true/false/g' ${deployDir}/engineVersion.json"
        #ssh ${deployServer} "sed -i 's/debug/warning/g' ${deployDir}/engineVersion.json"
        tar_file
    ;;
    *)
        echo -e "\033[31m[unknown environment target, something wrong happend, check it!!!!!!!]\033[0m"
        exit 1
    ;;
esac
echo -e "\033[32m[deploy to $1 succ]\033[0m" 
