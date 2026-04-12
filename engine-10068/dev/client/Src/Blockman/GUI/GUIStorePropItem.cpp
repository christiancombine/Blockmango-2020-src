#include "GUIStorePropItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{
	GUIStorePropItem::GUIStorePropItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("StorePropItem.json"));
		renameComponents(m_window);
		initView();
		AddChildWindow(m_window);
		SetTouchable(false);
	}

	void GUIStorePropItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIStorePropItem::initView()
	{
		m_status_box = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_use_line = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4));
		m_hint_new = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(5));
		m_hint_new->GetChildByIndex(0)->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_GUN_STORE_NEW).c_str());
		m_price = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		m_price_icon = dynamic_cast<GUIStaticImage*>(m_price->GetChildByIndex(0));
		m_price_text = dynamic_cast<GUIStaticText*>(m_price->GetChildByIndex(1));
		m_lock = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(7));
		m_lock_level = dynamic_cast<GUIButton*>(m_lock->GetChildByIndex(0));
		m_block_check = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		auto ItemClickBox = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		ItemClickBox->subscribeEvent(EventButtonClick, std::bind(&GUIStorePropItem::OnItemChick, this, std::placeholders::_1));
	}

	void GUIStorePropItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_data == NULL)
			return;
		if (m_data->HasChange)
		{
			InitStoreProp(m_data);
		}
	}

	void GUIStorePropItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIStorePropItem::InitStoreProp(StoreProp * prop)
	{
		m_data = prop;
		m_data->HasChange = false;
		if (prop->IsSelected)
		{
			m_status_box->SetImage("set:gun_store.json image:border_use");
			SetBlockCheck();
		}
		else
		{
			if (m_block_check->IsVisible())
			{
				m_block_check->SetVisible(false);
			}
			if (prop->Status == STATUS_LOCK || prop->Status == STATUS_NEW || prop->Status == STATUS_NOT_GET)
				m_status_box->SetImage("set:gun_store.json image:border_nor");
			else
				m_status_box->SetImage("set:gun_store.json image:border_has_get");
		}

		m_image->SetImage(prop->Status == STATUS_LOCK ? prop->LockImage.c_str() : prop->Image.c_str());
		m_use_line->SetVisible(prop->Status == STATUS_HAS_USE);
		m_use_line->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_BTN_EQUIPPED).c_str());
		m_hint_new->SetVisible(prop->Status == STATUS_NEW);
		m_price->SetVisible(prop->Status != STATUS_HAS_GET && prop->Status != STATUS_HAS_USE && prop->Status != STATUS_LOCK);
		m_price_icon->SetImage(GunStoreSetting::getPriceIcon(prop->PriceType).c_str());
		String price = GunStoreSetting::getPriceText(prop->PriceType, prop->Chip, prop->Price, prop->Status).c_str();
		m_price_text->SetText(price.c_str());
		m_price_icon->SetXPosition({ 0.0f, -(14.0f + (price.length() - 11) * 8.0f) });
		m_lock->SetVisible(prop->Status == STATUS_LOCK);
		m_lock_level->SetText(StringUtil::ToString(prop->UnlockLevel).c_str());
		auto RedPoint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(8));
		RedPoint->SetVisible(m_data->UpgradeComplete);
	}

	int GUIStorePropItem::GetPropId()
	{
		int result = -1;
		if (m_data != NULL)
		{
			result = m_data->Id;
		}
		return result;
	}

	void GUIStorePropItem::SetBlockCheck()
	{
		if (m_data->Type == PROP_TYPE_BLOCK && m_data->Subtype == PROP_SUBTYPE_BLOCK_MODULE)
		{
			m_block_check->SetVisible(true);
		}
	}

	bool GUIStorePropItem::OnItemChick(const EventArgs & events)
	{
		if (m_data == NULL)
			return false;
		if (m_data->Status == STATUS_LOCK)
		{
			if (!m_data->LockTip.empty())
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, m_data->LockTip.c_str());
			}
			return false;
		}
		if (m_data->Status == STATUS_NEW)
		{
			m_data->Status = STATUS_NOT_GET;
			m_data->HasChange = true;
			GunStoreSetting::useGunStoreFunc(StoreAction::FIRST_SHOW_PROP, m_data->Id);
		}

		auto RedPoint = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(8));
		if (m_data->UpgradeComplete && RedPoint->IsVisible())
		{
			RedPoint->SetVisible(false);
			UpgradeCompleteItemSelectEvent::emit(m_data->Id);
		}

		SetBlockCheck();
		GunStorePropItemClickEvent::emit(m_data->Id);
		return true;
	}

}
