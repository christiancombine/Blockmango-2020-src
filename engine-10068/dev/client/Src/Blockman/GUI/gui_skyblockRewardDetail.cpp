#include "gui_skyblockRewardDetail.h"
#include "UI/GUILayout.h"
#include "GUI/GuiItemStack.h"
#include "Item/Item.h"

namespace BLOCKMAN
{
	gui_skyblockRewardDetail::gui_skyblockRewardDetail() :
		gui_layout("SkyBlockRewardDetail.json")
	{
	}

	gui_skyblockRewardDetail::~gui_skyblockRewardDetail()
	{
	}

	void gui_skyblockRewardDetail::onLoad()
	{
		GUILayout* RewardDetail = getWindow<GUILayout>("SkyBlockRewardDetail");
		RewardDetail->subscribeEvent(EventWindowTouchUp, std::bind(&gui_skyblockRewardDetail::onContentClick, this, std::placeholders::_1));

		m_image = getWindow<GUIStaticImage>("SkyBlockRewardDetail-Image");
		m_name = getWindow<GUIStaticText>("SkyBlockRewardDetail-Name");
		m_desc = getWindow<GUIStaticText>("SkyBlockRewardDetail-Desc");
	}

	bool gui_skyblockRewardDetail::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain);
	}

	void gui_skyblockRewardDetail::ShowRewardDetail(int id)
	{
		SkyBlockSignInInfo reward;
		if (SkyBlockSignInSetting::Instance()->getSettingByOrder(reward, id))
		{
			if (reward.image.length() > 2)
			{
				m_image->SetImage(reward.image.c_str());
			}
			else
			{
				GuiItemStack::showItemIcon(m_image, reward.rewardId, reward.meta);
			}

			String name = "";
			if (reward.rewardName.length() > 2)
			{
				name = LanguageManager::Instance()->getString(reward.rewardName).c_str();
			}
			else
			{
				Item* pItem3 = Item::getItemById(reward.rewardId);
				if (pItem3)
				{
					ItemStackPtr itemStack = LORD::make_shared<ItemStack>(reward.rewardId, 1, reward.meta);
					name = LanguageManager::Instance()->getItemName(pItem3->getUnlocalizedName(itemStack).c_str()).c_str();
				}
			}
			m_name->SetText(name.c_str());
			m_desc->SetText(getString(reward.desc));
		}
	}

	bool gui_skyblockRewardDetail::onContentClick(const EventArgs & events)
	{
		getParent()->hideSkyBlockRewardDetail();
		return true;
	}

}
