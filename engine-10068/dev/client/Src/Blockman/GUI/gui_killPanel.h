#pragma once
#ifndef __GUI_KILL_PANEL_HEADER__
#define __GUI_KILL_PANEL_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIString.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class KillMsg 
	{
	public:
		int	kills = 1;
		int head = 0;
		int armsId = 0;
		int poison = 0;
		int spike = 0;
		String killer = "";
		String killerFame = "";
		String dead = "";
		String deadFame = "";
		String color = "";
		ui32 time = 3000;
		GUIWindow* window = nullptr;
	public:
		KillMsg(int kills, int head, int armsId, int poison, int spike, String killer, String dead, String color, String killerFame, String deadFame) :
			kills(kills), head(head), armsId(armsId), poison(poison), spike(spike), killer(killer), dead(dead), color(color), killerFame(killerFame),
			deadFame(deadFame)
		{
			time = 3000;
		}
	};

	class PlayInfo
	{
	public:
		GUIString icon = "";
		uint sound = 0;
		i32 stay = 0;
		i32 animate = 0;
	};

	class gui_killPanel : public gui_layout
	{
	private:
		static unsigned messageIndex;

		GUILayout* m_killMsgList = nullptr;
		std::list<KillMsg*> m_msgs;
		SubscriptionGuard m_subscriptionGuard;

		GUIStaticImage* m_comboKillIcon = nullptr;
		bool				m_isPlaying = false;
		LORD::UVector2		m_originPos;
		int					m_totalTime = 0;
		int					m_animateTime = 500;
		int					m_stayTime = 500;
		float				m_moveDistance = 0.0f;
		PlayInfo m_playNow;
		PlayInfo m_playNext;
		String tipTxt = "";

	private:
		void updateChildrens(ui32 nTimeElapse);
		void updateIcon(ui32 nTimeElapse);
		void addKillMsg(KillMsg* msg);
		void addNormalMsg(const String& msg, const String& name,const String& color);
	public:
		gui_killPanel();
		~gui_killPanel();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		bool onReceiveKillMsg(const String& data);
		bool  showComboKill(const String& icon, uint soundType, i32 stayTime, i32 animateTime);
		void onGetCommonData(String key, String data);
	};

}
#endif