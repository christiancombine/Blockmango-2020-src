#include "gui_chestRewardDetail.h"
#include "UI/GUILayout.h"

namespace BLOCKMAN
{
	gui_chestRewardDetail::gui_chestRewardDetail() :
		gui_layout("ChestRewardDetail.json")
	{
	}

	gui_chestRewardDetail::~gui_chestRewardDetail()
	{
	}

	void gui_chestRewardDetail::onLoad()
	{
		GUILayout* ChestRewardDetail = getWindow<GUILayout>("ChestRewardDetail");
		ChestRewardDetail->subscribeEvent(EventWindowTouchUp, std::bind(&gui_chestRewardDetail::onContentClick, this, std::placeholders::_1));

		m_title = getWindow<GUIStaticText>("ChestRewardDetail-Title");
		m_image = getWindow<GUIStaticImage>("ChestRewardDetail-Image");
		m_name = getWindow<GUIStaticText>("ChestRewardDetail-Name");
		m_desc = getWindow<GUIStaticText>("ChestRewardDetail-Desc");
	}

	bool gui_chestRewardDetail::isNeedLoad()
	{
		return checkGameType(ClientGameType::PixelGunGame1v1) || checkGameType(ClientGameType::PixelGunGamePerson) || checkGameType(ClientGameType::PixelGunGameTeam) || checkGameType(ClientGameType::PixelGunHall);
	}

	void gui_chestRewardDetail::ShowLotteryChestReward(Reward * reward)
	{
		m_title->SetText(getString(reward->Title));
		m_image->SetImage(reward->Image.c_str());
		m_name->SetText(getString(reward->Name));
		m_desc->SetText(getString(reward->Desc));
	}

	bool gui_chestRewardDetail::onContentClick(const EventArgs & events)
	{
		getParent()->hideChestRewardDetail();
		return true;
	}

}
