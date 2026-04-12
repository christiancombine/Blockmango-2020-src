#ifndef __GUI_SKYBLOCK_PRODUCTUPDATE_HEADER__
#define __GUI_SKYBLOCK_PRODUCTUPDATE_HEADER__
#include "Core.h"
#include "GUI/gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "Util/TipType.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_skyblock_productUpdate : public gui_layout
	{

	public:
		gui_skyblock_productUpdate();
		~gui_skyblock_productUpdate();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onCloseClick(const EventArgs & events);
		bool onSureClick(const EventArgs & events);
		bool onCostClick(const EventArgs & events);
		bool onAdClick(const EventArgs & events);
		bool isNeedLoad() override;
		bool onCubeAdVisible(bool visible);
		void onGetCommonData(String key, String data);
	private:
		void showUpdateProductMessage(const String& data);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	private:
		int m_unlockCost = 0;
		int m_costprice = 0;
		int m_npcid = 0;
		int m_npclevel = 0;
		int m_unlockType = 0;
		bool m_have_cube_ad = false;
		enum PriceType
		{
			LEFT_MAGIC = 1,
			LEFT_MONEY,
			RIGHT_MAGIC,
		};
		GUIStaticText*      m_current_text = nullptr;
		GUIStaticText*      m_next_text = nullptr;
		GUIStaticText*      m_capacity = nullptr;
		GUIStaticText*      m_one = nullptr;
		GUIStaticText*      m_two = nullptr;
		GUIStaticText*      m_third = nullptr;
		GUIStaticText*      m_four = nullptr;
		GUIStaticText*      m_five = nullptr;
		GUIStaticText*      m_six = nullptr;
		GUIStaticText*      m_current_capacity = nullptr;
		GUIStaticText*      m_next_capacity = nullptr;
		GUIStaticText*		m_valueText = nullptr;
		GUIStaticText*		m_first_current = nullptr;
		GUIStaticText*		m_first_next = nullptr;
		GUIStaticText*		m_first_current_num = nullptr;
		GUIStaticText*		m_first_next_num = nullptr;
		GUIStaticText*		m_second_current_num = nullptr;
		GUIStaticText*		m_second_next_num = nullptr;
		GUIStaticText*		m_third_current_num = nullptr;
		GUIStaticText*		m_third_next_num = nullptr;
		GUIStaticText*		m_current_num = nullptr;
		GUIStaticText*		m_next_num = nullptr;
		GUIStaticText*		m_second_current = nullptr;
		GUIStaticText*		m_second_next = nullptr;
		GUIStaticText*		m_third_current = nullptr;
		GUIStaticText*		m_third_next = nullptr;
		GUIStaticText*		m_price = nullptr;
		GUIStaticText*		m_discount = nullptr;
		GUIStaticText*      m_adprice = nullptr;
		GUIStaticImage*     m_price_png = nullptr;
		GUIStaticImage*     m_first_next_Icon = nullptr;
		GUIStaticImage*     m_first_current_Icon = nullptr;
		GUIStaticImage*     m_second_current_Icon = nullptr;
		GUIStaticImage*     m_second_next_Icon = nullptr;
		GUIStaticImage*     m_third_current_Icon = nullptr;
		GUIStaticImage*     m_third_next_Icon = nullptr;
		GUIStaticImage*     m_arrow = nullptr;
		GUIButton*          btnCost = nullptr;
		GUIButton*          btnAd = nullptr;
		GUIButton*          btnSure = nullptr;
		
	};
}
#endif
