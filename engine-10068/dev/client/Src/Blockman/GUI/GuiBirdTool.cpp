#include "GuiBirdTool.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	
	void GuiBirdTool::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdTool::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdTool::GuiBirdTool(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdTool.json"));
		renameComponents(m_window);

		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0)->GetChildByIndex(0));
		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0)->GetChildByIndex(1)->GetChildByIndex(0));
		m_des = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0)->GetChildByIndex(2));
		m_use = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(0)->GetChildByIndex(3));
		m_collect = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1)->GetChildByIndex(0)->GetChildByIndex(0));
		m_pack = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1)->GetChildByIndex(1)->GetChildByIndex(0));
		m_wing = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1)->GetChildByIndex(2)->GetChildByIndex(0));
		m_use->SetText(getString("gui_bird_pack_use_tool"));

		m_collect->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdTool::onClick, this, std::placeholders::_1, ViewId::GATHER));
		m_pack->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdTool::onClick, this, std::placeholders::_1, ViewId::PACK));
		m_wing->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdTool::onClick, this, std::placeholders::_1, ViewId::WING));
		m_use->subscribeEvent(EventButtonClick, std::bind(&GuiBirdTool::onClick, this, std::placeholders::_1, ViewId::USE));

		AddWindowToDrawList(*m_window);
	}

	void GuiBirdTool::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

		m_use->SetVisible(m_tool.status != 3);

	}

	void GuiBirdTool::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBirdTool::setToolInfo(BirdStoreItem tool)
	{
		m_tool = tool;
		m_name->SetText(getString(tool.name));
		m_image->SetImage(tool.iconNew.c_str());
		m_des->SetText(getString(tool.desc));
	}

	void GuiBirdTool::setCurUse(BirdStoreItem tool)
	{
		switch (tool.type)
		{
		case 1:
			m_curCollect = tool;
			m_collect->SetImage(tool.iconNew.c_str());
			break;
		case 2:
			m_curPack = tool;
			m_pack->SetImage(tool.iconNew.c_str());
			break;
		case 3:
			break;
		case 4:
			m_curWing = tool;
			m_wing->SetImage(tool.iconNew.c_str());
			break;
		default:
			break;
		}
	}

	bool GuiBirdTool::onClick(const EventArgs & args, ViewId viewId)
	{
 		switch (viewId)
 		{
 		case ViewId::PACK:
			if (m_curPack.id > 0)
			{
				ShowBirdToolInfoClickEvent::emit(m_curPack.id);
			}
 			break;
 		case ViewId::GATHER:
			if (m_curCollect.id > 0)
			{
				ShowBirdToolInfoClickEvent::emit(m_curCollect.id);
			}
 			break;
 		case ViewId::WING:
			if (m_curWing.id > 0)
			{
				ShowBirdToolInfoClickEvent::emit(m_curWing.id);
			}
 			break;
 		case ViewId::CAP:
 			break;
 		case ViewId::CLOTHES:
 			break;
 		case ViewId::USE:
			if (m_tool.status == 2)
			{
				ClientNetwork::Instance()->getSender()->sendBirdStoreOperation(m_tool.storeId, m_tool.id, 2);
			}
 			break;
 		}
		return true;
	}

}