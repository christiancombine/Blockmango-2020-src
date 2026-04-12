#include "gui_guideSystem.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "game.h"
#include "Setting/GuideSystemSetting.h"
#include "UI/GUISystem.h"
#include "UI/GUIStaticImage.h"

namespace BLOCKMAN
{
	gui_guideSystem::gui_guideSystem() :
		gui_layout("GuideSystem.json")
	{
	}

	gui_guideSystem::~gui_guideSystem()
	{
		m_subscriptionGuard.unsubscribeAll();
		GuideSystemSetting::unloadSetting();
	}

	void gui_guideSystem::onLoad()
	{
		GuideSystemSetting::loadSetting();

		auto Centre = getWindow<GUILayout>("GuideSystem-Centre");
		Centre->subscribeEvent(EventWindowClick, std::bind(&gui_guideSystem::onButtonClick, this, std::placeholders::_1));

		auto Skip = getWindow<GUIButton>("GuideSystem-Skip");
		Skip->subscribeEvent(EventButtonClick, std::bind(&gui_guideSystem::onSkipClick, this, std::placeholders::_1));

		auto Left = getWindow<GUILayout>("GuideSystem-Left-Layout");
		auto Right = getWindow<GUILayout>("GuideSystem-Right-Layout");
		auto Up = getWindow<GUILayout>("GuideSystem-Up-Layout");
		auto Down = getWindow<GUILayout>("GuideSystem-Down-Layout");

		Left->subscribeEvent(EventWindowClick, std::bind(&gui_guideSystem::onWindowClick, this, std::placeholders::_1));
		Right->subscribeEvent(EventWindowClick, std::bind(&gui_guideSystem::onWindowClick, this, std::placeholders::_1));
		Up->subscribeEvent(EventWindowClick, std::bind(&gui_guideSystem::onWindowClick, this, std::placeholders::_1));
		Down->subscribeEvent(EventWindowClick, std::bind(&gui_guideSystem::onWindowClick, this, std::placeholders::_1));


		m_subscriptionGuard.add(ShowGuideInfoEvent::subscribe(std::bind(&gui_guideSystem::showGuideInfo, this, std::placeholders::_1,
			std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
		m_subscriptionGuard.add(ShowGuideEffectEvent::subscribe(std::bind(&gui_guideSystem::showSereenEffect, this, std::placeholders::_1, std::placeholders::_2)));
	}

	bool gui_guideSystem::onButtonClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		getParent()->setGuidePanelShow(false);
		hide();
		const WindowEventArgs& event = dynamic_cast<const WindowEventArgs&>(events);
		if (event.window)
		{
			auto area = event.window->GetRenderArea();
			float x = (area.left + area.right) / 2;
			float y = (area.top + area.bottom) / 2;
			GameClient::CGame::Instance()->handleTouchClick(x, y);
		}
		GuideSystemSetting::onGuideDone(m_guide);
		return false;
	}
	bool gui_guideSystem::onWindowClick(const EventArgs & events)
	{
		SetGuidePanelShowEvent::emit(false);
		GuideSystemSetting::onPlayerSkip(m_skip, m_guide);
		return false;
	}
	bool gui_guideSystem::onSkipClick(const EventArgs & events)
	{
		SetGuidePanelShowEvent::emit(false);
		GuideSystemSetting::onPlayerSkip(m_skip, m_guide);
		return false;
	}

	bool gui_guideSystem::showGuideInfo(bool isCompel, int skip, const UVector2& uvPos, int guideId, int buttonSize)
	{
		//float x = StringUtil::ParseFloat(pos[0]);
		//float y = StringUtil::ParseFloat(pos[1]);

		m_guide = guideId;
		m_skip = skip;
		bool isSkip = skip > 0;

		float button_size = buttonSize > 0 ? (float)buttonSize : 45.0f;
		auto Centre = getWindow<GUILayout>("GuideSystem-Centre");
		Centre->SetPosition(uvPos);
		Centre->SetVisible(true);
		Centre->SetSize({ {0.0f, button_size}, {0.0f, button_size} });
		auto width = Centre->GetWidth().d_offset;
		auto height = Centre->GetHeight().d_offset;

		float lucency = isCompel ? 130.f : 0.0f;
		Color color = Color(0.0f, 0.0f, 0.0f, lucency / 255.f);

		auto Left = getWindow<GUILayout>("GuideSystem-Left-Layout");
		Left->SetPosition({ {0.0f, 0.0f}, {0.0f, 0.0f} });
		//Left->SetSize({ {x, 0.0f}, {1.0f, 0.0f} });
		Left->SetSize(UVector2(uvPos.d_x, { 1.0f, 0.0f }));
		Left->SetBackgroundColor(color);
		Left->SetTouchable(!isCompel);
		Left->SetVisible(true);

		auto Right = getWindow<GUILayout>("GuideSystem-Right-Layout");
		Right->SetPosition({ {uvPos.d_x.d_scale, uvPos.d_x.d_offset + width}, {0.0f, 0.0f} });
		Right->SetSize({{1 - uvPos.d_x.d_scale, - width - uvPos.d_x.d_offset}, {1.0f, 0.0f} });
		Right->SetBackgroundColor(color);
		Right->SetTouchable(!isCompel);
		Right->SetVisible(true);

		auto Up = getWindow<GUILayout>("GuideSystem-Up-Layout");
		Up->SetPosition({ {uvPos.d_x.d_scale, uvPos.d_x.d_offset}, {0.0f, 0.0f} });
		Up->SetSize({ {0.0f, width}, {uvPos.d_y.d_scale, uvPos.d_y.d_offset} });
		Up->SetBackgroundColor(color);
		Up->SetTouchable(!isCompel);
		Up->SetVisible(true);

		auto Down = getWindow<GUILayout>("GuideSystem-Down-Layout");
		Down->SetPosition(uvPos + UVector2({0, 0.0f}, {0, height}));

		Down->SetSize({ {0.0f, width}, {1 - uvPos.d_y.d_scale,  - height - uvPos.d_y.d_offset} });
		Down->SetBackgroundColor(color);
		Down->SetTouchable(!isCompel);
		Down->SetVisible(true);

		auto Skip = getWindow<GUIButton>("GuideSystem-Skip");
		Skip->SetVisible(isSkip);
		Skip->SetTouchable(isSkip);


		auto Shade = getWindow<GUIStaticImage>("GuideSystem-Shade");
		Shade->SetVisible(isCompel);
		Shade->SetAlpha(lucency / 255.f);
		return false;
	}

	bool gui_guideSystem::showSereenEffect(String effect, StringArray pos)
	{
		float x = StringUtil::ParseFloat(pos[0]);
		float y = StringUtil::ParseFloat(pos[1]);

		auto Centre = getWindow<GUILayout>("GuideSystem-Centre");
		auto Effect = getWindow<GUIStaticImage>("GuideSystem-Effect");
		if (checkGameType(ClientGameType::BlockCity))
		{
			Effect->SetPosition({ {x, 0.0f}, {y, -10.0f} });
			Effect->SetSize({ {0.f, 30.0f}, {0, 30.0f} });
		}
		else
		{
			Centre->SetPosition({ {x, 0.0f}, {y, 0.0f} });
			Effect->SetSize({ {0.f, 20.0f}, {0, 20.0f} });
		}
		Effect->SetVisible(true);
		Effect->SetEffectName(effect.c_str());
		return false;
	}

}