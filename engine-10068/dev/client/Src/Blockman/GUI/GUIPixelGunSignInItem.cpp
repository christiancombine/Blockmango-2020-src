#include "GUIPixelGunSignInItem.h"
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

namespace BLOCKMAN
{
	GUIPixelGunSignInItem::GUIPixelGunSignInItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("PixelGunSignInItem.json"));
		m_Bg = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(0));
		m_Bg->subscribeEvent(EventButtonClick, std::bind(&GUIPixelGunSignInItem::onItemClick, this, std::placeholders::_1));

		renameComponents(m_window);
		initView();
		AddChildWindow(m_window);
	}

	void GUIPixelGunSignInItem::setSignInItem(SignInItem * data)
	{
		m_data = data;
		auto setting = SignInSetting::getSettingByOrder(data->order);
		auto Title = dynamic_cast<GUIStaticText*>(m_Bg->GetChildByIndex(0));
		auto Image = dynamic_cast<GUIStaticImage*>(m_Bg->GetChildByIndex(1));
		auto NumText = dynamic_cast<GUIStaticText*>(m_Bg->GetChildByIndex(2));
		auto Received = dynamic_cast<GUIStaticImage*>(m_Bg->GetChildByIndex(3));
		auto Effect = dynamic_cast<GUIStaticImage*>(m_Bg->GetChildByIndex(4));

		m_Bg->SetNormalImage(setting->background.c_str());
		m_Bg->SetPushedImage(setting->background.c_str());
		Title->SetText(LanguageManager::Instance()->getString(setting->title).c_str());
		Image->SetImage(setting->image.c_str());
		if (data->side == 2)
		{
			Image->SetSize({ {0.0f, 400.0f}, {0.0f, 171.0f} });
		}
		if (data->status == ItemStatus::ABLE_RECEIVED)
		{
			Effect->SetVisible(true);
			Effect->SetEffectName(setting->effect.c_str());
			if (data->side == 2)
			{
				Effect->SetSize({ {0.7f, 0.0f}, {1.8f, 0.0f} });
			}
		}
		else
		{
			Effect->SetVisible(false);
		}
		if (data->status != ItemStatus::HAVE_RECEIVED)
		{
			Received->SetVisible(false);
		}
		else
		{
			Image->SetEnabled(false);
			m_Bg->SetEnabled(false);
			Received->SetVisible(true);
		}
		auto reward = RewardSetting::findRewardById(m_data->rewardId);
		if (!reward)
		{
			return;
		}
		NumText->SetText(getString(reward->Name));
	}

	void GUIPixelGunSignInItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIPixelGunSignInItem::initView()
	{
		
	}
	bool GUIPixelGunSignInItem::onItemClick(const EventArgs & events)
	{
		SoundSystem::Instance()->playEffectByType(ST_Click);
		if (m_data->status == ItemStatus::ABLE_RECEIVED)
		{
			ClientNetwork::Instance()->getCommonSender()->sendPixelGunReceiveSignIn(m_data->id);
			m_data->status = ItemStatus::HAVE_RECEIVED;
			setSignInItem(m_data);
		}
		else
		{
			if (m_data->status == ItemStatus::UN_RECEIVED)
			{
				ShowChestRewardDetailEvent::emit(m_data->rewardId);
			}
		}
		return true;
	}
}
