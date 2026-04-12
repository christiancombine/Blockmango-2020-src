#include "GUIPixelGunGiftBagItem.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Network/ClientNetwork.h"
#include "Audio/SoundSystem.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	GUIPixelGunGiftBagItem::GUIPixelGunGiftBagItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("PixelGunGiftBagItem.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
	}

	void GUIPixelGunGiftBagItem::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIPixelGunGiftBagItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIPixelGunGiftBagItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIPixelGunGiftBagItem::InitGift(GiftBag * gift)
	{
		auto bg = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		auto question = dynamic_cast<GUIButton*>(bg->GetChildByIndex(2));
		auto buy = dynamic_cast<GUIButton*>(bg->GetChildByIndex(4));
		auto image = dynamic_cast<GUIButton*>(bg->GetChildByIndex(1));

		buy->subscribeEvent(EventButtonClick, std::bind(&GUIPixelGunGiftBagItem::OnBuyButtonClick, this, std::placeholders::_1));
		question->subscribeEvent(EventButtonClick, std::bind(&GUIPixelGunGiftBagItem::OnQuestionButtonClick, this, std::placeholders::_1));
		image->subscribeEvent(EventButtonClick, std::bind(&GUIPixelGunGiftBagItem::OnQuestionButtonClick, this, std::placeholders::_1));
		InitGiftItem(gift);
	}

	void GUIPixelGunGiftBagItem::InitGiftItem(GiftBag * gift)
	{
		m_gift_id = gift->Id;

		auto bg = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		bg->SetImage(gift->BgImage.c_str());

		auto name = dynamic_cast<GUIStaticText*>(bg->GetChildByIndex(0));
		name->SetText(LanguageManager::Instance()->getString(gift->Name).c_str());

		auto image = dynamic_cast<GUIButton*>(bg->GetChildByIndex(1));
		image->SetNormalImage(gift->Image.c_str());
		image->SetPushedImage(gift->Image.c_str());

		auto tag_icon = dynamic_cast<GUIStaticImage*>(bg->GetChildByIndex(6));
		tag_icon->SetImage(gift->TagIcon.c_str());

		auto desc = dynamic_cast<GUIStaticText*>(bg->GetChildByIndex(3));
		desc->SetText(LanguageManager::Instance()->getString(gift->Describe).c_str());

		auto buy = dynamic_cast<GUIButton*>(bg->GetChildByIndex(4));
		if (gift->Status == GIFT_STATUS_UNBUY)
		{
			buy->SetNormalImage("set:pixel_gun_gift_bag_store.json image:btn_buy");
			buy->SetPushedImage("set:pixel_gun_gift_bag_store.json image:btn_buy");
			buy->SetTouchable(true);
		}
		else
		{
			buy->SetNormalImage("set:pixel_gun_task.json image:btn_finished");
			buy->SetPushedImage("set:pixel_gun_task.json image:btn_finished");
			buy->SetTouchable(false);
		}

		auto price = dynamic_cast<GUILayout*>(buy->GetChildByIndex(0));
		auto pristine = dynamic_cast<GUILayout*>(buy->GetChildByIndex(1));

		String moneyIcon = UICommon::getCurrencyIconByType(gift->MoneyType);

		auto price_icon = dynamic_cast<GUIStaticImage*>(price->GetChildByIndex(0));
		price_icon->SetImage(moneyIcon.c_str());

		auto price_text = dynamic_cast<GUIStaticText*>(price->GetChildByIndex(1));
		String priceText = StringUtil::Format("%d", gift->Price * gift->Discount / 100);
		price_text->SetText(priceText.c_str());

		auto pristine_icon = dynamic_cast<GUIStaticImage*>(pristine->GetChildByIndex(0));
		pristine_icon->SetImage(moneyIcon.c_str());

		auto pristine_text = dynamic_cast<GUIStaticText*>(pristine->GetChildByIndex(1));
		pristine_text->SetText(StringUtil::Format("%d", gift->Price).c_str());

		auto percentage = dynamic_cast<GUIStaticText*>(bg->GetChildByIndex(5));
		String percentageText = StringUtil::Format("%s:-%d%%", LanguageManager::Instance()->getString("pixel_gun_gift_discount").c_str(), 100 - gift->Discount);
		percentage->SetText(percentageText.c_str());

	}

	bool GUIPixelGunGiftBagItem::OnBuyButtonClick(const EventArgs & events)
	{
		SoundSystem::Instance()->playEffectByType(ST_Click);
		auto bg = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		auto buy = dynamic_cast<GUIButton*>(bg->GetChildByIndex(4));

		auto gift = PixelGunGiftBagSetting::findGiftBagById(m_gift_id);
		if (gift == NULL)
			return false;
		if (gift->Status == GIFT_STATUS_BOUGHT)
			return false;
		int needMoney = PixelGunGiftBagSetting::checkEnoughMoney(gift);
		if (needMoney > 0)
		{
			ShowLackOfMoneyTipDialogEvent::emit(needMoney, (CurrencyType)gift->MoneyType);
			return false;
		}
		UICommon::checkMoney(gift->Price * gift->Discount / 100, gift->MoneyType, [=](bool isSuccess) {
			if (isSuccess)
			{
				ClientNetwork::Instance()->getCommonSender()->sendPixelGunBuyGiftBag(m_gift_id);
			}
		});
		return true;
	}

	bool GUIPixelGunGiftBagItem::OnQuestionButtonClick(const EventArgs & events)
	{
		SoundSystem::Instance()->playEffectByType(ST_Click);
		ShowGiftBagDetailEvent::emit(5, m_gift_id);
		return true;
	}
}
