#pragma once

#ifndef __GUI_ITEM_SLOT_HEADER__
#define __GUI_ITEM_SLOT_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIProgressBar.h"
#include "GUI/CustomGuiWindow.h"
#include "GUI/GuiItemStack.h"

using namespace LORD;

namespace BLOCKMAN
{
	class Ingredient;

	enum class ArmorType
	{
		NONE = -1,
		HELMET,
		CHESTPLATE,
		LEGGINGS,
		BOOTS
	};

	class GuiItemSlot : public CustomGuiWindow
	{
	private:
		using IngredientPtr = std::shared_ptr<Ingredient>;

	public:
		enum Style// : signed char
		{
			INVALID = -1,
			ROCK_TRANS = 0,
			ROCK_SOLID = 1,
			METAL = 2,
			BLOCKCITY = 3,
			SKYBLOCK = 7,
			WALKING_DEAD = 8,
			BLOCK_FORT = 9,
			BLOCK_FORT_PACK = 10,
			SKYBLOCK2 = 11,
			WALKING_DEAD_ARMOR = 12,
			SKY_WAR_LUCKY_BLOCK1 = 14,
			SKY_WAR_LUCKY_BLOCK2 = 15,
			SKY_WAR_LUCKY_BLOCK3 = 16
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUIStaticImage* m_window = nullptr;
		GUIStaticImage* m_armorTypeImage = nullptr;
		GUIStaticImage* m_itemBackGround = nullptr;
		GUIWindow* m_enchantmentIndicator = nullptr;
		GuiItemStack* m_guiItemStack = nullptr;
		GUIProgressBar* m_dropItemProgress = nullptr;
		GUIProgressBar* m_skillCdProgress = nullptr;
		GUIProgressBar* m_durationProgress = nullptr;
		ArmorType m_armor_type = ArmorType::NONE;
		bool m_selected = false;
		Style m_style = Style::ROCK_TRANS;
		Style m_oldStyle = Style::INVALID;
		bool is_modified_style = false;

		bool onItemStackTouchDown(const EventArgs & events)
		{
			WindowEventArgs new_event(this);
			fireEvent("ItemStackTouchDown", new_event);
			return true;
		}

		bool onItemStackTouchUp(const EventArgs & events)
		{
			WindowEventArgs new_event(this);
			fireEvent("ItemStackTouchUp", new_event);
			return true;
		}

		bool onItemStackLongTouchStart(const EventArgs & events)
		{
			WindowEventArgs new_event(this);
			fireEvent("ItemStackLongTouchStart", new_event);
			return true;
		}

		bool onItemStackLongTouchEnd(const EventArgs & events) 
		{
			WindowEventArgs new_event(this);
			fireEvent("ItemStackLongTouchEnd", new_event);
			return true;
		}

		bool onItemStackClick(const EventArgs & events);

		void removeComponents();
		void updateStyle();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GuiItemSlot(const GUIWindowType& nType, const GUIString& strName);
		~GuiItemSlot() = default;

		GUIStaticImage* getWindow() { return m_window; }

		bool isSelected()
		{
			return m_selected;
		}

		void setSelected(bool selected)
		{
			m_selected = selected;
			is_modified_style = true;
		}

		void setItemStack(ItemStackPtr pItemStack)
		{
			m_guiItemStack->setItemStack(pItemStack);
		}

		void setIngredient(IngredientPtr pIngredient)
		{
			m_guiItemStack->setIngredient(pIngredient);
		}

		void setStyle(Style style)
		{
			m_style = style;
			is_modified_style = true;
		}

		void setStyle(int style)
		{
			setStyle((Style)style);
		}

		void setItemStackBackgroundColor(const Color& color)
		{
			m_guiItemStack->SetBackgroundColor(color);
		}

		void setItemStackNumberVisibility(GuiItemStack::NumberVisibility visibility)
		{
			m_guiItemStack->setNumberVisibility(visibility);
		}

		void setItemStackGrayedOut(bool grayedOut, int type = 0)
		{
			m_guiItemStack->setGrayedOut(grayedOut, type);
		}

		void setPlayEffect(bool playEffect)
		{
			m_guiItemStack->setPlayEffect(playEffect);
		}

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void updateItemDurationInfo(ui32 timeElapse);

		UVector2 getGuiItemStackPosition()
		{
			return m_guiItemStack->getAbsolutePositionFromRoot();
		}

		GuiItemStack* cloneGuiItemStack(const GUIString& newName);
		void setArmorType(ArmorType type);
		void setDropItemProgress(float progress);
		void setItemSkillCdProgress(int curValue, int maxValue);
		void setItemDurationProgress(int curValue, int maxValue);
	};
}

#endif
