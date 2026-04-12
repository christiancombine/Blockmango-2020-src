#ifndef __GUI_DESIGNATION_ITEM_HEADER__
#define __GUI_DESIGNATION_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/DesignationSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIDesignationItem : public CustomGuiWindow
	{
	private:
		DesignationItem* m_data = NULL;
		GUIWindow* m_window = nullptr;
		GUIStaticImage* m_left_image = nullptr;
		GUIStaticText* m_left_image_text= nullptr;
		GUIStaticImage* m_center_image = nullptr;
		GUIStaticText* m_center_image_text = nullptr;
		GUIButton* m_right_button = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void initView();

	public:
		GUIDesignationItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIDesignationItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void InitDesignationItem(DesignationItem* item);

	private:
		bool OnItemChick(const EventArgs & events);
	};
}

#endif
