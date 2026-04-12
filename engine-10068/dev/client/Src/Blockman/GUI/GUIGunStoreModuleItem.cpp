#include "GUIGunStoreModuleItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "GUI/GUIStoreGunItem.h"
#include "GUI/GUIStorePropItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{
	GUIGunStoreModuleItem::GUIGunStoreModuleItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("StoreModuleItem.json"));
		renameComponents(m_window);
		initView();
		AddChildWindow(m_window);
	}

	void GUIGunStoreModuleItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIGunStoreModuleItem::initView()
	{
		m_status_box = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_use_line = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_hint_new = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(4));
		m_hint_new->GetChildByIndex(0)->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_GUN_STORE_NEW).c_str());
		m_price = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(5));
		m_price_icon = dynamic_cast<GUIStaticImage*>(m_price->GetChildByIndex(0));
		m_price_text = dynamic_cast<GUIStaticText*>(m_price->GetChildByIndex(1));
		m_lock = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		m_lock_level = dynamic_cast<GUIButton*>(m_lock->GetChildByIndex(0));
		m_tag = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(7));
		auto ItemClickBox = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2));
		ItemClickBox->subscribeEvent(EventButtonClick, std::bind(&GUIGunStoreModuleItem::OnItemChick, this, std::placeholders::_1));
	}

	void GUIGunStoreModuleItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x }, { 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_data == NULL)
			return;
		if (m_data->HasChange)
		{
			InitModuleItem(m_data);
		}
	}

	void GUIGunStoreModuleItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}
	void GUIGunStoreModuleItem::InitModuleItem(StoreBlockModule * module)
	{
		m_data = module;
		m_data->HasChange = false;

		if (module->IsSelected)
			m_status_box->SetImage("set:gun_store.json image:border_use");
		else
			if (module->Status == STATUS_LOCK || module->Status == STATUS_NEW || module->Status == STATUS_NOT_GET)
				m_status_box->SetImage("set:gun_store.json image:border_nor");
			else
				m_status_box->SetImage("set:gun_store.json image:border_has_get");

		m_image->SetImage(module->Status == STATUS_LOCK ? module->LockImage.c_str() : module->Image.c_str());
		m_use_line->SetVisible(module->Status == STATUS_HAS_USE);
		m_use_line->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_BTN_EQUIPPED).c_str());
		m_hint_new->SetVisible(module->Status == STATUS_NEW);
		m_price->SetVisible(module->Status != STATUS_HAS_GET && module->Status != STATUS_HAS_USE && module->Status != STATUS_LOCK);
		m_price_icon->SetImage(GunStoreSetting::getPriceIcon(module->PriceType).c_str());
		String price = GunStoreSetting::getPriceText(module->PriceType, 0, module->Price, module->Status).c_str();
		m_price_text->SetText(price.c_str());
		m_price_icon->SetXPosition({ 0, -(14.0f + (price.length() - 11) * 8.0f) });
		m_lock->SetVisible(module->Status == STATUS_LOCK);
		m_lock_level->SetText(StringUtil::ToString(module->UnlockLevel).c_str());
		m_tag->SetVisible((int)module->Tag > 0 && module->Tag < MODULE_TAG_NONE);
		m_tag->SetImage(StringUtil::Format("set:gun_store.json image:module_tag_%d", (int)module->Tag).c_str());

	}
	bool GUIGunStoreModuleItem::OnItemChick(const EventArgs & events)
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
			GunStoreSetting::useGunStoreFunc(StoreAction::FIRST_SHOW_MODULE, m_data->Id);
		}
		GunStoreModuleItemClickEvent::emit(m_data->Id);
		return true;
	}
}
