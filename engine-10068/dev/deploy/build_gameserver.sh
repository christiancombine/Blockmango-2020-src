#!/bin/bash
set -e


#tar package save place
targetDir="/deploy/code/res"
#source code path
sourceDir="/deploy/code/test/blockmango-client"
#user of destination server
defaultuser="console"

check_failed(){
    if [ $? -ne 0 ]
    then
         exit 1
    fi
}
switch_branch(){
    cd ${sourceDir}
    git checkout $branch
    check_failed
    git pull 
#    check_failed
    echo -e "\n"
    echo -e "\n"
    echo "######################CHANGES######################"
    echo "commit,   commitTime:     description,    committer,      tag"
    git log --pretty=format:"%h , %ar : %s, %cn, %d" --graph -10
    echo -e "\n"
	
	############## pull engine-core############
	echo "begin pull enginecore"
	cd ${sourceDir}/dev/deploy
	bash ./pullcore.sh
	if [ $? != 0 ]; then
		echo "error: pull engine core failed"
		exit 1
	fi
	echo "pull enginecore over"
}

copy_to(){
    serverhost="$1"
    serverpath=$2
    tarfile="$3"
    scp -oStrictHostKeyChecking=no ${tarfile} ${defaultuser}@${serverhost}:${serverpath}/${targetfile}
}

build(){
   branch="${1}"
   sourcecode="${2}"
   engineVer="${3}"
   switch_branch ${branch}
   
   if [ $? -ne 0 ]
   then
           echo "docker run failed" 
           exit 1
   fi
   if  [ -n "$engineVer" ] &&  [ "$branch" == "release_seoul" ] || [ "$branch" == "bugfix_release_seoul" ] || [ "$branch" == "release_china" ] || [ "$branch" == "hotfix_release_seoul" ];then 
       #production 
       # set useCmd false
       engineversionjson=`cat ${sourcecode}/res/client/Media/engineVersion.json|grep engineVersion|awk -F: '{print $NF}'|tr -d " ,"`
       if [ "$engineVer" != "$engineversionjson" ];then
          echo "error input invalid engine version $engineVer "
          exit 1
       fi
       sudo docker run -i --rm -v ${sourcecode}:/data/code/blockmango -v /deploy/ccache/test:/ccache_data game_server_build 8
       tarfile=`${sourceDir}/dev/deploy/copy_engine_files.sh ${sourcecode} ${targetDir} "old" ${engineVer} "true"`

   elif [ -n "$engineVer" ] && [ "$engineVer" -ge 90000 ]; then
       sudo docker run -i --rm -v ${sourcecode}:/data/code/blockmango -v /deploy/ccache/test:/ccache_data game_server_build 8
       tarfile=`${sourceDir}/dev/deploy/copy_engine_files.sh ${sourcecode} ${targetDir}  "old" ${engineVer}`
    	   #${engineVer}`
   elif  [ -n "$engineVer" ] && [ "$branch" == "engine2_release_seoul" ] ||  [ "$branch" == "hotfix_engine2_release_seoul" ] || [ "$branch" == "engine2_release_china" ];then
       #production 
       # set useCmd false
       engineversionjson=`cat ${sourcecode}/res/Media/engineVersion.json|grep engineVersion|awk -F: '{print $NF}'|tr -d " ,"` 
       if [ "$engineVer" != "$engineversionjson" ];then
          echo "error input invalid engine version $engineVer "
          exit 1
       fi

       sudo docker run -i --rm -v ${sourcecode}:/data/code/blockmango -v /deploy/ccache/test:/ccache_data game_server_build 8
       tarfile=`${sourceDir}/dev/deploy/copy_engine_files.sh ${sourcecode} ${targetDir} "new" ${engineVer} "true"`
   elif [ -n "$engineVer" ] && [ $engineVer -ge 80000 ] && [ $engineVer -lt 90000 ];then
       sudo docker run -i --rm -v ${sourcecode}:/data/code/blockmango -v /deploy/ccache/test:/ccache_data game_server_build 8
       tarfile=`${sourceDir}/dev/deploy/copy_engine_files.sh ${sourcecode} ${targetDir}  "new" ${engineVer}`
   else
      echo "engineversion or branch invalid !!"  
      echo "engineversion is $engineversion"  
      echo "branch is $branch"  
      exit 1
   fi
}

#sync_targz_to(){
#}

branch=""
destServer=""
rscripts=""
destPath=""
engineVersion=""
while getopts "b:d:r:p:v:" opt; do
   case $opt in
       b)
           echo "build branch is : ! $OPTARG" 
           branch=${OPTARG}
           ;;
       d)
           echo "destnation Server ip is: $OPTARG" 
           destServer="${OPTARG}"
           ;;
       r)
           echo "remote scripts : $OPTARG" 
           rscripts="${OPTARG}"
           ;;
       p)

           echo "destnation Server folder is: $OPTARG" 
           destPath="${OPTARG}"
           ;;
       v)
           engineVersion="${OPTARG}"
           ;;
       \?)
           echo "invalid option: -$OPTARG"
           echo "-b build branch"
           echo "-d  dest server ip"
           echo "-r  remote server scripts "
           echo "-v  specific engineversion  "
           echo "-p  save tar.gz file in  destnation server path "
           exit 1
           ;;

       esac
done

echo "build $branch $destServer $destPath $rscripts $engineVer $codesource"
if [ -z $branch  ];then
           echo "invalid option: "
           echo "-b build branch"
           echo "-d  dest server ip"
           echo "-r  remote server scripts "
           echo "-s  source code path "
	   exit 1
fi
cd ${sourceDir}/dev/deploy

if [ -n ${engineVersion} ]
then
   if [ $branch == "release_seoul" ];then
       engineVer="${engineVersion}"
   elif [ $branch == "release_china" ];then
       engineVer="${engineVersion}"
   elif [ $branch == "bugfix_release_seoul" ];then
       engineVer="${engineVersion}"
   elif [ $branch == "engine2_release_seoul" ];then
       engineVer="${engineVersion}"
    elif [ $branch == "hotfix_release_seoul" ];then
       engineVer="${engineVersion}"
    elif [ $branch == "hotfix_engine2_release_seoul" ];then
       engineVer="${engineVersion}"
   elif [ $branch == "engine2_release_china" ];then
       engineVer="${engineVersion}"
   else
       #if specific engineversion and branch is not belongs production 
       #count  engineversion
       engineVer=`python ./chk_branch.py -b ${branch}`
      
   fi
else
   #count  dev/test engineversion
   engineVer=`./chk_branch.py -b ${branch}`
fi


#################### MAIN HERE 	#########################

build ${branch} ${sourceDir} ${engineVer}
if [ -n "$destServer" ];then
   if [ -n "$destPath" ];then
       copy_to ${destServer} ${destPath} ${tarfile}
   else
       echo "error destination path is empty"
   fi
   echo ${engineVer}.tar.gz
   if [ -n "$rscripts" ];then
      ssh  -oStrictHostKeyChecking=no  ${defaultuser}@${destServer} ${rscripts} ${engineVer}
   fi
fi
#################### END  #########################
