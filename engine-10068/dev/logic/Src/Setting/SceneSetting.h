#pragma once

#include "BM_TypeDef.h"
#include "Network/protocol/PacketDefine.h"

namespace BLOCKMAN
{
	class SkyInfo
	{
	public:
		int skyboxId = 0;
		String skyboxName = "";
		float startFadeIn = 0.0f;
		float endFadeIn = 0.0f;
		float startFadeOut = 0.0f;
		float endFadeOut = 0.0f;
		int blendMode = 0;
		float rotateAngle = 0.0f;
		float rotateAxisX = 0.0f;
		float rotateAxisY = 0.0f;
		float rotateAxisZ = 0.0f;
		bool isTwinkle = false;
		float twinkleBrightMax = 1.0f;
		float twinkleDarkMax = 1.0f;
		float twinkleAlphaMin = 1.0f;
		bool operator<(const SkyInfo& rhs) const { return (*this).skyboxId < rhs.skyboxId; };
		bool operator>(const SkyInfo& rhs) const { return (*this).skyboxId > rhs.skyboxId; };
		bool operator==(const SkyInfo& rhs) const { return (*this).skyboxId == rhs.skyboxId; }
		DEFINE_NETWORK_SERIALIZER(skyboxId, skyboxName, startFadeIn, endFadeIn, startFadeOut, endFadeOut, blendMode, rotateAngle, rotateAxisX, rotateAxisY, rotateAxisZ, isTwinkle, twinkleBrightMax, twinkleDarkMax, twinkleAlphaMin);
	};

	class SceneSetting : public Singleton<SceneSetting>, public ObjectAlloc
	{
	public:
		enum RenderSkyType
		{
			RST_NONE = 0,
			RST_SKY_2D = 1,
			RST_SKY_CUBE = 2,
		};

		enum RenderCloudType
		{
			RCT_NONE = 0,
			RCT_CLOUD_MODEL = 1,
			RCT_CLOUD_EFFECT = 2,
		};

	public:
		SceneSetting();
		~SceneSetting();

		bool loadSetting(bool bIsClient);
		void unloadSetting();

		void addSkyInfo(SkyInfo &sI);
		void setSkyInfoList(vector<SkyInfo>::type skyInfoList);
		vector<SkyInfo>::type getSkyInfoList();
		bool needDisplaySkybox(float currTime, int id, float& alpha);
		bool needDisplayMeteor(float currTime);
		bool needDisplayStar(float currTime, int& starNum);

		void setSunAndMoonName(const String& sunName, const String& moonName) { m_sunName = sunName; m_moonName = moonName; }
		String getSunName() { return m_sunName; }
		String getMoonName() { return m_moonName; }
		String getMeteorName() { return m_meteorName; }
		String getStarName() { return m_starName; }
		String getCloud2DName() { return m_cloud2DName; }

		/** Set scene param before using this func */
		bool changeSceneSwitch(RenderSkyType skyType, RenderCloudType cloudType, bool showSunAndMoon, bool showMeteor, bool showStarEffect);
		RenderSkyType getSkyType() { return m_skyType; }
		RenderCloudType getCloudType() { return m_cloudType; }
		bool showSunAndNoon() { return m_showSunAndMoon; }
		bool showMeteor() { return m_showMeteor; }
		bool showStarEffect() { return m_showStarEffect; }

		bool useCustomBackgroundColor() { return m_useCustomBackgroundColor; }
		bool getBackgroundColor(Color& color, int hour, int minute);
		void resetStarRender() { m_starRenderNum = 0; }

	private:
		vector<SkyInfo>::type m_skyInfoList;
		String m_sunName = "sun.png";
		String m_moonName = "moon_phases.png";
		String m_meteorName = "meteor.png";
		String m_starName = "star.png";
		String m_cloud2DName = "cloud_2D.png";

		// switch
		RenderSkyType m_skyType = RST_SKY_2D;
		RenderCloudType m_cloudType = RCT_CLOUD_MODEL;
		bool m_showSunAndMoon = true;
		bool m_showMeteor = false;
		bool m_showStarEffect = false;

		bool m_useCustomBackgroundColor = false;
		map<int, Color>::type m_customBackgroundColor{
			{0, Color(0.000f, 0.000f, 0.000f)},
			{1, Color(0.000f, 0.000f, 0.000f)},
			{2, Color(0.000f, 0.000f, 0.000f)},
			{3, Color(0.000f, 0.000f, 0.000f)},
			{4, Color(0.000f, 0.000f, 0.000f)},
			{5, Color(0.000f, 0.000f, 0.000f)},
			{6, Color(0.131f, 0.145f, 0.224f)},
			{7, Color(0.327f, 0.392f, 0.509f)},
			{8, Color(0.264f, 0.346f, 0.705f)},
			{9, Color(0.200f, 0.300f, 0.900f)},
			{10, Color(0.200f, 0.300f, 0.900f)},
			{11, Color(0.200f, 0.300f, 0.900f)},
			{12, Color(0.200f, 0.300f, 0.900f)},
			{13, Color(0.200f, 0.300f, 0.900f)},
			{14, Color(0.200f, 0.300f, 0.900f)},
			{15, Color(0.200f, 0.300f, 0.900f)},
			{16, Color(0.200f, 0.300f, 0.900f)},
			{17, Color(0.200f, 0.300f, 0.900f)},
			{18, Color(0.200f, 0.300f, 0.900f)},
			{19, Color(0.200f, 0.300f, 0.900f)},
			{20, Color(0.478f, 0.336f, 0.616f)},
			{21, Color(0.715f, 0.372f, 0.332f)},
			{22, Color(0.000f, 0.000f, 0.000f)},
			{23, Color(0.000f, 0.000f, 0.000f)}
		};

		float m_meteorStartTime = 21.5f;
		float m_meteorEndTime = 6.0f;

		float m_starStartFadeIn = 20.5f;
		float m_starEndFadeIn = 21.5f;
		float m_starStartFadeOut = 5.5f;
		float m_starEndFadeOut = 6.5f;
		int m_starTotalNum = 2000;
		int m_starRenderNum = 0;
	};
}