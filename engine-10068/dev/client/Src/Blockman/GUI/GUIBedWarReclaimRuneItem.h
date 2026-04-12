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
	class GuiBedWarReclaimRuneItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUIWindow* m_window = nullptr;
		void removeComponents();

	public:
		GuiBedWarReclaimRuneItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBedWarReclaimRuneItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void RefreshItem(int id, int num);
		bool onRuneClick(const EventArgs & events);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

		GUIStaticImage* m_rune_img = nullptr;
		GUIStaticText* m_rune_num_txt = nullptr;
		GUIStaticText* m_rune_title = nullptr;
		GUIStaticText* m_rune_des = nullptr;
		GUIButton* m_rune_btn = nullptr;

		int m_rune_id = 0;
		int m_rune_num = 0;
	};

}

