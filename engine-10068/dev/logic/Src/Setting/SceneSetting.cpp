#include "SceneSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	SceneSetting::SceneSetting()
	{

	}

	SceneSetting::~SceneSetting()
	{

	}

	bool SceneSetting::loadSetting(bool bIsClient)
	{
		CsvReader* pReader = new CsvReader();

		std::string path;
		if (bIsClient)
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "SceneSetting.csv").c_str();
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "SceneSetting.csv").c_str();
		}

		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogInfo("Can not open Sky setting file %s", path.c_str());
			LordSafeDelete(pReader);
			return false;
		}

		m_skyInfoList.clear();

		pReader->Read();// discard the second line
		while (pReader->Read())
		{
			String sceneType;
			pReader->GetString("Type", sceneType);
			int sceneSwitch;
			pReader->GetInt("Switch", sceneSwitch);

			if (sceneType == "Sky")
			{
				m_skyType = (RenderSkyType)sceneSwitch;
				if (sceneSwitch == 2)
				{
					SkyInfo skyInfo;
					pReader->GetString("Name", skyInfo.skyboxName);
					pReader->GetInt("GroupId", skyInfo.skyboxId);
					pReader->GetFloat("StartIn", skyInfo.startFadeIn);
					pReader->GetFloat("EndIn", skyInfo.endFadeIn);
					pReader->GetFloat("StartOut", skyInfo.startFadeOut);
					pReader->GetFloat("EndOut", skyInfo.endFadeOut);
					pReader->GetInt("Mode", skyInfo.blendMode);
					pReader->GetFloat("RotateAngle", skyInfo.rotateAngle);
					pReader->GetFloat("AxisX", skyInfo.rotateAxisX);
					pReader->GetFloat("AxisY", skyInfo.rotateAxisY);
					pReader->GetFloat("AxisZ", skyInfo.rotateAxisZ);
					pReader->GetBool("isTwinkle", skyInfo.isTwinkle);
					pReader->GetFloat("twinkleBrightMax", skyInfo.twinkleBrightMax);
					pReader->GetFloat("twinkleDarkMax", skyInfo.twinkleDarkMax);
					pReader->GetFloat("twinkleAlphaMin", skyInfo.twinkleAlphaMin);
					m_skyInfoList.push_back(skyInfo);
				}
			}
			else  if (sceneType == "Cloud")
			{
				m_cloudType = (RenderCloudType)sceneSwitch;
				if (sceneSwitch == 2)
				{
					pReader->GetString("Name", m_cloud2DName);
				}
			}
			else if (sceneType == "Sun")
			{
				m_showSunAndMoon = sceneSwitch;
				pReader->GetString("Name", m_sunName);
			}
			else if (sceneType == "Moon")
			{
				m_showSunAndMoon = sceneSwitch;
				pReader->GetString("Name", m_moonName);
			}
			else if (sceneType == "Meteor")
			{
				m_showMeteor = sceneSwitch;
				pReader->GetString("Name", m_meteorName);
				pReader->GetFloat("StartIn", m_meteorStartTime);
				pReader->GetFloat("StartOut", m_meteorEndTime);
			}
			else if (sceneType == "Star")
			{
				m_showStarEffect = sceneSwitch;
				pReader->GetString("Name", m_starName);
				pReader->GetInt("GroupId", m_starTotalNum);
				pReader->GetFloat("StartIn", m_starStartFadeIn);
				pReader->GetFloat("EndIn", m_starEndFadeIn);
				pReader->GetFloat("StartOut", m_starStartFadeOut);
				pReader->GetFloat("EndOut", m_starEndFadeOut);
			}
		}
		LordSafeDelete(pReader);
		return true;
	}

	void SceneSetting::unloadSetting()
	{
		m_skyInfoList.clear();
	}

	void SceneSetting::addSkyInfo(SkyInfo &sI)
	{
		SkyInfo skyInfo;
		skyInfo.skyboxId = sI.skyboxId;
		skyInfo.skyboxName = sI.skyboxName;
		skyInfo.startFadeIn = sI.startFadeIn;
		skyInfo.endFadeIn = sI.endFadeIn;
		skyInfo.startFadeOut = sI.startFadeOut;
		skyInfo.endFadeOut = sI.endFadeOut;
		skyInfo.blendMode = sI.blendMode;
		skyInfo.rotateAngle = sI.rotateAngle;
		skyInfo.rotateAxisX = sI.rotateAxisX;
		skyInfo.rotateAxisY = sI.rotateAxisY;
		skyInfo.rotateAxisZ = sI.rotateAxisZ;
		skyInfo.isTwinkle = sI.isTwinkle;
		skyInfo.twinkleBrightMax = sI.twinkleBrightMax;
		skyInfo.twinkleDarkMax = sI.twinkleDarkMax;
		skyInfo.twinkleAlphaMin = sI.twinkleAlphaMin;
		m_skyInfoList.push_back(skyInfo);
	}

	void SceneSetting::setSkyInfoList(vector<SkyInfo>::type skyInfoList)
	{
		m_skyInfoList = skyInfoList;
	}

	vector<SkyInfo>::type SceneSetting::getSkyInfoList()
	{
		return m_skyInfoList;
	}

	bool SceneSetting::needDisplaySkybox(float currTime, int id, float& alpha)
	{
		alpha = 1.0f;
		for (auto iter : m_skyInfoList)
		{
			if (iter.skyboxId == id)
			{
				if ((iter.startFadeIn < iter.endFadeOut && currTime >= iter.startFadeIn && currTime <= iter.endFadeOut)
					|| (iter.startFadeIn > iter.endFadeOut && (currTime >= iter.startFadeIn || currTime <= iter.endFadeOut)))
				{
					float alphaFadeIn = -1.0f;
					float alphaFadeOut = -1.0f;

					float fadeInTotalTime = iter.endFadeIn - iter.startFadeIn;
					if (fadeInTotalTime < 0.0f)
					{
						fadeInTotalTime += 24.0f;
					}
					float fadeInDeltaTime = currTime - iter.startFadeIn;
					if (fadeInDeltaTime < 0.0f)
					{
						fadeInDeltaTime += 24.0f;
					}
					if (Math::Abs(fadeInTotalTime) <= 0.00001f)
					{
						alphaFadeIn = 1.0f;
					}
					else
					{
						alphaFadeIn = Math::Clamp(fadeInDeltaTime / fadeInTotalTime, 0.0f, 1.0f);
					}

					float fadeOutTotalTime = iter.endFadeOut - iter.startFadeOut;
					if (fadeOutTotalTime < 0.0f)
					{
						fadeOutTotalTime += 24.0f;
					}
					float fadeOutDeltaTime = iter.endFadeOut - currTime;
					if (fadeOutDeltaTime < 0.0f)
					{
						fadeOutDeltaTime += 24.0f;
					}
					if (Math::Abs(fadeOutTotalTime) <= 0.00001f)
					{
						alphaFadeOut = 1.0f;
					}
					else
					{
						alphaFadeOut = Math::Clamp(fadeOutDeltaTime / fadeOutTotalTime, 0.0f, 1.0f);
					}

					alpha = Math::Min(alphaFadeIn, alphaFadeOut);
					return true;
				}
			}
		}
		return false;
	}

	bool SceneSetting::needDisplayMeteor(float currTime)
	{
		if ((m_meteorStartTime < m_meteorEndTime && currTime >= m_meteorStartTime && currTime <= m_meteorEndTime)
			|| (m_meteorStartTime > m_meteorEndTime && (currTime >= m_meteorStartTime || currTime <= m_meteorEndTime)))
		{
			return true;
		}
		return false;
	}

	bool SceneSetting::needDisplayStar(float currTime, int& starNum)
	{
		float percent = 1.0f;
		if ((m_starStartFadeIn < m_starEndFadeOut && currTime >= m_starStartFadeIn && currTime <= m_starEndFadeOut)
			|| (m_starStartFadeIn > m_starEndFadeOut && (currTime >= m_starStartFadeIn || currTime <= m_starEndFadeOut)))
		{
			float percentFadeIn = -1.0f;
			float percentFadeOut = -1.0f;

			float fadeInTotalTime = m_starEndFadeIn - m_starStartFadeIn;
			if (fadeInTotalTime < 0.0f)
			{
				fadeInTotalTime += 24.0f;
			}
			float fadeInDeltaTime = currTime - m_starStartFadeIn;
			if (fadeInDeltaTime < 0.0f)
			{
				fadeInDeltaTime += 24.0f;
			}
			if (Math::Abs(fadeInTotalTime) <= 0.00001f)
			{
				percentFadeIn = 1.0f;
			}
			else
			{
				percentFadeIn = Math::Clamp(fadeInDeltaTime / fadeInTotalTime, 0.0f, 1.0f);
			}

			float fadeOutTotalTime = m_starEndFadeOut - m_starStartFadeOut;
			if (fadeOutTotalTime < 0.0f)
			{
				fadeOutTotalTime += 24.0f;
			}
			float fadeOutDeltaTime = m_starEndFadeOut - currTime;
			if (fadeOutDeltaTime < 0.0f)
			{
				fadeOutDeltaTime += 24.0f;
			}
			if (Math::Abs(fadeOutTotalTime) <= 0.00001f)
			{
				percentFadeOut = 1.0f;
			}
			else
			{
				percentFadeOut = Math::Clamp(fadeOutDeltaTime / fadeOutTotalTime, 0.0f, 1.0f);
			}

			percent = Math::Min(percentFadeIn, percentFadeOut);
			int currStarNum = (int)(percent * m_starTotalNum);
			if (m_starRenderNum == currStarNum)
			{
				starNum = 0;
			}
			else
			{
				starNum = currStarNum - m_starRenderNum;
				m_starRenderNum = currStarNum;
			}
			return true;
		}
		m_starRenderNum = 0;
		return false;
	}

	bool SceneSetting::changeSceneSwitch(RenderSkyType skyType, RenderCloudType cloudType, bool showSunAndMoon, bool showMeteor, bool showStarEffect)
	{
		if (m_skyInfoList.size() < 1 && skyType == RST_SKY_CUBE)
		{
			skyType = RST_SKY_2D;
		}
		m_useCustomBackgroundColor = (skyType == RST_SKY_CUBE);
		if (m_skyType != skyType || m_cloudType != cloudType || m_showSunAndMoon != showSunAndMoon || m_showMeteor != showMeteor || m_showStarEffect != showStarEffect)
		{
			m_skyType = skyType;
			m_cloudType = cloudType;
			m_showSunAndMoon = showSunAndMoon;
			m_showMeteor = showMeteor;
			m_showStarEffect = showStarEffect;
			return true;
		}
		return false;
	}

	bool SceneSetting::getBackgroundColor(Color& color, int hour, int minute)
	{
		if (hour < 0 || hour >= m_customBackgroundColor.size())
			return false;

		Color startColor = m_customBackgroundColor[hour];
		Color endColor = m_customBackgroundColor[hour == 23 ? 0 : hour + 1];
		color = (endColor - startColor) * (float)minute / 60.0f + startColor;
		return true;
	}

}