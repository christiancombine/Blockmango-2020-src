#include "GuiBirdPackAndFuseItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Inventory/Wallet.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Util/RedDotManager.h"
#include "GUI/RootGuiLayout.h"
#include "Game/BirdSimulator.h"

namespace BLOCKMAN
{
	
	void GuiBirdPackAndFuseItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdPackAndFuseItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdPackAndFuseItem::GuiBirdPackAndFuseItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdPackAndFuseItem.json"));
		renameComponents(m_window);

		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_tick = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_greenDot = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_level = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		m_greenDot->SetVisible(false);
		m_tick->SetVisible(false);
		m_level->SetVisible(false);

		m_image->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdPackAndFuseItem::onClick, this, std::placeholders::_1));
		
		AddChildWindow(m_window);
	}

	GuiBirdPackAndFuseItem::~GuiBirdPackAndFuseItem()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GuiBirdPackAndFuseItem::UpdateSelf(ui32 nTimeElapse)
	{
 		const auto& windowSize = GetPixelSize();
 		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
 		BaseClass::UpdateSelf(nTimeElapse);
		changeImage();
		m_tick->SetVisible(m_isSelect && m_showType  == 2);
		m_greenDot->SetVisible(m_bird.isCarry || m_tool.status == 3);
	}
	
	void GuiBirdPackAndFuseItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBirdPackAndFuseItem::setBirdInfo(BirdInfo data)
	{
		m_bird = data;
		m_level->SetText(m_bird.id > 0 ? StringUtil::Format("Lv %d", data.level).c_str() : "");
		m_level->SetVisible(true);
		String imageName = StringUtil::Format("set:bird_icon.json image:body_%s", data.bodyId.c_str());
		m_image->SetImage(m_bird.id > 0 ? imageName.c_str() : "");
		m_image->SetTouchable(m_bird.id > 0);
	}

	void GuiBirdPackAndFuseItem::setSelect(i64 id)
	{
		switch (m_showType)
		{
		case 1:
			m_isSelect = (m_bird.id == id);
			break;
		case 2:
			if (m_bird.id == id)
			{
				m_isSelect = true;
			}
			break;
		case 3:		
			m_isSelect = (m_tool.id == id);
			break;
		}
	}

	void GuiBirdPackAndFuseItem::unSelect(i64 birdId)
	{
		if (m_bird.id == birdId)
		{
			m_isSelect = false;
		}
	}

	bool GuiBirdPackAndFuseItem::onClick(const EventArgs& args)
	{
		if (m_bird.id == 0 && m_showType != 3)
		{
			return false;
		}

		switch (m_showType)
		{
		case 1:
			ShowBirdInfoClickEvent::emit(m_bird.id);
			break;
		case 2:
			if (m_isSelect)
			{
				unSelect(m_bird.id);
				BirdRemoveFuseItemEvent::emit(m_bird.id);
			}
			else
			{
				BirdFusePutItemEvent::emit(m_bird.id, StringUtil::Format("set:bird_icon.json image:body_%s", m_bird.bodyId.c_str()), 
					StringUtil::Format("set:bird_pack.json image:item_%s_nor", StringUtil::ToString(m_bird.quality).c_str()));
			}
			break;
		case 3:
			ShowBirdToolInfoClickEvent::emit(m_tool.id);
			break;
		}
		return true;
	}

	void GuiBirdPackAndFuseItem::changeImage()
	{
		switch (m_bird.quality)
		{
		case 1:
			m_window->SetBackImage(m_isSelect && m_showType == 1 ? "set:bird_pack.json image:item_1_pre" : "set:bird_pack.json image:item_1_nor");
			break;
		case 2:
			m_window->SetBackImage(m_isSelect && m_showType == 1 ? "set:bird_pack.json image:item_2_pre" : "set:bird_pack.json image:item_2_nor");
			break;
		case 3:
			m_window->SetBackImage(m_isSelect && m_showType == 1 ? "set:bird_pack.json image:item_3_pre" : "set:bird_pack.json image:item_3_nor");
			break;
		case 4:
			m_window->SetBackImage(m_isSelect && m_showType == 1 ? "set:bird_pack.json image:item_4_pre" : "set:bird_pack.json image:item_4_nor");
			break;
		default:
			m_window->SetBackImage("set:bird_pack.json image:item");
			break;
		}

		if (m_showType == 3)
		{
			m_window->SetBackImage(m_isSelect ? "set:bird_pack.json image:item_1_pre" : "set:bird_pack.json image:item_1_nor");
		}
	}

	void GuiBirdPackAndFuseItem::setToolInfo(BirdStoreItem data)
	{
		m_tool = data;
		m_image->SetImage(m_tool.iconNew.c_str());
		m_image->SetTouchable(true);
	}

}