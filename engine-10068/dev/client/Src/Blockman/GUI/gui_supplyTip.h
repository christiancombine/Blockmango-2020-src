#pragma once
#ifndef __GUI_SUPPLY_TIP_HEADER__
#define __GUI_SUPPLY_TIP_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include <list>

using namespace LORD;

namespace BLOCKMAN
{
	class OffsetView
	{
	public:
		GUIWindow* m_view = nullptr;
		UVector2 m_startPos;
		UVector2 m_endPos;
		int m_duration = 0;
		int m_progress = 0;
		OffsetView* m_next = nullptr;

	public:
		OffsetView(GUIWindow* view, UVector2 startPos, UVector2 endPos, int duration, OffsetView* next)
			: m_view(view),
			m_startPos(startPos),
			m_endPos(endPos),
			m_duration(duration),
			m_next(next)
		{}
	};

	class gui_supplyTip : public  gui_layout
	{
	public:
		gui_supplyTip();
		virtual ~gui_supplyTip();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		void updateOffsetViews(ui32 nTimeElapse);
		void updateOffsetView(OffsetView* offsetView, ui32 nTimeElapse);
		void deleteOffsetView(OffsetView* offsetView);
		bool createHealthAddUI(const String& icon, int addValue);
		bool createDefenseAddUI(const String& icon, int addValue);
		bool createBulletAddUI(const String& icon, int addValue);

	private:
		list<OffsetView*>::type m_offsetViews;
	};
}
#endif