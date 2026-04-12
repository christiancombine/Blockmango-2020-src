#!/bin/bash

mkdir -p stackdump
mkdir -p scriptexception

#stack dump log
cd log 
ls | xargs -n 100 grep "Received fatal signal"  | grep -v "SIGTERM" |awk -F ":\\\(" '{print $1}' > ../stackdump/stackdump.log
cd ..
stackdumplogs=$(cat stackdump/stackdump.log)
for log in ${stackdumplogs[@]}  
do  
    cp log/${log} stackdump/
done
rm stackdump/stackdump.log

#script exception
cd log
ls | xargs -n 100 grep "SCRIPT_EXCEPTION" | awk -F ":\\\(" '{print $1}' > ../scriptexception/scriptexception.log
cd ..
scriptexceptiongs=$(cat scriptexception/scriptexception.log)
for log in ${scriptexceptiongs[@]}  
do  
    cp log/${log} scriptexception/
done 
rm scriptexception/scriptexception.log

#rm all log
rm log/*.log

#rm sendlogin stackdump log
#grep "ServerPacketSender::sendUserIn" *.log | awk -F ":\t" '{print $1}' | xargs rm
#rm consumecoin stackdump log
#grep "BLOCKMAN::Wallet::consumeCoin" *.log | awk -F ":\t" '{print $1}' | xargs rm