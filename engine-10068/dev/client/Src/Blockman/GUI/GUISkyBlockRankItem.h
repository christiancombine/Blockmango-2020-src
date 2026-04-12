#ifndef __GUI_SKYBLOCK_RASNK_ITEM_HEADER__
#define __GUI_SKYBLOCK_RASNK_ITEM_HEADER__
#include "UI/GUIWindow.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticText.h"
#include "gui_skyBlockRank.h"

using namespace LORD;
using namespace GameClient;

namespace BLOCKMAN
{
	class GUISkyBlockRankItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUIWindow* m_window = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_rank = nullptr;
		GUIStaticText* m_score = nullptr;

		GUIButton* m_btn = nullptr;
		GUIStaticImage* m_rankIcon = nullptr;

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GUISkyBlockRankItem(const GUIWindowType& nType, const GUIString& strName);
		~GUISkyBlockRankItem() = default;

		void Destroy() override;
		void UpdateSelf(ui32 nTimeElapse) override;
		void RefreshItem(SKyBlockRankData* rankData);
		bool onClick();
	private:
		ui64 m_userId = 0;
	};
}

#endif
