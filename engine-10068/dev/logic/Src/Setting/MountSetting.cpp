#include "MountSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

using namespace LORD;

namespace BLOCKMAN
{
	LORD::map<int, MountSetting>::type MountSetting::mountSettings;

	bool MountSetting::loadSetting(bool bIsClient)
	{
		mountSettings.clear();
		CsvReader reader;

		std::string path;
		if (bIsClient)
		{
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/Mount.csv")).c_str();
		}
		else
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/Mount.csv")).c_str();
#else
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Mount.csv")).c_str();
#endif
		}

		if (!reader.LoadFile(path))
		{
			LordLogError("Can not open gun setting file %s", path.c_str());
			return false;
		}

		reader.Read(); // discard the second line
		while (reader.Read())
		{
			int id = 0;
			if (!reader.GetInt("Id", id))
				continue;
			auto&& setting = mountSettings[id];
			setting.id = id;

			reader.GetFloat("SizeX", setting.size.x, 2.f);
			reader.GetFloat("SizeY", setting.size.y, 1.3f);
			reader.GetFloat("SizeZ", setting.size.z, 2.f);
			reader.GetFloat("Sensitivity", setting.sensitivity, 0.01f);
			reader.GetFloat("TurnSpeed", setting.turnSpeed, 0.03f);
			reader.GetFloat("StraightenSpeed", setting.straightenSpeed, 0.06f);
			reader.GetFloat("MoveSpeed", setting.moveSpeed, 0.6f);
			reader.GetFloat("WaitStraightenTime", setting.waitStraightenTime, 0.5f);
			reader.GetFloat("CameraOffsetX", setting.cameraOffset.x, 0.f);
			reader.GetFloat("CameraOffsetY", setting.cameraOffset.y, 4.f);
			reader.GetFloat("CameraOffsetZ", setting.cameraOffset.z, -9.f);
			reader.GetFloat("CameraRotCenterOffsetX", setting.cameraRotCenterOffset.x, 0.f);
			reader.GetFloat("CameraRotCenterOffsetY", setting.cameraRotCenterOffset.y, 0.f);
			reader.GetFloat("CameraRotCenterOffsetZ", setting.cameraRotCenterOffset.z, 0.f);
			reader.GetFloat("RiderRenderOffsetX", setting.riderRenderOffset.x, 0.f);
			reader.GetFloat("RiderRenderOffsetY", setting.riderRenderOffset.y, 0.5f);
			reader.GetFloat("RiderRenderOffsetZ", setting.riderRenderOffset.z, 0.f);
			reader.GetFloat("RiderRenderOrientationW", setting.riderRenderOrientation.w, 0.5f);
			reader.GetFloat("RiderRenderOrientationX", setting.riderRenderOrientation.x, 0.5f);
			reader.GetFloat("RiderRenderOrientationY", setting.riderRenderOrientation.y, 0.5f);
			reader.GetFloat("RiderRenderOrientationZ", setting.riderRenderOrientation.z, 0.5f);
			reader.GetFloat("Yoffset", setting.yOffset, 0.f);
			setting.modelName = "long.actor";
			reader.GetString("ModelName", setting.modelName);
			setting.mountBone = "Bip001_Head";
			reader.GetString("MountBone", setting.mountBone);
			reader.GetFloat("HeadTextOffset", setting.headTextOffset, 4.f);
			reader.GetFloat("MaxCameraBodyAngle", setting.maxCameraBodyAngle, 60.f);
			setting.maxCameraBodyAngle *= LORD::Math::DEG2RAD;
			reader.GetFloat("Scale", setting.scale, 1.f);
			setting.rideAnimation = "ride_dragon";
			reader.GetString("RideAnimation", setting.rideAnimation);
			reader.GetBool("PenetrateBlocks", setting.penetrateBlocks, false);
			reader.GetInt("SpawnSoundId", setting.spawnSoundId);
			reader.GetInt("DeadSoundId", setting.deadSoundId);
			reader.GetInt("TakingSoundId", setting.takingSoundId);
		}
		return true;
	}

	void MountSetting::unloadSetting()
	{
		mountSettings.clear();
	}

	const MountSetting* MountSetting::getSetting(int id)
	{
		auto it = mountSettings.find(id);
		if (it == mountSettings.end())
		{
			return nullptr;
		}
		return &it->second;
	}

		void MountSetting::addSetting(MountSetting setting)
    	{
    		auto it = mountSettings.find(setting.id);
    		if (it != mountSettings.end())
    		{
    			mountSettings.erase(setting.id);
    		}
    		mountSettings.insert({ setting.id , setting });
    	}

}
