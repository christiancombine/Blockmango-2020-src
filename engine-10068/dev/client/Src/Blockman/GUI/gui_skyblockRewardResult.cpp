#include "gui_skyblockRewardResult.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Setting/SkyblockSignInSetting.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiDef.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "GUI/GuiItemStack.h"
#include "Item/Item.h"

namespace BLOCKMAN
{
	gui_skyblockRewardResult::gui_skyblockRewardResult() :
		gui_layout("SkyBlockRewardResult.json")
	{
	}

	gui_skyblockRewardResult::~gui_skyblockRewardResult()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_skyblockRewardResult::onLoad()
	{
		GUILayout* SkyBlockRewardResult = getWindow<GUILayout>("SkyBlockRewardResult");
		SkyBlockRewardResult->subscribeEvent(EventWindowTouchUp, std::bind(&gui_skyblockRewardResult::onContentClick, this, std::placeholders::_1));

		setText("SkyBlockRewardResult-Title", "gui_sky_block_pri_reward_text");

		auto Continue = getWindow<GUIButton>("SkyBlockRewardResult-Continue");
		Continue->subscribeEvent(EventButtonClick, std::bind(&gui_skyblockRewardResult::onContinueClick, this, std::placeholders::_1));
		Continue->SetText(getString("gui_dialog_tip_btn_sure"));

		m_image = getWindow<GUIStaticImage>("SkyBlockRewardResult-Image");
		m_name = getWindow<GUIStaticText>("SkyBlockRewardResult-Name");

	}

	bool gui_skyblockRewardResult::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain);
	}

	bool gui_skyblockRewardResult::onContentClick(const EventArgs & events)
	{
		getParent()->hideSkyBlockRewardResult();
		return true;
	}

	bool gui_skyblockRewardResult::onContinueClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		getParent()->hideSkyBlockRewardResult();
		return true;
	}

	void gui_skyblockRewardResult::onGetCommonData(String key, String data)
	{
		if (key == "SignInRewardResult")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int id = builder.getIntParam("id");
			onShowSkyBlockSignInRewards(id);
			return;
		}
	}

	bool gui_skyblockRewardResult::onShowSkyBlockSignInRewards(int id)
	{
		SkyBlockSignInInfo data;
		if (SkyBlockSignInSetting::Instance()->getSettingByOrder(data, id))
		{
			if (data.image.length() > 2)
			{
				m_image->SetImage(data.image.c_str());
			}

			else
			{
				GuiItemStack::showItemIcon(m_image, data.rewardId, data.meta);
			}

			String name = "";
			if (data.image.length() > 2)
			{
				name = LanguageManager::Instance()->getString(data.rewardName).c_str();
			}
			else
			{
				Item* pItem3 = Item::getItemById(data.rewardId);
				if (pItem3)
				{
					ItemStackPtr itemStack = LORD::make_shared<ItemStack>(data.rewardId, 1, data.meta);
					name = LanguageManager::Instance()->getItemName(pItem3->getUnlocalizedName(itemStack).c_str()).c_str();
				}
			}

			if (data.num > 1)
			{
				String num_text = "";
				num_text = StringUtil::Format("X%d", data.num).c_str();
				name = name + num_text;
			}

			m_name->SetText(name.c_str());
			getParent()->showSkyBlockRewardResult();
			return true;
		}

		getParent()->hideSkyBlockRewardResult();
		return false;
	}
}
