#!/bin/bash
projectname=KingBeatKnight
schemename=$1
profile=$2
xcodebuildpath=/Applications/Xcode5.app/Contents/Developer/usr/bin/xcodebuild

#clean
echo 'clean target'
rm -rf ./archive/$schemename.xcarchive
$xcodebuildpath clean -project $projectname.xcodeproj -configuration Release -target $schemename

#archive
echo 'archive begin'
$xcodebuildpath archive -project $projectname.xcodeproj -scheme $schemename -archivePath ./archive/$schemename.xcarchive

#export
echo 'export'
rm -f ./build/${schemename}.ipa
$xcodebuildpath -exportArchive -archivePath ./archive/$schemename.xcarchive -exportPath ./build/$schemename -exportFormat ipa -exportProvisioningProfile $profile
