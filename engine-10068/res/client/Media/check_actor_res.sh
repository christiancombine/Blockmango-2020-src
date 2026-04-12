#!/bin/bash

find . -name *.actor | xargs grep -n -E "FileName.*.mesh" | awk -F'"' '{print $2}' > mesh_file_list.txt
echo "unknown_mesh" >> mesh_file_list.txt
for line in `cat mesh_file_list.txt` 
do
    output=`find . -name $line`
    if [[ "$output"x == ""x ]] ; then
        echo "$line miss"
    fi

done
rm mesh_file_list.txt

find . -name *.actor | xargs grep -n -E "MeshName.*.skin" | awk -F'"' '{print $2}' > skin_file_list.txt
 echo "unknown_skin" >> skin_file_list.txt
for line in `cat skin_file_list.txt` 
do
    output=`find . -name $line`
    if [[ "$output"x == ""x ]] ; then
        echo "$line miss"
    fi

done
rm skin_file_list.txt

find . -name *.actor | xargs grep -n -E "Animation.*.anim" | awk -F'"' '{print $2}' > anim_file_list.txt
echo "unknown_anim" >> anim_file_list.txt 
for line in `cat anim_file_list.txt` 
do
    output=`find . -name $line`
    if [[ "$output"x == ""x ]] ; then
        echo "$line miss"
    fi

done
rm anim_file_list.txt

