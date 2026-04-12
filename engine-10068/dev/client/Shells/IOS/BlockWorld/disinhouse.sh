#!/bin/bash
projectname=BlockWorld
schemename=BlockMan

#clean
echo 'clean target BlockManInhouse'
rm -rf ./archive/$schemename.xcarchive
xcodebuild clean -project $projectname.xcodeproj -configuration Release -target BlockManInhouse

#archive
echo 'archive begin'
xcodebuild archive -project $projectname.xcodeproj -scheme $schemename -archivePath ./archive/$schemename.xcarchive

#export
echo 'export'
rm -f ./build/${schemename}.ipa
xcodebuild -exportArchive -archivePath ./archive/$schemename.xcarchive -exportPath ./build/$schemename -exportFormat ipa -exportProvisioningProfile 'BlockMan Distribution'
