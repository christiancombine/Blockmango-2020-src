#pragma once

#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBedWarRuneProItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUIWindow* m_window = nullptr;
		void removeComponents();

	public:
		GuiBedWarRuneProItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBedWarRuneProItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void RefreshItem(int index, int rune_id, bool select);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

		GUIStaticImage* m_rune_img_bg = nullptr;
		GUIStaticText* m_rune_pro_txt = nullptr;
		int m_rune_id = 0;
	};

}


