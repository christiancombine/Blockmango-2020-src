#pragma once
#ifndef __GUI_BIRD_PACKANDFUSE_HEADER__
#define __GUI_BIRD_PACKANDFUSE_HEADER__

#include "gui_layout.h"

#include "Game/BirdSimulator.h"


using namespace LORD;
namespace LORD {
	class GUIWindow;
	class GUILayout;
	class GUIRadioButton;
	class GUIListHorizontalBox;
	class GUIGridView;
	class GUIStaticImage;
	class GUIStaticText;
}
namespace BLOCKMAN
{
	class GuiBirdPack;
	class GuiBirdFuse;
	class GuiBirdTool;
	class gui_birdPackAndFuse : public  gui_layout
	{
	private:
		enum class ViewId {
			MAIN,
			PACK,
			FUSE,
			TOOL,
			QUALITY,
			LEVEL,
			CLOSE,
			CARRAY,
			CAPACITY,
			GRID_VIEW,
			HIDE_VIEW
		};

	public:
		gui_birdPackAndFuse();
		virtual ~gui_birdPackAndFuse();
		void onHide() override;
		void onShow() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onPackUpdate();
		bool onRadioChange(const EventArgs & events, ViewId viewId);
		bool onRadioSortChange(const EventArgs & events, ViewId viewId);
		bool onClick(const EventArgs & events, ViewId viewId);
		bool openExpandCarry();
		i32 getShowType();
		bool onShowBirdInfo(i64 birdId, bool isRefresh = true);
		bool onShowBirdCarryGuide();
		bool onShowToolInfo(i32 toolId);
		void setShowPack();
		bool isNeedLoad() override;

	private:
		void sortItemSizeChange();
		void toolItemSizeChange();

		void initPackItem();
		void initFuseItem();
		void initToolItem();
		void changeSortBirdItem(bool isForceRefresh);
		bool onFuseRemoveItem(i64 birdId);
		bool onFusePutItem(i64 birdId, const String& image, const String& backImage);
		void setIconAndName(ViewId viewId);
		void changeToolItem();

	private:
		bool m_isPackUpdate = false;
		bool m_isShowCarryGuide = false;

		float m_gvSortXSize = 0.f;
		float m_gvToolXSize = 0.f;

		SubscriptionGuard m_subscriptionGuard;
		map<ViewId, GUIWindow*>::type m_radioContentMap;
		map<ViewId, GUIRadioButton*>::type m_radioMap;

		map<ViewId, GUIRadioButton*>::type m_radioSortMap;
		GUIGridView* m_gvSort = nullptr;

		GUILayout* m_pack = nullptr;
		GUILayout* m_fuse = nullptr;
		GUILayout* m_tool = nullptr;
		GUILayout* m_carryTab = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIStaticText* m_name = nullptr;
		GuiBirdPack* m_birdPackView = nullptr;
		GuiBirdFuse* m_birdFuseView = nullptr;
		GuiBirdTool* m_birdToolView = nullptr;
		GUIGridView* m_gvTool = nullptr;
		
		i32 m_birdSize = 0;
		BirdInfo m_birdInfo;
		BirdStoreItem m_toolInfo;
	};
}

#endif