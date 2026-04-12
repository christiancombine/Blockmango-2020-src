#pragma once
#ifndef __GUI_BLOCKCITY_TIGER_LOTTERY_HEADER__
#define __GUI_BLOCKCITY_TIGER_LOTTERY_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "GUI/GuiBlockCityTigerLotteryItem.h"

using namespace LORD;

namespace BLOCKMAN
{
	class CityLotteryItem 
	{
	public:
		String m_uniqueId = "";
		int m_type = 0;
		int m_itemId = 0;
		int m_itemIdMate = 0;
	public:
		CityLotteryItem() = default;
		CityLotteryItem(String uniqueId, i32 type, i32 itemId, i32 itemIdMate)
			: m_uniqueId(uniqueId)
			, m_type(type)
			, m_itemId(itemId)
			, m_itemIdMate(itemIdMate)
		{
		};
	};

	class gui_blockCityTigerLottery : public gui_layout
	{
	private:
		enum class ViewId {
			FIRST = 1,
			SECOND = 2,
			THIRD = 3,
		};

	private:
		SubscriptionGuard m_subscriptionGuard;
		
		i32 m_entityId = -1;

		map<String, CityLotteryItem>::type m_items;

		list<String>::type m_firstIds;
		list<String>::type m_secondIds;
		list<String>::type m_thirdIds;

		String m_firstId = "";
		String m_secondId = "";
		String m_thirdId = "";

		float m_firstTopPosY = 0;
		float m_secondTopPosY = 0;
		float m_thirdTopPosY = 0;

		bool m_firstScroll = false;
		bool m_secondScroll = false;
		bool m_thirdScroll = false;

		bool m_firstLock = true;
		bool m_secondLock = true;
		bool m_thirdLock = true;

		int m_firstThroughTimes = 0;
		int m_secondThroughTimes = 0;
		int m_thirdThroughTimes = 0;

		float m_firstMoveSpeed = 0;
		float m_secondMoveSpeed = 0;
		float m_thirdMoveSpeed = 0;

		float m_subMoveSpeed = 2.0f;

		int m_countdownTime = 0;
		int m_countdownCD = 0;

		bool m_firstStop = false;
		bool m_secondStop = false;

	private:
		GUILayout* m_item1_scroll = nullptr;
		GUIStaticText* m_item1_name = nullptr;
		GUIStaticImage* m_item1_mask = nullptr;
		GUIStaticImage* m_item1_lock = nullptr;
		GUIButton* m_item1_choose = nullptr;
		GUIStaticImage* m_item1_question_mark = nullptr;
		map<String, GuiBlockCityTigerLotteryItem*>::type m_item1_childs;

		GUILayout* m_item2_scroll = nullptr;
		GUIStaticText* m_item2_name = nullptr;
		GUIStaticImage* m_item2_mask = nullptr;
		GUIStaticImage* m_item2_lock = nullptr;
		GUIButton* m_item2_choose = nullptr;
		GUIStaticImage* m_item2_question_mark = nullptr;
		map<String, GuiBlockCityTigerLotteryItem*>::type m_item2_childs;

		GUILayout* m_item3_scroll = nullptr;
		GUIStaticText* m_item3_name = nullptr;
		GUIStaticImage* m_item3_mask = nullptr;
		GUIStaticImage* m_item3_lock = nullptr;	
		GUIButton* m_item3_choose = nullptr;
		GUIStaticImage* m_item3_question_mark = nullptr;
		GUIStaticImage* m_item3_vip_icon = nullptr;
		map<String, GuiBlockCityTigerLotteryItem*>::type m_item3_childs;

		GUIButton* m_lottery_begin = nullptr;
		GUIStaticText* m_countdown = nullptr;
		GUIStaticImage* m_up = nullptr;
		GUIStaticImage* m_down = nullptr;

	public:
		gui_blockCityTigerLottery();
		~gui_blockCityTigerLottery();

		void onLoad() override;
		bool isNeedLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onUpdateChild(String selectId, int& throughTimes, float& moveSpeed, map<String, GuiBlockCityTigerLotteryItem*>::type childs, float topPosY, ViewId viewId);

		bool onLotteryData(const String& data);
		void initAllChilds();
		float initItemChilds(GUILayout* parent, map<String, GuiBlockCityTigerLotteryItem*>::type& childs, list<String>::type ids);
		bool onLotteryResult(const String& firstId, const String& secondId, const String& thirdId);
		void setLotteryResultName(String uniqueId, list<String>::type ids, GUIStaticText* item);

 		bool onSelectFirst(const EventArgs & events);
		bool onSelectSecond(const EventArgs & events);
		bool onSelectThird(const EventArgs & events);
		bool onLotteryBegin(const EventArgs & events);
		bool onProbabilityClick(const EventArgs& events);

		void selectItem(String uniqueId);

		bool setLotteryDataByEntityId(i32 entityId);
		void initLotteryUiByEntityId(bool isVipTiger);
		void onGetCommonData(String key, String data) override;

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif