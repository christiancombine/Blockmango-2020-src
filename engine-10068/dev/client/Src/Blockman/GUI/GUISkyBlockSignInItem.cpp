#include "GUISkyBlockSignInItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Network/ClientNetwork.h"
#include "Audio/SoundSystem.h"
#include "Audio/SoundDef.h"
#include "GUI/GuiItemStack.h"
#include "Item/Item.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/InventoryPlayer.h"

namespace BLOCKMAN
{
	GUISkyBlockSignInItem::GUISkyBlockSignInItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SkyBlockSignInItem.json"));
		m_Bg = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(0));
		m_Bg->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockSignInItem::onItemClick, this, std::placeholders::_1));

		renameComponents(m_window);
		initView();
		AddChildWindow(m_window);
	}

	void GUISkyBlockSignInItem::setSignInItem(SkyBlockSignInInfo data)
	{
		m_data = data;
		auto NumText = dynamic_cast<GUIStaticText*>(m_Bg->GetChildByIndex(3));
		m_small_title_panle = dynamic_cast<GUIStaticImage*>(m_Bg->GetChildByIndex(0)->GetChildByIndex(0));
		m_big_title_panle = dynamic_cast<GUIStaticText*>(m_Bg->GetChildByIndex(0)->GetChildByIndex(1));
		auto Item_bg = dynamic_cast<GUIStaticImage*>(m_Bg->GetChildByIndex(1));
		auto Image = dynamic_cast<GUIStaticImage*>(m_Bg->GetChildByIndex(2));
		auto Received = dynamic_cast<GUIStaticImage*>(m_Bg->GetChildByIndex(4));

		Item_bg->SetImage(m_data.background.c_str());
		auto small_title = dynamic_cast<GUIStaticText*>(m_small_title_panle->GetChildByIndex(0));
		small_title->SetText(LanguageManager::Instance()->getString(m_data.title).c_str());
		m_big_title_panle->SetText(LanguageManager::Instance()->getString(m_data.title).c_str());

		if (m_data.image.length() > 2)
		{
			Image->SetImage(m_data.image.c_str());
		}
		else
		{
			GuiItemStack::showItemIcon(Image, m_data.rewardId, m_data.meta);
		}


		if (m_data.side == 1)
		{

			if (m_data.status == SkyBlockSignInStatus::SKYBLOCK_ABLE_RECEIVED_SIGN)
			{
				m_Bg->SetNormalImage("set:skyblock_sign_in.json image:item_select");


				m_Bg->SetPushedImage("set:skyblock_sign_in.json image:item_select");
			}
			else
			{
				m_Bg->SetNormalImage("set:skyblock_sign_in.json image:item_nor");
				m_Bg->SetPushedImage("set:skyblock_sign_in.json image:item_nor");
			}
			m_big_title_panle->SetVisible(false);
		}

		if (m_data.side == 2)
		{
			if (m_data.status == SkyBlockSignInStatus::SKYBLOCK_ABLE_RECEIVED_SIGN)
			{
				m_Bg->SetNormalImage("set:skyblock_sign_in.json image:big_select");
				m_Bg->SetPushedImage("set:skyblock_sign_in.json image:big_select");
			}
			else
			{
				m_Bg->SetNormalImage("set:skyblock_sign_in.json image:big_nor");
				m_Bg->SetPushedImage("set:skyblock_sign_in.json image:big_nor");
			}

			m_small_title_panle->SetVisible(false);

			NumText->SetYPosition({-0.12f, 0});
			m_Bg->SetArea(UDim(0, 0), UDim(0, -4), UDim(1.f, 0.0f), UDim(1, 7.0f));
			Image->SetSize({ {0.0f, 80.0f}, {0.0f, 80.0f} });
			Item_bg->SetSize({ {0.0f, 90.0f}, {0.0f, 90.0f} });
		}

		if (m_data.status != SkyBlockSignInStatus::SKYBLOCK_HAVE_RECEIVED_SIGN)
		{
			Received->SetVisible(false);
		}
		else
		{
			Received->SetVisible(true);
		}

		String name = "";
		if (m_data.image.length() > 2)
		{
			name = LanguageManager::Instance()->getString(m_data.rewardName).c_str();
		}
		else
		{
			Item* pItem3 = Item::getItemById(m_data.rewardId);
			if (pItem3)
			{
				ItemStackPtr itemStack = LORD::make_shared<ItemStack>(m_data.rewardId, 1, m_data.meta);
				name = LanguageManager::Instance()->getItemName(pItem3->getUnlocalizedName(itemStack).c_str()).c_str();
			}
		}
		if (m_data.num > 1)
		{
			String num_text = "";
			num_text = StringUtil::Format("X%d", m_data.num).c_str();
			name = name + num_text;
		}
		NumText->SetText(name.c_str());
	}

	void GUISkyBlockSignInItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISkyBlockSignInItem::initView()
	{

	}
	bool GUISkyBlockSignInItem::onItemClick(const EventArgs & events)
	{
		if (m_data.status == SkyBlockSignInStatus::SKYBLOCK_ABLE_RECEIVED_SIGN)
		{
			if (m_data.rewardType == SKYBLOCK_SIGNIN_ITEM)
			{
				if (!Blockman::Instance()->m_pPlayer->inventory->isCanAddItem(m_data.rewardId, m_data.meta, m_data.num))
				{
					ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_sign_in_iventor_is_full");
					return false;
				}
				ClientNetwork::Instance()->getCommonSender()->sendSkyBlockReceiveSignInReward(m_data.day);
				m_data.status = SkyBlockSignInStatus::SKYBLOCK_HAVE_RECEIVED_SIGN;
				setSignInItem(m_data);
			}
			else
			{
				ClientNetwork::Instance()->getCommonSender()->sendSkyBlockReceiveSignInReward(m_data.day);
				m_data.status = SkyBlockSignInStatus::SKYBLOCK_HAVE_RECEIVED_SIGN;
				setSignInItem(m_data);
			}
			return true;
		}
		ShowSkyBlockSignInRewardDetailEvent::emit(m_data.id);
		return true;
	}
}
