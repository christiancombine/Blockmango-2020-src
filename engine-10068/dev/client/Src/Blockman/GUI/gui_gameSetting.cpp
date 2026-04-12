#include "gui_gameSetting.h"
#include "Util/LanguageKey.h"
#include <functional>
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "Audio/SoundSystem.h"
#include "Core.h"
#include "Util/PathUtil.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "Object/Root.h"
#include "UI/GUIStaticText.h"
#include "game.h"

using namespace LORD;

namespace BLOCKMAN {

	gui_gameSetting::gui_gameSetting()
		: gui_layout("GameSetting.json")
	{

	}

	gui_gameSetting::~gui_gameSetting()
	{
	}

	void gui_gameSetting::onLoad()
	{
		getWindow("GameSetting-Content-Function-Luminance-Name")->SetText(getString(LanguageKey::GUI_SETTING_LUMINANCE));
		getWindow("GameSetting-Content-Function-Horizon-Name")->SetText(getString(LanguageKey::GUI_SETTING_HORIZON));
		getWindow("GameSetting-Content-Function-Volume-Name")->SetText(getString(LanguageKey::GUI_SETTING_VOLUME));
		getWindow("GameSetting-Content-Function-Gui-Size-Name")->SetText(getString(LanguageKey::GUI_SETTING_GUI_SIZE));
		getWindow("GameSetting-Content-Function-Camera-Sensitive-Name")->SetText(getString(LanguageKey::GUI_SETTING_CAMERA_SENSITIVE));
		getWindow("GameSetting-Content-Function-BackgroundMusic-Name")->SetText(getString(LanguageKey::GUI_GAME_SETTING_BACKGROUND_MUSIC));
		
		GUIStaticText* fps = getWindow<GUIStaticText>("GameSetting-Content-Function-FPS-Name");
		fps->SetText(getString(LanguageKey::GUI_SETTING_FPS));

		GUIStaticText* poleControl = getWindow<GUIStaticText>("GameSetting-Content-Function-Toggle-Pole-Name");
		poleControl->SetText(getString(LanguageKey::GUI_SETTING_POLE_STATE_TOGGLE));

		m_sliderLuminance = getWindow<GUISlider>("GameSetting-Content-Function-Luminance-Value");
		m_sliderHorizon = getWindow<GUISlider>("GameSetting-Content-Function-Horizon-Value");
		m_sliderVolume = getWindow<GUISlider>("GameSetting-Content-Function-Volume-Value");
		m_sliderGuiSize = getWindow<GUISlider>("GameSetting-Content-Function-Gui-Size-Value");
		m_sliderCameraSensitive = getWindow<GUISlider>("GameSetting-Content-Function-Camera-Sensitive-Value");
		m_sliderFPS = getWindow<GUISlider>("GameSetting-Content-Function-FPS-Value");
		m_checkboxPoleControlState = getWindow<GUICheckBox>("GameSetting-Content-Function-Toggle-Pole-State");
		m_checkBoxBackgroundMusic = getWindow<GUICheckBox>("GameSetting-Content-Function-BackgroundMusic-State");

		m_sliderFPS->subscribeEvent(EventWindowTouchDown, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::FPS));

		m_sliderLuminance->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::LUMINANCE));
		m_sliderHorizon->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::HORIZON));
		m_sliderVolume->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::VOLUME));
		m_sliderGuiSize->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::GUI_SIZE));
		m_sliderCameraSensitive->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::CAMERA_SENSITIVE));
		m_sliderFPS->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::FPS));

		m_sliderLuminance->subscribeEvent(EventMotionRelease, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::LUMINANCE));
		m_sliderHorizon->subscribeEvent(EventMotionRelease, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::HORIZON));
		m_sliderVolume->subscribeEvent(EventMotionRelease, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::VOLUME));
		m_sliderGuiSize->subscribeEvent(EventMotionRelease, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::GUI_SIZE));
		m_sliderCameraSensitive->subscribeEvent(EventMotionRelease, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::CAMERA_SENSITIVE));
		m_sliderFPS->subscribeEvent(EventMotionRelease, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::FPS));

		m_sliderVolume->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::VOLUME));
		m_sliderHorizon->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::HORIZON));
		m_sliderLuminance->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::LUMINANCE));
		m_sliderGuiSize->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::GUI_SIZE));
		m_sliderCameraSensitive->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::CAMERA_SENSITIVE));
		m_sliderFPS->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::FPS));

		m_checkboxPoleControlState->subscribeEvent(EventCheckStateChanged, std::bind(&gui_gameSetting::onCheckBoxStateChange, this, std::placeholders::_1, LayoutId::POLE_STATE));
		m_checkBoxBackgroundMusic->subscribeEvent(EventCheckStateChanged, std::bind(&gui_gameSetting::onCheckBoxStateChange, this, std::placeholders::_1, LayoutId::BACKGROUND_MUSIC));
		
		loadSettingConfig();
	}

	void gui_gameSetting::setVolume()
	{
		if (m_sliderVolume)
		{
			float bgVolume = SoundSystem::Instance()->getEffectsVolume();
			m_sliderVolume->SetProgress(bgVolume);
		}
	}

	void gui_gameSetting::setLuminance()
	{
		if (m_sliderLuminance)
		{
			float gammaVolue = Blockman::Instance()->m_gameSettings->gammaSetting;
			m_sliderLuminance->SetProgress(gammaVolue);
		}
	}

	void gui_gameSetting::setHorizon()
	{
		if (m_sliderHorizon)
		{
			float fovSetting = Blockman::Instance()->m_gameSettings->getFovSetting();
			m_sliderHorizon->SetProgress(fovSetting);
		}
	}

	void gui_gameSetting::setGuiSize()
	{
		if (m_sliderGuiSize) 
		{
			float guiSize = (float)Blockman::Instance()->m_gameSettings->playerActivityGuiSize;
			guiSize = Math::Min(guiSize, GUI_MAX_SIZE);
			guiSize = Math::Max(guiSize, GUI_MIN_SIZE);
			float progress = (guiSize - GUI_MIN_SIZE) / (GUI_MAX_SIZE - GUI_MIN_SIZE);
			m_sliderGuiSize->SetProgress(progress);
		}
	}

	void gui_gameSetting::setCameraSensitive()
	{
		if (m_sliderCameraSensitive)
		{
			float cameraSensitive = Blockman::Instance()->m_gameSettings->getCameraSensitive();
			float minSize = 1.0F;
			float maxSize = 9.0F;
			float progress = (cameraSensitive - minSize) / (maxSize - minSize);
			m_sliderCameraSensitive->SetProgress(progress);
		}
	}

	void gui_gameSetting::setFPS()
	{
		if (m_sliderFPS)
		{
			float fps = Blockman::Instance()->m_gameSettings->m_fpsSetting;
			m_sliderFPS->SetProgress(fps);
		}
	}

	void gui_gameSetting::setPoleControlState()
	{
		if (m_checkboxPoleControlState)
		{
			bool usePole = Blockman::Instance()->m_gameSettings->usePole > 0;
			m_checkboxPoleControlState->SetCheckedNoEvent(usePole);
		}
	}

	void gui_gameSetting::setBackgroundMusicSwitc()
	{
		if (m_checkBoxBackgroundMusic)
		{
			bool isOn = SoundSystem::Instance()->isSwitchOnBackgroundMusic();
			m_checkBoxBackgroundMusic->SetCheckedNoEvent(isOn);
		}
	}

	void gui_gameSetting::refreshVolume()
	{
		if (m_sliderVolume)
		{
			if (m_sliderVolume->GetProgress() < 0.1F)
			{
				m_sliderVolume->SetProgress(0.0F);
			}
			else if (m_sliderVolume->GetProgress() > 0.9F)
			{
				m_sliderVolume->SetProgress(1.0F);
			}
			SoundSystem::Instance()->setBackgroundMusicVolume(m_sliderVolume->GetProgress());
			SoundSystem::Instance()->setEffectsVolume(m_sliderVolume->GetProgress());
			saveSettingConfig();
		}
	}

	void gui_gameSetting::refreshLuminance()
	{
		if (m_sliderLuminance)
		{
			if (m_sliderLuminance->GetProgress() < 0.1F)
			{
				m_sliderLuminance->SetProgress(0.0F);
			}
			else if (m_sliderLuminance->GetProgress() > 0.9F)
			{
				m_sliderLuminance->SetProgress(1.0F);
			}
			Blockman::Instance()->m_gameSettings->gammaSetting = m_sliderLuminance->GetProgress();
			m_settingMap["luminance"] = m_sliderLuminance->GetProgress();
			saveSettingConfig();
		}
	}

	void gui_gameSetting::refreshHorizon()
	{
		if (m_sliderHorizon)
		{
			if (m_sliderHorizon->GetProgress() < 0.1F)
			{
				m_sliderHorizon->SetProgress(0.0F);
			}
			else if (m_sliderHorizon->GetProgress() > 0.9F)
			{
				m_sliderHorizon->SetProgress(1.0F);
			}
			Blockman::Instance()->m_gameSettings->setFovSetting(m_sliderHorizon->GetProgress());
			m_settingMap["horizon"] = m_sliderHorizon->GetProgress();
			saveSettingConfig();
		}
	}

	void gui_gameSetting::refreshGuiSize()
	{
		if (m_sliderGuiSize)
		{
			if (m_sliderGuiSize->GetProgress() < 0.1F)
			{
				m_sliderGuiSize->SetProgress(0.0F);
			}
			else if (m_sliderGuiSize->GetProgress() > 0.9F)
			{
				m_sliderGuiSize->SetProgress(1.0F);
			}

			float size = GUI_MIN_SIZE + (GUI_MAX_SIZE - GUI_MIN_SIZE) * m_sliderGuiSize->GetProgress();
			Blockman::Instance()->m_gameSettings->playerActivityGuiSize = size;
			m_settingMap["gui_size"] = (float)size;
			saveSettingConfig();
		}
	}

	void gui_gameSetting::refreshCameraSensitive()
	{
		if (m_sliderCameraSensitive)
		{
			if (m_sliderCameraSensitive->GetProgress() < 0.1F)
			{
				m_sliderCameraSensitive->SetProgress(0.0F);
			}
			else if (m_sliderCameraSensitive->GetProgress() > 0.9F)
			{
				m_sliderCameraSensitive->SetProgress(1.0F);
			}
			float minSize = 1.0F;
			float maxSize = 9.0F;
			float size = minSize + (maxSize - minSize) *m_sliderCameraSensitive->GetProgress();
			Blockman::Instance()->m_gameSettings->setCameraSensitive(size);
			m_settingMap["camera_sensitive"] = size;
			saveSettingConfig();
		}
	}

	void gui_gameSetting::refreshFPS()
	{
		if (m_sliderFPS)
		{
			unsigned int maxFPS;
			if (m_sliderFPS->GetProgress() < 0.33f)
			{
				m_sliderFPS->SetProgress(0.0f);
				maxFPS = 30;
			}
			else if (m_sliderFPS->GetProgress() > 0.67f)
			{
				m_sliderFPS->SetProgress(1.0f);
				maxFPS = 60;
			}
			else {
				m_sliderFPS->SetProgress(0.5f);
				maxFPS = 45;
			}
			Blockman::Instance()->m_gameSettings->m_fpsSetting = m_sliderFPS->GetProgress();
			GameClient::CGame::Instance()->SetMaxFps(maxFPS);
			m_settingMap["fps"] = m_sliderFPS->GetProgress();
			saveSettingConfig();
		}
	}

	void gui_gameSetting::refreshPoleControlState(int isChecked)
	{
		Blockman::Instance()->m_gameSettings->usePole = isChecked;
		m_settingMap["usePole"] = (float)isChecked;
		saveSettingConfig();
	}

	void gui_gameSetting::refreshBackgroundMusicSwitch(int isChecked)
	{
		SoundSystem::Instance()->setBackgroundMusicSwitch(isChecked == 1);
		m_settingMap["backgroundMusic"] = (float) isChecked;
		saveSettingConfig();
	}

	void gui_gameSetting::checkBackgroundMusic()
	{
		if (m_settingMap.find("backgroundMusic") != m_settingMap.end())
		{
			float isChecked = m_settingMap["backgroundMusic"];
			refreshBackgroundMusicSwitch(isChecked > 0.0f);
		}
	}

	void gui_gameSetting::onShow()
	{
		setVolume();
		setLuminance();
		setHorizon();
		setGuiSize();
		setCameraSensitive();
		setFPS();
		setPoleControlState();
		setBackgroundMusicSwitc();
	}

	bool gui_gameSetting::onClick(const EventArgs & events, LayoutId layoutId)
	{
		switch (layoutId)
		{
		case LayoutId::LUMINANCE:
			refreshLuminance();
			break;
		case LayoutId::HORIZON:
			refreshHorizon();
			break;
		case LayoutId::VOLUME:
			refreshVolume();
			break;
		case LayoutId::GUI_SIZE:
			refreshGuiSize();
			break;
		case LayoutId::CAMERA_SENSITIVE:
			refreshCameraSensitive();
			break;
		case LayoutId::FPS:
			refreshFPS();
			break;
		}
		return true;
	}

	bool gui_gameSetting::onCheckBoxStateChange(const EventArgs& events, LayoutId layoutId)
	{
		auto windowEvents = dynamic_cast<const WindowEventArgs&>(events);
		auto checkBox = dynamic_cast<GUICheckBox*>(windowEvents.window);
		if (!checkBox)
			return false;

		int isChecked = checkBox->GetChecked() ? 1 : 0;
		switch (layoutId)
		{
		case LayoutId::POLE_STATE:
			refreshPoleControlState(isChecked);
			break;
		case LayoutId::BACKGROUND_MUSIC:
			refreshBackgroundMusicSwitch(isChecked);
			break;
		}
		return true;
	}

	void gui_gameSetting::loadSettingConfig()
	{

		String configPath = PathUtil::ConcatPath(Root::Instance()->getWriteablePath(), "blockManClientConfig.txt");
		if (!PathUtil::IsFileExist(configPath))
		{
			LordLogInfo("configPath file not found [%s].", configPath.c_str());
			return;
		}

		std::ifstream fin(configPath.c_str(), std::ios::in);
		char line[1024];
		while (fin.getline(line, sizeof(line)))
		{
			String lineString = line;
			if (lineString.size() > 0 && lineString.at(0) != '#' && lineString.at(0) != '@')
			{
				StringArray array = StringUtil::Split(lineString, "=", 1);
				if (array.size() == 2)
				{
					m_settingMap[array[0]] = (float)atof(array[1].c_str());
				}
			}
		}
		fin.clear();
		fin.close();

		if (m_settingMap.size() == 0)
		{
			LordLogInfo("m_settingMap size is null");
			return;
		}
		Blockman::Instance()->m_gameSettings->setFovSetting(m_settingMap["horizon"]);
		Blockman::Instance()->m_gameSettings->gammaSetting = m_settingMap["luminance"];
		float fGuiSize = m_settingMap["gui_size"];
		fGuiSize = Math::Max(fGuiSize, GUI_MIN_SIZE);
		fGuiSize = Math::Min(fGuiSize, GUI_MAX_SIZE);
		Blockman::Instance()->m_gameSettings->playerActivityGuiSize = fGuiSize;
		m_settingMap["camera_sensitive"] = Math::Clamp(m_settingMap["camera_sensitive"], 1.0f, 9.0f);
		Blockman::Instance()->m_gameSettings->setCameraSensitive(m_settingMap["camera_sensitive"]);
		if (m_settingMap.find("usePole") != m_settingMap.end())
		{
			if ((int)m_settingMap["usePole"] > 1)
				m_settingMap["usePole"] = 1;
			Blockman::Instance()->m_gameSettings->usePole = (int)m_settingMap["usePole"];
		}

		if (m_settingMap.find("fps") != m_settingMap.end())
		{
			unsigned int maxFPS;
			if (m_settingMap["fps"] < 0.33f)
			{
				m_settingMap["fps"] = 0.0f;
				maxFPS = 30;
			}
			else if (m_settingMap["fps"] > 0.67f)
			{
				m_settingMap["fps"] = 1.0f;
				maxFPS = 60;
			}
			else
			{
				m_settingMap["fps"] = 0.5f;
				maxFPS = 45;
			}
			GameClient::CGame::Instance()->SetMaxFps(maxFPS);
			Blockman::Instance()->m_gameSettings->m_fpsSetting = m_settingMap["fps"];
		}

		if (m_settingMap.find("backgroundMusic") != m_settingMap.end())
		{
			SoundSystem::Instance()->setBackgroundMusicSwitch(m_settingMap["backgroundMusic"] > 0);
		}

		if (m_settingMap.find("audioVolume") != m_settingMap.end())
		{
			SoundSystem::Instance()->setBackgroundMusicVolume(m_settingMap["audioVolume"]);
			SoundSystem::Instance()->setEffectsVolume(m_settingMap["audioVolume"]);
			setVolume();
		}
	}

	void gui_gameSetting::saveSettingConfig()
	{
		//String configPath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Config/gameConfig.txt");
		String configPath = PathUtil::ConcatPath(Root::Instance()->getWriteablePath(), "blockManClientConfig.txt");
		if (!PathUtil::IsFileExist(configPath))
		{
			LordLogInfo("configPath file not found [%s].", configPath.c_str());
		}
		m_settingMap["horizon"] = Blockman::Instance()->m_gameSettings->getFovSetting();
		m_settingMap["luminance"] = Blockman::Instance()->m_gameSettings->gammaSetting;
		m_settingMap["gui_size"] = (float)Blockman::Instance()->m_gameSettings->playerActivityGuiSize;
		m_settingMap["camera_sensitive"] = Blockman::Instance()->m_gameSettings->getCameraSensitive();
		m_settingMap["usePole"] = (float)Blockman::Instance()->m_gameSettings->usePole;
		m_settingMap["fps"] = (float)Blockman::Instance()->m_gameSettings->m_fpsSetting;
		m_settingMap["backgroundMusic"] = SoundSystem::Instance()->isSwitchOnBackgroundMusic() ? 1.0f : 0;
		m_settingMap["audioVolume"] = SoundSystem::Instance()->getEffectsVolume();
		std::ofstream out(configPath.c_str());
		if (out.is_open())
		{
			for (auto value : m_settingMap)
			{
				String str = StringUtil::Format("%s=%.2f\n", value.first.c_str(), value.second);
				out << str.c_str();
			}
			out.close();
		}
	}

}
