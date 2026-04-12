#include "gui_supplyTip.h"
#include "Util/StringUtil.h"
#include "Util/ClientEvents.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_supplyTip::gui_supplyTip() :
		gui_layout("SupplyTip.json")
	{

	}

	gui_supplyTip::~gui_supplyTip()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_supplyTip::onLoad()
	{
		m_rootWindow->SetTouchable(false);
		m_subscriptionGuard.add(AddHealthSupplyTipEvent::subscribe(std::bind(&gui_supplyTip::createHealthAddUI, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(AddDefenseSupplyTipEvent::subscribe(std::bind(&gui_supplyTip::createDefenseAddUI, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(AddBulletSupplyTipEvent::subscribe(std::bind(&gui_supplyTip::createBulletAddUI, this, std::placeholders::_1, std::placeholders::_2)));
	}

	void gui_supplyTip::onUpdate(ui32 nTimeElapse)
	{
		updateOffsetViews(nTimeElapse);
	}

	void gui_supplyTip::updateOffsetViews(ui32 nTimeElapse)
	{
		for (auto iter = m_offsetViews.begin(); iter != m_offsetViews.end(); )
		{
			auto offsetView = *iter;
			updateOffsetView(offsetView, nTimeElapse);
			if (offsetView->m_progress < offsetView->m_duration)
			{
				iter++;
				continue;
			}
			if (offsetView->m_next != nullptr && offsetView->m_next->m_progress < offsetView->m_next->m_duration)
			{
				updateOffsetView(offsetView->m_next, nTimeElapse);
				iter++;
			}
			else
			{
				deleteOffsetView(offsetView);
				delete offsetView;
				iter = m_offsetViews.erase(iter);
			}
		}
	}

	void gui_supplyTip::updateOffsetView(OffsetView * offsetView, ui32 nTimeElapse)
	{
		if (offsetView->m_view == nullptr)
		{
			return;
		}
		if (offsetView->m_progress >= offsetView->m_duration)
		{
			return;
		}
		offsetView->m_progress += nTimeElapse;
		float progress = (float)offsetView->m_progress / (float)offsetView->m_duration;
		UVector2 position = offsetView->m_startPos + (offsetView->m_endPos - offsetView->m_startPos) * UVector2({ progress , progress }, { progress , progress });
		offsetView->m_view->SetPosition(position);
		offsetView->m_view->SetVisible(offsetView->m_progress < offsetView->m_duration);
	}

	void gui_supplyTip::deleteOffsetView(OffsetView * offsetView)
	{
		if (offsetView->m_view != nullptr)
		{
			offsetView->m_view->GetParent()->RemoveChildWindow(offsetView->m_view);
			offsetView->m_view = nullptr;
			if (offsetView->m_next != nullptr)
			{
				deleteOffsetView(offsetView->m_next);
				delete offsetView->m_next;
				offsetView->m_next = nullptr;
			}
		}
	}

	bool gui_supplyTip::createHealthAddUI(const String & icon, int addValue)
	{
		static unsigned int health_add_text_index = 0;
		health_add_text_index++;
		GUIString TextName = StringUtil::Format("HealthAddText-%d", health_add_text_index).c_str();
		GUIStaticText* Text = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, TextName);
		UVector2 t_startPos = UVector2({ 0.0f, -171.0f }, { 0.0f, -95.0f });
		UVector2 t_endPos = UVector2({ 0.0f, -140.0f }, { 0.0f, -120.0f });
		Text->SetArea(t_startPos, UVector2({ 0.0f, 258.0f }, { 0.0f, 25.0f }));
		Text->SetTextHorzAlign(HA_RIGHT);
		Text->SetTextVertAlign(VA_CENTRE);
		Text->SetTextColor(Color::RED);
		Text->SetText(StringUtil::Format("+%d", addValue).c_str());
		Text->SetTextBoader(true);
		Text->SetVisible(false);
		Text->SetTouchable(false);
		Text->SetVerticalAlignment(VA_BOTTOM);
		Text->SetHorizontalAlignment(HA_CENTRE);
		m_rootWindow->AddChildWindow(Text);
		OffsetView* offset_view_text = new OffsetView(Text, t_startPos, t_endPos, 400, nullptr);

		static unsigned int health_add_icon_index = 0;
		health_add_icon_index++;
		GUIString IconName = StringUtil::Format("HealthAddIcon-%d", health_add_icon_index).c_str();
		GUIStaticImage* Icon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, IconName);
		UVector2 i_startPos = UVector2({ 0.5f, -25.0f }, { 0.35f, -25.0f });
		UVector2 i_endPos = UVector2({ 0.42f, -25.0f }, { 0.83f, -25.0f });
		Icon->SetArea(i_startPos, UVector2({ 0.0f, 50.0f }, { 0.0f, 50.0f }));
		Icon->SetImage(icon.c_str());
		Icon->SetTouchable(false);
		m_rootWindow->AddChildWindow(Icon);
		OffsetView* offset_view_icon = new OffsetView(Icon, i_startPos, i_endPos, 600, offset_view_text);

		m_offsetViews.push_back(offset_view_icon);
		return true;
	}

	bool gui_supplyTip::createDefenseAddUI(const String & icon, int addValue)
	{
		static unsigned int defense_add_text_index = 0;
		defense_add_text_index++;
		GUIString TextName = StringUtil::Format("DefenseAddText-%d", defense_add_text_index).c_str();
		GUIStaticText* Text = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, TextName);
		UVector2 t_startPos = UVector2({ 0.0f, -171.0f }, { 0.0f, -125.0f });
		UVector2 t_endPos = UVector2({ 0.0f, -140.0f }, { 0.0f, -160.0f });
		Text->SetArea(t_startPos, UVector2({ 0.0f, 258.0f }, { 0.0f, 25.0f }));
		Text->SetTextHorzAlign(HA_RIGHT);
		Text->SetTextVertAlign(VA_CENTRE);
		Text->SetTextColor(Color::BLUE);
		Text->SetText(StringUtil::Format("+%d", addValue).c_str());
		Text->SetTextBoader(true);
		Text->SetVisible(false);
		Text->SetTouchable(false);
		Text->SetVerticalAlignment(VA_BOTTOM);
		Text->SetHorizontalAlignment(HA_CENTRE);
		m_rootWindow->AddChildWindow(Text);
		OffsetView* offset_view_text = new OffsetView(Text, t_startPos, t_endPos, 400, nullptr);

		static unsigned int defense_add_icon_index = 0;
		defense_add_icon_index++;
		GUIString IconName = StringUtil::Format("DefenseAddIcon-%d", defense_add_icon_index).c_str();
		GUIStaticImage* Icon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, IconName);
		UVector2 i_startPos = UVector2({ 0.5f, -25.0f }, { 0.35f, -25.0f });
		UVector2 i_endPos = UVector2({ 0.42f, -25.0f }, { 0.79f, -25.0f });
		Icon->SetArea(i_startPos, UVector2({ 0.0f, 50.0f }, { 0.0f, 50.0f }));
		Icon->SetImage(icon.c_str());
		Icon->SetTouchable(false);
		m_rootWindow->AddChildWindow(Icon);
		OffsetView* offset_view_icon = new OffsetView(Icon, i_startPos, i_endPos, 600, offset_view_text);

		m_offsetViews.push_back(offset_view_icon);
		return true;
	}

	bool gui_supplyTip::createBulletAddUI(const String & icon, int addValue)
	{
		static unsigned int bullet_add_icon_index = 0;
		bullet_add_icon_index++;
		GUIString IconName = StringUtil::Format("BulletAddIcon-%d", bullet_add_icon_index).c_str();
		GUIStaticImage* Icon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, IconName);
		UVector2 i_startPos = UVector2({ 0.5f, -25.0f }, { 0.35f, -25.0f });
		UVector2 i_endPos = UVector2({ 0.5f, -25.0f }, { 0.92f, -25.0f });
		Icon->SetArea(i_startPos, UVector2({ 0.0f, 50.0f }, { 0.0f, 50.0f }));
		Icon->SetImage(icon.c_str());
		Icon->SetTouchable(false);
		m_rootWindow->AddChildWindow(Icon);
		OffsetView* offset_view_icon = new OffsetView(Icon, i_startPos, i_endPos, 600, nullptr);
		m_offsetViews.push_back(offset_view_icon);
		return true;
	}

}
