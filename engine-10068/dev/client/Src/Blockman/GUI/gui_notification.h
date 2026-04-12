#pragma once
#ifndef __GUI_NOTIFICATION_HEADER__
#define __GUI_NOTIFICATION_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class Notify
	{
	public:
		String message = "";
		bool top = false;
		bool isVip = false;
		bool isShowBtn = false;
		i64 targetId = 0;

		Notify(String message, bool top, bool isVip, bool isShowBtn, i64 targetId)
		{
			this->message = message;
			this->top = top;
			this->isVip = isVip;
			this->isShowBtn = isShowBtn;
			this->targetId = targetId;
		}
	};

	class Notification
	{
	public:
		int TYPE_DEFAULT = 0;
		int TYPE_CUSTOM = 1;
		int WAIT_TIME = 3000;
		int WAIT_LONG_TIME = 5000;
		i64 m_currentTargetId = 0;
		int m_current_show_type = 0;

		int m_current_wait_time = 0;
		int m_current_roll_time = 0;

		float m_height = 26.0f;

		std::list<String> m_current_notification;
		std::vector<Notify> m_default_notifications;
		std::list<Notify> m_custom_notifications;
		GUIWindow* m_rootWindow = nullptr;
		GUILayout* m_content = nullptr;
		GUILayout* m_effectLayout = nullptr;
		std::list<GUIStaticText*> m_texts;
		std::list<GUIStaticImage*> m_vipImage;
		std::list<GUIButton*> m_visitBtn;

	public:
		void onUpdate(ui32 nTimeElapse);
		bool onReceiveNotification(int type, const String& data);
		void splitNotification(String notification);
		void addNotification(int type, String notification, bool top, bool isVip, bool isShowBtn, i64 targetId);
		String getNotification(bool &isVip, bool &isShowBtn);
	};

	class gui_notification : public gui_layout
	{
	private:
		Notification* m_notification = nullptr;

	public:
		gui_notification();
		~gui_notification();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		bool onReceiveNotification(int type, const String& data);
		bool onSkyBlockVisitEvent();
	};

}
#endif