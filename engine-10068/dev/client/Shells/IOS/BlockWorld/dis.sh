#!/bin/bash
projectname=BlockWorld
schemename=BlockMan

#clean
echo 'clean all targets'
rm -rf ./archive/$schemename.xcarchive
xcodebuild clean -project $projectname.xcodeproj -configuration Release -alltargets

#archive
echo 'archive begin'
xcodebuild archive -project $projectname.xcodeproj -scheme $schemename -archivePath ./archive/$schemename.xcarchive

#export
echo 'export'
rm -f ./build/${schemename}.ipa
xcodebuild -exportArchive -archivePath ./archive/$schemename.xcarchive -exportPath ./build/$schemename -exportFormat ipa -exportProvisioningProfile 'BlockWorldDis'
