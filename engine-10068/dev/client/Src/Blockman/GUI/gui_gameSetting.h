#pragma once
#ifndef __GUI_GAME_SETTING_HEADER__
#define __GUI_GAME_SETTING_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUISlider.h"
#include "UI/GUICheckBox.h"
#include "GUI/gui_menu.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_gameSetting : public  gui_layout
	{
	private:
		enum class LayoutId
		{
			LUMINANCE,
			HORIZON,
			VOLUME,
			GUI_SIZE,
			CAMERA_SENSITIVE,
			FPS,
			POLE_STATE,
			BACKGROUND_MUSIC
		};

	public:
		gui_gameSetting();
		virtual ~gui_gameSetting();
		void onLoad() override;
		void onShow() override;
		gui_menu* getParent()
		{
			return dynamic_cast<gui_menu*>(m_parentLayout);
		}
		void checkBackgroundMusic();
		void refreshPoleControlState(int checked);
	private:
		bool onClick(const EventArgs& events, LayoutId layoutId);
		bool onCheckBoxStateChange(const EventArgs& events, LayoutId layoutId);
		void loadSettingConfig();
		void saveSettingConfig();
		void setVolume();
		void setLuminance();
		void setHorizon();
		void setGuiSize();
		void setCameraSensitive();
		void setFPS();
		void setPoleControlState();
		void setBackgroundMusicSwitc();

		void refreshVolume();
		void refreshLuminance();
		void refreshHorizon();
		void refreshGuiSize();
		void refreshCameraSensitive();
		void refreshFPS();
		void refreshBackgroundMusicSwitch(int checked);

	private:
		const float GUI_MIN_SIZE = 0.5F;
		const float GUI_MAX_SIZE = 1.0F;
		GUISlider* m_sliderLuminance = nullptr;
		GUISlider* m_sliderHorizon = nullptr;
		GUISlider* m_sliderVolume = nullptr;
		GUISlider* m_sliderGuiSize = nullptr;
		GUISlider* m_sliderCameraSensitive = nullptr;
		GUISlider* m_sliderFPS = nullptr;
		GUICheckBox* m_checkboxPoleControlState = nullptr;
		GUICheckBox* m_checkBoxBackgroundMusic = nullptr;
		map<String, float>::type m_settingMap;
	};
}

#endif
