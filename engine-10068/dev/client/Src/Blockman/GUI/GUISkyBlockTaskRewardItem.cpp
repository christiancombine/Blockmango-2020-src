#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "Setting/SkyBlockTaskSystemSetting.h"
#include "GUISkyBlockTaskRewardItem.h"
#include "UI/GUILayout.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "Util/LanguageManager.h"
#include "GUI/GuiItemStack.h"
#include "Block/BlockUtil.h"
#include "Item/Item.h"

namespace BLOCKMAN
{
	GUISkyBlockTaskRewardItem::GUISkyBlockTaskRewardItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SkyBlockTaskRewardItem.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
	}

	void GUISkyBlockTaskRewardItem::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISkyBlockTaskRewardItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUISkyBlockTaskRewardItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUISkyBlockTaskRewardItem::InitReward(const SkyBlockTaskReward reward)
	{
		auto icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		auto value = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		auto itemStackIcon = dynamic_cast<GuiItemStack*>(m_window->GetChildByIndex(3));
		
		int cur_blockId = reward.RewardId;

		if (BlockUtil::isValidBlockId(cur_blockId))
		{
			Block* pBlock = BlockManager::sBlocks[cur_blockId];
			if (pBlock != NULL)
			{
				if (!BlockUtil::isBasicBlock(cur_blockId))
				{
					cur_blockId = cur_blockId + Item::EC_BLOCK_TO_ITEM_OFFSET;
				}
			}
		}

		ItemStackPtr itemStack = LORD::make_shared<ItemStack>(cur_blockId, 1, reward.Meta);
		if (reward.Icon.length() > 2)
		{
			icon->SetVisible(true);
			itemStackIcon->SetVisible(false);
			GuiItemStack::showItemIcon(icon, cur_blockId, reward.Meta, reward.Icon);
		}
		else
		{
			icon->SetVisible(false);
			if (itemStack)
			{
				itemStackIcon->setItemStack(itemStack);
				itemStackIcon->SetVisible(true);
			}
		}
		

		value->SetText(StringUtil::Format("x%d", reward.Num).c_str());
	}
}
