#pragma once
#ifndef __GUI_BLOCKFORT_BUILDING_INFO_HEADER__
#define __GUI_BLOCKFORT_BUILDING_INFO_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "UI/GUIListBox.h"
#include "Game/BlockFort.h"
#include "Setting/BlockFortItemSetting.h"

using namespace LORD;
namespace BLOCKMAN
{
	class gui_blockFortBuildingInfo : public gui_layout
	{
	public:
		gui_blockFortBuildingInfo();
		~gui_blockFortBuildingInfo();
		void onLoad() override;
		bool isNeedLoad() override;

	private:
		GUILayout* m_mainLayout = nullptr;
		GUILayout* m_descLayout = nullptr;
		GUILayout* m_descFortLayout = nullptr;
		GUIListBox* m_descTurretAndWallList = nullptr;


		GUIStaticImage* m_mainIcon = nullptr;
		GUIStaticText* m_mainTitleText = nullptr;

		GUIButton* m_removeBtn = nullptr;
		GUIStaticImage* m_removeBtnIcon = nullptr;
		GUIStaticText* m_removeBtnText = nullptr;

		GUIButton* m_upgradeBtn = nullptr;
		GUIStaticImage* m_upgradeBtnIcon = nullptr;
		GUIStaticText* m_upgradeBtnText = nullptr;

	private:
		enum class ViewId {
			REMOVE,
			UPGRADE,
			CLOSE
		};
		enum class BuildingType {
			FORT = 1,
			TURRET,
			WALL
		};
		enum class OperationType {
			REMOVE = 1,
			UPGRADE = 2
		};
		bool onClick(const EventArgs& args, ViewId viewId);
		bool openBuildingInfoByEvent(i32 id, i32 index);
		bool openBuildingInfo(i32 id, bool isUpdate);
		void changeUI(const BlockFortBuilding& itemInfos);

	private:
		i32 m_Index = 0;
		i32 m_id = 0;
	};
}

#endif