#ifndef __GUI_BLOCKCITY_PORTAL_HEADER__
#define __GUI_BLOCKCITY_PORTAL_HEADER__

#include "GUI/gui_layout.h"
#include "Setting/BlockCityPortalSetting.h"
using namespace LORD;
namespace LORD
{
	class GUILayout;
}

namespace BLOCKMAN
{
	class gui_blockCityPortal : public gui_layout
	{
	private:
		enum ViewId
		{
			CLOSE,
			TRANSFER,
		};

	public:
		gui_blockCityPortal();
		~gui_blockCityPortal();

		void onLoad() override;
		void onShow() override;
		void onHide() override;
		bool isNeedLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isForceUpdate() override { return true; }

		void showPortal(i32 portalId);

	private:
		bool onItemClick(const BlockCityPortalItem & item);
		bool onClick(const EventArgs& args, ViewId viewId);

	private:
		GUILayout* m_items = nullptr;

		bool m_isShowUI = false;
		i32 m_portalId = 0;
		BlockCityPortalItem m_curItem;

	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
