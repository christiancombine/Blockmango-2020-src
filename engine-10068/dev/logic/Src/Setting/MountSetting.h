#pragma once
#include "Core.h"

namespace BLOCKMAN
{
	class MountSetting
	{
	public:
		int id = 0;
		LORD::Vector3 size = { 2.f, 1.3f, 2.f };
		float sensitivity = 0.01f;
		float turnSpeed = 0.03f;
		float straightenSpeed = 0.06f;
		float moveSpeed = 0.6f;
		float waitStraightenTime = 0.5f;
		LORD::Vector3 cameraOffset = { 0, 6, -6 };
		LORD::Vector3 cameraRotCenterOffset = { 0, 0, 0 };
		LORD::Vector3 riderRenderOffset = { 0, 0.5, 0 };
		float yOffset = 0.f;
		LORD::Quaternion riderRenderOrientation = { 0.5, 0.5, 0.5, 0.5 };
		LORD::String modelName = "long.actor";
		LORD::String mountBone = "Bip001_Head";
		float headTextOffset = 4;
		float maxCameraBodyAngle = 60 * LORD::Math::DEG2RAD;
		float scale = 1;
		LORD::String rideAnimation = "ride_dragon";
		bool penetrateBlocks = false;
		int spawnSoundId = 0;
		int deadSoundId = 0;
		int takingSoundId = 0;

	private:
		static LORD::map<int, MountSetting>::type mountSettings;

	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();
		static const MountSetting* getSetting(int gunId);
		static void addSetting(MountSetting setting);
	};
}