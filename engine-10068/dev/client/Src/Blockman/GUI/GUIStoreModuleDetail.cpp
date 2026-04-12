#include "GUIStoreModuleDetail.h"
#include "GUI/GUIStoreAttributeItem.h"
#include "GUI/GUIStorePropertyItem.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	GUIStoreModuleDetail::GUIStoreModuleDetail() :
		gui_layout("StoreModuleDetail.json")
	{
	}

	GUIStoreModuleDetail::~GUIStoreModuleDetail()
	{
	}

	void GUIStoreModuleDetail::onLoad()
	{
		m_name = getWindow<GUIStaticText>("StoreModuleDetail-Name");
		m_rarity = getWindow<GUIStaticText>("StoreModuleDetail-Rarity");
		m_model = getWindow<GuiActorWindow>("StoreModuleDetail-Model");
		m_attribute = getWindow<GUILayout>("StoreModuleDetail-Attribute");
		m_property = getWindow<GUILayout>("StoreModuleDetail-Property");
		m_use = getWindow<GUIButton>("StoreModuleDetail-Use");
		m_buy = getWindow<GUIButton>("StoreModuleDetail-Buy");
		m_price_type = getWindow<GUIStaticImage>("StoreModuleDetail-Price-Icon");
		m_price = getWindow<GUILayout>("StoreModuleDetail-Price");
		m_price_text = getWindow<GUIStaticText>("StoreModuleDetail-Price-Value");

		auto Close = getWindow<GUIButton>("StoreModuleDetail-Close");
		Close->subscribeEvent(EventButtonClick, std::bind(&GUIStoreModuleDetail::onCloseClick, this, std::placeholders::_1));

		m_buy->SetText(getString(LanguageKey::SHOP_BTN_BUY));

		m_use->subscribeEvent(EventButtonClick, std::bind(&GUIStoreModuleDetail::onUseClick, this, std::placeholders::_1));
		m_buy->subscribeEvent(EventButtonClick, std::bind(&GUIStoreModuleDetail::onBuyClick, this, std::placeholders::_1));
	}

	void GUIStoreModuleDetail::SetStoreModule(StoreBlockModule * module)
	{
		m_data = module;
		m_name->SetText(getString(module->Name));
		m_rarity->SetText(getString(module->Rarity));
		m_model->SetActor(module->Actor.c_str(), "idle");
		m_property->CleanupChildren();
		m_attribute->CleanupChildren();
		m_buy->SetEnabled(true);
		auto properties = GunStoreSetting::findStorePropertyByIds(module->PropertyIds);
		static unsigned module_property_index = 0;
		int index = 0;
		float PropertyHeight = 34.0f;
		for (auto property : properties)
		{
			index++;
			module_property_index++;
			GUIString PropertyName = StringUtil::Format("GUIStoreModuleDetail-Property-%d", module_property_index).c_str();
			GUIStorePropertyItem* pPropertyItem = (GUIStorePropertyItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_PROPERTY_ITEM, PropertyName);
			pPropertyItem->SetWidth({ 1.0f, 0.0f });
			pPropertyItem->SetHeight({ 0.0f, PropertyHeight });
			pPropertyItem->SetYPosition({ 0.0f, (index - 1) * PropertyHeight });
			pPropertyItem->SetStoreProperty(property, true);
			m_property->AddChildWindow(pPropertyItem);
		}
		float AttributeHeight = 34.0f;
		auto attriutes = GunStoreSetting::findStoreAttributeByIds(module->AttributeIds);
		static unsigned module_attribute_index = 0;
		index = 0;
		for (auto attriute : attriutes)
		{
			index++;
			module_attribute_index++;
			GUIString AttributeName = StringUtil::Format("GUIStoreModuleDetail-Attribute-%d", module_attribute_index).c_str();
			GUIStoreAttributeItem* pAttributeItem = (GUIStoreAttributeItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_ATTRIBUTE_ITEM, AttributeName);
			pAttributeItem->SetWidth({ 1.0f, 0.0f });
			pAttributeItem->SetHeight({ 0.0f, AttributeHeight });
			pAttributeItem->SetYPosition({ 0.0f, (index - 1) * AttributeHeight });
			pAttributeItem->SetStoreAttribute(attriute, 0, true);
			m_attribute->AddChildWindow(pAttributeItem);
		}
		switch (m_data->Status)
		{
		case STATUS_NEW:
		case STATUS_NOT_GET:
			m_use->SetVisible(false);
			m_buy->SetVisible(true);
			m_price->SetVisible(true);
			m_price_type->SetImage(GunStoreSetting::getPriceIcon(module->PriceType).c_str());
			m_price_text->SetText(GunStoreSetting::getPriceText(module->PriceType, 0, module->Price).c_str());
			break;
		case STATUS_HAS_GET:
			m_price->SetVisible(false);
		case STATUS_HAS_USE:
			m_use->SetVisible(true);
			m_buy->SetVisible(false);
			m_price->SetVisible(false);
			m_use->SetNormalImage(module->Status == STATUS_HAS_USE ? "set:gun_store.json image:btn_purple" : "set:gun_store.json image:btn_green");
			m_use->SetPushedImage(module->Status == STATUS_HAS_USE ? "set:gun_store.json image:btn_purple" : "set:gun_store.json image:btn_green");
			m_use->SetEnabled(true);
			m_use->SetText(getString(module->Status == STATUS_HAS_USE ? LanguageKey::GUI_BTN_UNLOAD : LanguageKey::INVENTORY_PANEL_EQUIPMENT));
			break;
		default:
			break;
		}
	}

	bool GUIStoreModuleDetail::onCloseClick(const EventArgs & events)
	{
		hide();
		return true;
	}

	bool GUIStoreModuleDetail::onUseClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_data == NULL)
			return false;
		if (m_data->Status != STATUS_HAS_GET && m_data->Status != STATUS_HAS_USE)
			return false;
		if (m_data->Status == STATUS_HAS_GET)
			GunStoreSetting::useGunStoreFunc(StoreAction::USE_MODULE, m_data->Id);
		else
			GunStoreSetting::useGunStoreFunc(StoreAction::UNLOAD_MODULE, m_data->Id);
		return true;
	}

	bool GUIStoreModuleDetail::onBuyClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_data == NULL)
			return false;
		if (m_data->Status == STATUS_LOCK)
			return false;
		bool hasMoney = GunStoreSetting::checkHasMoney(m_data->PriceType, 0, m_data->Price);
		if (!hasMoney)
		{
			ShowLackOfMoneyTipDialogEvent::emit(m_data->Price, m_data->PriceType);
			return false;
		}
		m_buy->SetEnabled(false);
		GunStoreSetting::useGunStoreFunc(StoreAction::BUY_MODULE, m_data->Id);
		return true;
	}

}
