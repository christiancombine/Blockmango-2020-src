#pragma once

#ifndef __DYNAMIC_CAST_HELPER_H__
#define __DYNAMIC_CAST_HELPER_H__

#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "UI/GUIGridView.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITableView.h"
#include "UI/GUIAnimateWindow.h"
#include "UI/GUIBubbleContain.h"
#include "UI/GUICell.h"
#include "UI/GUIEditBox.h"
#include "UI/GUIInputBox.h"
#include "UI/GUIMultiLineEdit.h"
#include "UI/GUIPageView.h"
#include "UI/GUISceneMap.h"
#include "UI/GUIScrollablePane.h"
#include "UI/GUIScrollCard.h"
#include "UI/GUIScrolledContainer.h"
#include "UI/GUISlider.h"
#include "GUI/GuiUrlImage.h"
#include "GUI/GuiActorWindow.h"
#include "GUI/GuiEffectViewOfObtainment.h"
#include "GUI/GuiItemStack.h"
#include "GUI/GuiItemSlot.h"
#include "GUI/GuiSlotTable.h"
#include "GUI/GuiScrollableInventory.h"
#include "GUI/gui_menu.h"
#include "GUI/gui_gameSetting.h"
#include "GUI/gui_mainControl.h"
#include "GUI/gui_chat.h"

using namespace LORD;

struct DynamicCastHelper
{
#pragma region //GUIWindow* window
	static GUILayout* dynamicCastLayout(GUIWindow* window)
	{
		return dynamic_cast<GUILayout*>(window);
	}

	static GUIStaticText* dynamicCastStaticText(GUIWindow* window)
	{
		return dynamic_cast<GUIStaticText*>(window);
	}

	static GUIStaticImage* dynamicCastStaticImage(GUIWindow* window)
	{
		return dynamic_cast<GUIStaticImage*>(window);
	}

	static GUIButton* dynamicCastButton(GUIWindow* window)
	{
		return dynamic_cast<GUIButton*>(window);
	}

	static GUIGridView* dynamicCastGridView(GUIWindow* window)
	{
		return dynamic_cast<GUIGridView*>(window);
	}

	static GUIListBox* dynamicCastListBox(GUIWindow* window)
	{
		return dynamic_cast<GUIListBox*>(window);
	}

	static GUIListHorizontalBox* dynamicCastListHorizontalBox(GUIWindow* window)
	{
		return dynamic_cast<GUIListHorizontalBox*>(window);
	}

	static GUICheckBox* dynamicCastCheckBox(GUICheckBox* window)
	{
		return dynamic_cast<GUICheckBox*>(window);
	}

	static GUIProgressBar* dynamicCastProgressBar(GUIWindow* window)
	{
		return dynamic_cast<GUIProgressBar*>(window);
	}

	static GUIRadioButton* dynamicCastRadioButton(GUIWindow* window)
	{
		return dynamic_cast<GUIRadioButton*>(window);
	}

	static GUITableView* dynamicCastTableView(GUIWindow* window)
	{
		return dynamic_cast<GUITableView*>(window);
	}

	static GUIAnimateWindow* dynamicCastAnimateWindow(GUIWindow* window)
	{
		return dynamic_cast<GUIAnimateWindow*>(window);
	}

	static GUIBubbleContain* dynamicCastBubbleContain(GUIWindow* window)
	{
		return dynamic_cast<GUIBubbleContain*>(window);
	}

	static GUICell* dynamicCastCell(GUIWindow* window)
	{
		return dynamic_cast<GUICell*>(window);
	}

	static GUIEditBox* dynamicCastEditBox(GUIWindow* window)
	{
		return dynamic_cast<GUIEditBox*>(window);
	}

	static GUIInputBox* dynamicCastInputBox(GUIWindow* window)
	{
		return dynamic_cast<GUIInputBox*>(window);
	}

	static GUIMultiLineEdit* dynamicCastMultiLineEdit(GUIWindow* window)
	{
		return dynamic_cast<GUIMultiLineEdit*>(window);
	}

	static GUIPageView* dynamicCastPageView(GUIWindow* window)
	{
		return dynamic_cast<GUIPageView*>(window);
	}

	static GUISceneMap* dynamicCastSceneMap(GUIWindow* window)
	{
		return dynamic_cast<GUISceneMap*>(window);
	}

	static GUIScrollablePane* dynamicCastScrollablePane(GUIWindow* window)
	{
		return dynamic_cast<GUIScrollablePane*>(window);
	}

	static GUIScrollCard* dynamicCastScrollCard(GUIWindow* window)
	{
		return dynamic_cast<GUIScrollCard*>(window);
	}

	static GUIScrolledContainer* dynamicCastScrolledContainer(GUIWindow* window)
	{
		return dynamic_cast<GUIScrolledContainer*>(window);
	}

	static GUISlider* dynamicCastSlider(GUIWindow* window)
	{
		return dynamic_cast<GUISlider*>(window);
	}

	static GuiUrlImage* dynamicCastUrlImage(GUIWindow* window)
	{
		return dynamic_cast<GuiUrlImage*>(window);
	}

	static GuiActorWindow* dynamicCastActorWindow(GUIWindow* window)
	{
		return dynamic_cast<GuiActorWindow*>(window);
	}

	static GuiEffectViewOfObtainment* dynamicCastEffectViewOfObtainment(GUIWindow* window)
	{
		return dynamic_cast<GuiEffectViewOfObtainment*>(window);
	}

	static GuiItemStack* dynamicCastItemStack(GUIWindow* window)
	{
		return dynamic_cast<GuiItemStack*>(window);
	}

	static GuiSlotTable* dynamicCastSlotTable(GUIWindow* window)
	{
		return dynamic_cast<GuiSlotTable*>(window);
	}

	static GuiScrollableInventory* dynamicCastScrollableInventory(GUIWindow* window)
	{
		return dynamic_cast<GuiScrollableInventory*>(window);
	}

	static GuiItemSlot* dynamicCastItemSlot(GUIWindow* window)
	{
		return dynamic_cast<GuiItemSlot*>(window);
	}
#pragma endregion

#pragma region //gui_layout* layout
	static gui_menu* dynamicCastGuiMenu(gui_layout* layout)
	{
		return dynamic_cast<gui_menu*>(layout);
	}

	static gui_gameSetting* dynamicCastGuiGameSetting(gui_layout* layout)
	{
		return dynamic_cast<gui_gameSetting*>(layout);
	}

	static gui_mainControl* dynamicCastGuiMainControl(gui_layout* layout)
	{
		return dynamic_cast<gui_mainControl*>(layout);
	}

	static gui_chat* dynamicCastGuiChat(gui_layout* layout)
	{
		return dynamic_cast<gui_chat*>(layout);
	}
#pragma endregion

};

#endif