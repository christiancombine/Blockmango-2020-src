#pragma once
#ifndef __GUI_BLOCK_CITY_STORE_FURNITURE_HEADER__
#define __GUI_BLOCK_CITY_STORE_FURNITURE_HEADER__

#include "gui_layout.h"
#include "Game/BlockCity.h"

using namespace LORD;
namespace LORD {
	class GUILayout;
	class GUIGridView;
	class GUIRadioButton;
}

namespace BLOCKMAN
{
	class gui_blockCityStoreFurniture : public  gui_layout
	{

	public:
		gui_blockCityStoreFurniture();
		virtual ~gui_blockCityStoreFurniture();
		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onDataChange();
		bool isNeedLoad() override;
	private:
		void initData();
		void initTabs(std::vector<BlockCityStoreGroup>& groups);
		void updateItems(std::vector<BlockCityStoreItem>& items);

	private:
		bool onRadioChange(const EventArgs& args, i32 groupId);
		bool onItemClick(const BlockCityStoreItem & item);

	public:
		 UVector2 getItemPosition() override;

	private:
		BlockCityStoreItem m_item;
		bool m_dataChange = false;
		map<i32, GUIRadioButton*>::type m_radioMap;
		GUILayout* m_tabs = nullptr;
		GUIGridView* m_itemsView = nullptr;
	};
}

#endif