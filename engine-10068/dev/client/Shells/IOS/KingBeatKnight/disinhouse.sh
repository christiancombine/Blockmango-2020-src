#!/bin/bash
projectname=KingBeatKnight
schemename=dotaHolyshitInhouse

#clean
echo 'clean target dotaHolyshitInhouse'
rm -rf ./archive/$schemename.xcarchive
xcodebuild clean -project $projectname.xcodeproj -configuration Release -target dotaHolyshitInhouse

#archive
echo 'archive begin'
xcodebuild archive -project $projectname.xcodeproj -scheme $schemename -archivePath ./archive/$schemename.xcarchive

#export
echo 'export'
rm -f ./build/${schemename}.ipa
xcodebuild -exportArchive -archivePath ./archive/$schemename.xcarchive -exportPath ./build/$schemename -exportFormat ipa -exportProvisioningProfile 'shanqiuzousiqi Distribution'
