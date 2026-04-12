#pragma once
#ifndef __GUI_PIXEL_CHICKEN_MAIN_HEADER__
#define __GUI_PIXEL_CHICKEN_MAIN_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_pixelChickenMain : public gui_layout
	{
		enum WarningType
		{
			SHOOT = 1,
			FOOT,
			GUN,
			WEAPON_INFO
		};

		enum WatchButton
		{
			BACK = 1,
			NEXT,
			EXIT
		};

	private:
		bool m_can_show_warning = false;

		int m_poison_time = 0;
		float m_change_weapon_section = 166.67f;
		int m_change_weapon_wait = 0;
		float m_shootWarningRotateAngle = 0.0f;
		int m_shootWarningTime = 1500;

		float m_footWarningRotateAngle = 0.0f;
		int m_footWarningTime = 1500;

		float m_gunWarningRotateAngle = 0.0f;
		float m_shootWarningRange = 0.f;
		float m_footWarningRange = 0.f;
		float m_weaponWarningRange = 0.f;

		int m_revive_time = 0; //可复活的时间

		bool m_isShowWeaponView = true;

		GUIStaticImage* m_weapon = nullptr;
		GUIStaticImage* m_weapon_progress = nullptr;
		GUIStaticImage* m_weapon_icon = nullptr;
		GUIStaticText* m_survive = nullptr;
		GUIStaticText* m_kill = nullptr;
		GUIStaticText* m_poison = nullptr;
		GUIStaticText* m_damage_value = nullptr; 
		GUIStaticText* m_speed_value = nullptr;
		GUIStaticImage* m_shoot = nullptr;
		GUIStaticImage* m_foot = nullptr;
		GUIStaticImage* m_gun = nullptr;
		GUIStaticImage* m_weapon_info_img = nullptr;
		GUILayout* m_watch = nullptr;
		GUILayout* m_weapon_info = nullptr;
		GUILayout* m_weapon_info_view = nullptr;
		GUILayout* m_weapon_info_property = nullptr;
		GUIButton* m_back = nullptr;
		GUIButton* m_exit = nullptr;
		GUIButton* m_next = nullptr;
		GUIButton* m_weapon_info_btn = nullptr;
		GUIStaticImage* m_poison_mask = nullptr;

		GUIStaticImage* m_no_revive_icon = nullptr;
		GUIStaticText* m_revive_text = nullptr;
		GUILayout* m_revive_bg = nullptr;
	public:
		gui_pixelChickenMain();
		virtual ~gui_pixelChickenMain();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isForceUpdate() { return true; }
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:
		bool resetWeaponIcon(const String& image, const String& bg, int time);
		void resetChickenInfo(const String& data);
		void updateWeaponInfo(const String& data);
		bool showWarning(const Vector3& position, int type);
		bool hideWarning(int type);
		bool setCanWarning(bool canShowWarning) { m_can_show_warning = canShowWarning; return true; };
		bool watchButtonClick(WatchButton type);
		bool showPoisonMask(bool isShow);
		void setWarningRange(float shootRange, float footRange, float weaponRange);
		float getWarningRange(int type);
		void setReviveStatus(bool canRevive);
		String getTimeString(int time);
		bool ifShowWeaponView();

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}
#endif 
