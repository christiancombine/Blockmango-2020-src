#!/bin/bash
projectname=BlockWorld
schemename=$1
profile=$2

#clean
echo 'clean target'
rm -rf ./archive/$schemename.xcarchive
xcodebuild clean -project $projectname.xcodeproj -configuration Release -target $schemename

#archive
echo 'archive begin'
xcodebuild archive -project $projectname.xcodeproj -scheme $schemename -archivePath ./archive/$schemename.xcarchive

#export
echo 'export'
rm -f ./build/${schemename}.ipa
xcodebuild -exportArchive -archivePath ./archive/$schemename.xcarchive -exportPath ./build/$schemename -exportFormat ipa -exportProvisioningProfile $profile
