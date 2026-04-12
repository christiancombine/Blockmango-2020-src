#include "gui_videoAdTip.h"
#include "ShellInterface.h"
#include "UI/GUIWindowManager.h"
#include "Util/UICommon.h"
#include "game.h"
#include "UI/GUIButton.h"
#include "UI/GUIRenderText.h"
#include "UI/GUIStaticImage.h"

namespace BLOCKMAN
{
	gui_videoAdTip::gui_videoAdTip() :
		gui_layout("VideoAdTip.json")
	{
	}

	gui_videoAdTip::~gui_videoAdTip()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_videoAdTip::onStart()
	{
		if (checkGameType({ ClientGameType::BED_WAR_HALL ,ClientGameType::BED_WAR }))
		{
			m_jsonName = "VideoAdTipBedWar.json";
		}
	}

	void gui_videoAdTip::onLoad()
	{
		left_position = getWindow("VideoAdTip-Watch-Cancel")->GetPosition();
		right_position = getWindow("VideoAdTip-Watch-Sure")->GetPosition();
		getWindow("VideoAdTip-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_videoAdTip::onCloseClick, this, std::placeholders::_1));
		getWindow("VideoAdTip-Watch-Ad")->subscribeEvent(EventButtonClick, std::bind(&gui_videoAdTip::onWatchClick, this, std::placeholders::_1));
		getWindow("VideoAdTip-Watch-Sure")->subscribeEvent(EventButtonClick, std::bind(&gui_videoAdTip::onSureClick, this, std::placeholders::_1));
		getWindow("VideoAdTip-Watch-Cancel")->subscribeEvent(EventButtonClick, std::bind(&gui_videoAdTip::onCloseClick, this, std::placeholders::_1));
		/*m_text_offset = getWindow<GUIButton>("VideoAdTip-Watch-Ad")->getRenderText().getRenderOffset();*/
		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_videoAdTip::onVideoAdVisible, this, std::placeholders::_1)));
	}

	void gui_videoAdTip::onGetCommonData(String key, String data)
	{	
		clearDynamicUI();
		auto builder = CommonDataBuilder().fromData(data);
		m_ad_type = builder.getIntParam("AdType");	
		m_ad_params = builder.getStringParam("AdParams");
		m_try_show_report = builder.getStringParam("TryShowReport");
		m_show_report = builder.getStringParam("ShowReport");
		m_watch_report = builder.getStringParam("WatchReport");
		m_images_report = builder.getStringParam("ImagesReport");
		m_texts_report = builder.getStringParam("TextsReport");

		if (!isLoaded())
		{
			onCloseClick(EventArgs());
			return;
		}

		bool isAutoClose = builder.getBoolParam("IsAutoClose");
		if (!m_have_ad && isAutoClose)
		{
			onCloseClick(EventArgs());
			return;
		}

		getWindow("VideoAdTip-Title-Name")->SetText(getString(builder.getStringParam("Title")));
		getWindow("VideoAdTip-Content-Text")->SetText(getString(builder.getStringParam("Content")));
		getWindow("VideoAdTip-Close")->SetVisible(builder.getBoolParam("ShowCloseButton"));

		m_show_btn_num = 0;
		initSureButton(&builder);
		initCancelButton(&builder);
		initAdButton(&builder);
		initImages(&builder);
		initText(&builder);
		if (m_show_btn_num == 0)
		{
			return;
		}
		if (m_show_btn_num == 1)
		{
			getWindow("VideoAdTip-Watch-Ad")->SetXPosition({ 0,0 });
			getWindow("VideoAdTip-Watch-Ad")->SetHorizontalAlignment(HA_CENTRE);

			getWindow("VideoAdTip-Watch-Sure")->SetXPosition({ 0,0 });
			getWindow("VideoAdTip-Watch-Sure")->SetHorizontalAlignment(HA_CENTRE);

			getWindow("VideoAdTip-Watch-Cancel")->SetXPosition({ 0,0 });
			getWindow("VideoAdTip-Watch-Cancel")->SetHorizontalAlignment(HA_CENTRE);
		}
		else
		{
			getWindow("VideoAdTip-Watch-Ad")->SetXPosition(left_position.d_x);
			getWindow("VideoAdTip-Watch-Ad")->SetHorizontalAlignment(HA_LEFT);

			getWindow("VideoAdTip-Watch-Sure")->SetXPosition(right_position.d_x);
			getWindow("VideoAdTip-Watch-Sure")->SetHorizontalAlignment(HA_RIGHT);

			getWindow("VideoAdTip-Watch-Cancel")->SetXPosition(left_position.d_x);
			getWindow("VideoAdTip-Watch-Cancel")->SetHorizontalAlignment(HA_LEFT);
		}
		getParent()->showWindowByLayoutId(RootGuiLayout::LayoutId::VIDEO_AD_TIP);
	}

	bool gui_videoAdTip::onVideoAdVisible(bool visible)
	{
		m_have_ad = visible;
		return true;
	}

	bool gui_videoAdTip::onCloseClick(const EventArgs & events)
	{
		getParent()->hideWindowByLayoutId(RootGuiLayout::LayoutId::VIDEO_AD_TIP);
		ClientNetwork::Instance()->getCommonSender()->sendCloseVideoAdTip(m_ad_type, m_ad_params, false);
		clearDynamicUI();
		return true;
	}

	bool gui_videoAdTip::onWatchClick(const EventArgs & events)
	{
		if (m_ad_type == 0)
			return false;
		LordLogInfo("onWatchClick start");
		GameClient::CGame::Instance()->getShellInterface()->onWatchAd(m_ad_type, m_ad_params);
		//ClientNetwork::Instance()->getSender()->sendWatchAdFinished(m_ad_type, m_ad_params, 1);
		getParent()->hideWindowByLayoutId(RootGuiLayout::LayoutId::VIDEO_AD_TIP);
		if (!m_watch_report.empty())
		{
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport(m_watch_report.c_str(), GameClient::CGame::Instance()->getGameType());
		}
		return true;
	}

	bool gui_videoAdTip::onSureClick(const EventArgs & events)
	{
		getParent()->hideWindowByLayoutId(RootGuiLayout::LayoutId::VIDEO_AD_TIP);
		ClientNetwork::Instance()->getCommonSender()->sendCloseVideoAdTip(m_ad_type, m_ad_params, true);
		clearDynamicUI();
		return false;
	}

	void BLOCKMAN::gui_videoAdTip::initSureButton(CommonDataBuilder * builder)
	{
		String text = builder->getStringParam("SureButtonTxt");
		String icon = builder->getStringParam("SureIcon");
		bool isShow = (!text.empty() || !icon.empty());
		getWindow("VideoAdTip-Watch-Sure")->SetVisible(isShow);
		if (isShow)
		{
			m_show_btn_num++;

			initCommonButton(getWindow<GUIButton>("VideoAdTip-Watch-Sure"), text, icon);
		}
	}

	void BLOCKMAN::gui_videoAdTip::initCancelButton(CommonDataBuilder * builder)
	{
		String text = builder->getStringParam("CancelButtonTxt");
		String icon = builder->getStringParam("CancelIcon");
		bool isShow = (!text.empty() || !icon.empty());
		getWindow("VideoAdTip-Watch-Cancel")->SetVisible(isShow);
		if (isShow)
		{
			m_show_btn_num++;

			initCommonButton(getWindow<GUIButton>("VideoAdTip-Watch-Cancel"), text, icon);
		}
	}

	void BLOCKMAN::gui_videoAdTip::initAdButton(CommonDataBuilder * builder)
	{
		String text = builder->getStringParam("AdButtonTxt");
		String icon = builder->getStringParam("AdIcon");
		bool isShow = (!text.empty() || !icon.empty());
		getWindow("VideoAdTip-Watch-Ad")->SetVisible(isShow && m_have_ad);

		if (isShow && !m_try_show_report.empty())
		{
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport(m_try_show_report.c_str(), GameClient::CGame::Instance()->getGameType());
		}
		if (isShow && m_have_ad)
		{
			m_show_btn_num++;

			initCommonButton(getWindow<GUIButton>("VideoAdTip-Watch-Ad"), text, icon);

			if (!m_show_report.empty())
			{
				GameClient::CGame::Instance()->getShellInterface()->
					dataReport(m_show_report.c_str(), GameClient::CGame::Instance()->getGameType());
			}
		}
	}

	void BLOCKMAN::gui_videoAdTip::initCommonButton(GUIButton* button, String buttonText, String iconStr)
	{
		button->SetText(getString(buttonText));
		button->SetRenderOffset(m_text_offset);
		if (button->GetChildCount() == 0 )
		{
			button->SetRenderOffset({ 0,0 });
			return;
		}

		auto icon = dynamic_cast<GUIStaticImage*>(button->GetChildByIndex(0));
		icon->SetImage(iconStr.c_str());
		if (iconStr.empty())
		{
			button->SetRenderOffset({ 0,0 });
		}

		if (buttonText.empty())
		{
			icon->SetXPosition({ 0,0 });
			icon->SetHorizontalAlignment(HA_CENTRE);
		}
	}

	void BLOCKMAN::gui_videoAdTip::initImages(CommonDataBuilder* builder)
	{
		m_images_report = builder->getStringParam("ImagesReport");
		auto images = StringUtil::Split(m_images_report, "#");
		for (auto image : images)
		{
			auto imageSetting = StringUtil::Split(image, ",");
			
			if (!imageSetting.empty()&&imageSetting.size() == 6)
			{
				String imageWindow = imageSetting[0];
				String imageName = imageSetting[1];
				GUIStaticImage* StaticImage = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, imageWindow.c_str());
				m_dynamic_images.emplace_back(StaticImage);
				StaticImage->SetImage(imageName.c_str());
				getWindow("VideoAdTip-Panel")->AddChildWindow(StaticImage);
				auto imageXPos = StringUtil::ParseFloat(imageSetting[2]);
				auto imageYPos = StringUtil::ParseFloat(imageSetting[3]);
				auto imageWidth = StringUtil::ParseFloat(imageSetting[4]);
				auto imageHeight = StringUtil::ParseFloat(imageSetting[5]);
				StaticImage->SetArea(UDim(imageXPos, 0.f), UDim(imageYPos, 0.f), UDim(imageWidth, 0.f), UDim(imageHeight, 0.f));
			}
		}
	}

	void BLOCKMAN::gui_videoAdTip::initText(CommonDataBuilder* builder)
	{
		m_texts_report = builder->getStringParam("TextsReport");
		auto texts = StringUtil::Split(m_texts_report, "#");
		for (auto text : texts)
		{
			auto textSetting = StringUtil::Split(text, ",");
			if (!textSetting.empty() && textSetting.size() == 6)
			{
				String TextWindow = textSetting[0];
				String TextName = textSetting[1];
				GUIStaticText* StaticText = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, TextWindow.c_str());
				m_dynamic_text.emplace_back(StaticText);
				StaticText->SetText(getString(textSetting[1]).c_str());
				StaticText->SetTextHorzAlign(HA_CENTRE);
				getWindow("VideoAdTip-Panel")->AddChildWindow(StaticText);
				auto textXPos = StringUtil::ParseFloat(textSetting[2]);
				auto textYPos = StringUtil::ParseFloat(textSetting[3]);
				auto textWidth = StringUtil::ParseFloat(textSetting[4]);
				auto textHeight = StringUtil::ParseFloat(textSetting[5]);
				StaticText->SetArea(UDim(textXPos, 0.f), UDim(textYPos, 0.f), UDim(textWidth, 0.f), UDim(textHeight, 0.f));
				StaticText->SetWordWrap(true);
			}
		}
	}


	void BLOCKMAN::gui_videoAdTip::clearDynamicUI()
	{

		for (auto image = m_dynamic_images.begin(); image != m_dynamic_images.end(); image++)
		{
			(*image)->Destroy();
		}
		m_dynamic_images.clear();

		for (auto text = m_dynamic_text.begin(); text != m_dynamic_text.end(); text++)
		{
			(*text)->Destroy();
		}
		m_dynamic_text.clear();
	}
}
