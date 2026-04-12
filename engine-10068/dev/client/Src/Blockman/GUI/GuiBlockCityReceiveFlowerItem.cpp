#include "GuiBlockCityReceiveFlowerItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientHttpRequest.h"

namespace BLOCKMAN
{
	
	void GuiBlockCityReceiveFlowerItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockCityReceiveFlowerItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockCityReceiveFlowerItem::GuiBlockCityReceiveFlowerItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockCityReceiveFlowerItem.json"));
		renameComponents(m_window);

		m_pic = dynamic_cast<GuiUrlImage*>(m_window->GetChildByIndex(0));
		m_pic->setDefaultImage("set:blockcity_get_flower.json image:head_icon");
		m_pic->setChildMaterial(GUIMaterialType_CULL_BACK);
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_btnTossBack = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2)->GetChildByIndex(0));
		m_btnVisit = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2)->GetChildByIndex(1));
		m_btnDelete = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2)->GetChildByIndex(2));

		m_btnTossBack->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityReceiveFlowerItem::onClick, this, std::placeholders::_1, ViewId::TOSS_BACK));
		m_btnVisit->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityReceiveFlowerItem::onClick, this, std::placeholders::_1, ViewId::VISIT));
		m_btnDelete->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityReceiveFlowerItem::onClick, this, std::placeholders::_1, ViewId::DEL));

		AddChildWindow(m_window);
	}

	void GuiBlockCityReceiveFlowerItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiBlockCityReceiveFlowerItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBlockCityReceiveFlowerItem::setItem(const ThumbUpRecord& item)
	{
		m_record = item;
		m_pic->setUrl(item.picUrl);
		m_name->SetText(item.userName.c_str());
	}

	bool GuiBlockCityReceiveFlowerItem::onClick(const EventArgs& args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::TOSS_BACK:
			ClientNetwork::Instance()->getCommonSender()->sendBlockCityTossBack(m_record.targetId);
			ClientHttpRequest::getThumbUpRecord(0, 20);
			break;
		case ViewId::VISIT:
			ClientNetwork::Instance()->getSender()->sendCallOnManor(m_record.targetId, 5, false, 0);
			break;
		case ViewId::DEL:
			ClientHttpRequest::deleteThumbUpRecord(m_record.messageId);
			break;
		}
		return true;
	}
}