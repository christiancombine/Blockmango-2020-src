#include "gui_buyGoodsTip.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderBlocks.h"
#include "cItem/cItem.h"
#include "cBlock/cBlockManager.h"
#include "cBlock/cBlock.h"
#include "GUI/GuiItemStack.h"
#include "Util/PlayerPrefs.h"
#include "game.h"
#include "Util/UICommon.h"
#include "Block/BlockUtil.h"

using namespace LORD;
namespace BLOCKMAN
{

	gui_buyGoodsTip::gui_buyGoodsTip()
		: gui_layout("BuyGoodsTip.json")
		, m_totalTime(0)
		, m_isPlaying(false)
	{
	}

	gui_buyGoodsTip::~gui_buyGoodsTip()
	{
	}

	void gui_buyGoodsTip::onLoad()
	{
		m_tipBg = getWindow<GUIStaticImage>("BuyGoodsTip-Bg");
		m_countText = getWindow<GUIStaticText>("BuyGoodsTip-Text"); 
		m_iconImage = getWindow<GUIStaticImage>("BuyGoodsTip-Icon");
		m_sellFailText = getWindow<GUIStaticText>("BuyGoodsTip-SellFailText");
		m_sellFailText->SetText(getString(LanguageKey::GUI_ARMOR_BAG_SELL_FAIL_TXT));
		m_sellFailText->SetVisible(false);
		m_originPos = m_rootWindow->GetPosition();
		bool isShowBg = PlayerPrefs::Instance()->getBoolPrefs("IsShowGetGoodsBg");
		m_originPos.d_y.d_offset = m_originPos.d_y.d_offset - (isShowBg ? 0.0f : 100.0f);
		m_totalTime = 0;
		auto rect = m_rootWindow->GetUnclippedOuterRect();
		m_moveDistance = rect.top;

		m_subscriptionGuard.add(GetMoneyEvent::subscribe(std::bind(&gui_buyGoodsTip::showContent, this, std::placeholders::_1)));
		m_subscriptionGuard.add(GetItemEvent::subscribe(std::bind(&gui_buyGoodsTip::showItemContent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(GetGoodsEvent::subscribe(std::bind(&gui_buyGoodsTip::showGoodsContent, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(notifyGetGoodsByItemIdEvent::subscribe(std::bind(&gui_buyGoodsTip::showGoodsContentByItem, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
	}

	void gui_buyGoodsTip::onShow()
	{

	}

	void gui_buyGoodsTip::onUpdate(ui32 nTimeElapse)
	{
		if (!m_isPlaying)
			return;

		m_totalTime += nTimeElapse;
		if (m_totalTime < m_stayTime)
		{
			return;
		}

		float ratio = (float)(m_totalTime - m_stayTime)/ m_animateTime;
		if (m_sellFailText->IsVisible())
		{
			m_rootWindow->SetPosition(m_originPos);
			if (ratio > 1.0f)
			{
				m_isPlaying = false;
				getParent()->showBuyGoodsTip(false);
				m_rootWindow->SetAlpha(0.0f);
				m_tipBg->SetVisible(true);
				m_countText->SetVisible(true);
				m_sellFailText->SetVisible(false);
			}
		}
		else
		{
			ratio = ratio > 1.0f ? 1.0f : ratio;
			float moveY = ratio * m_moveDistance;
			LORD::UVector2 newPos = UVector2(m_originPos.d_x, UDim(m_originPos.d_y.d_scale, m_originPos.d_y.d_offset - moveY));
			m_rootWindow->SetPosition(newPos);
			m_rootWindow->SetAlpha(1.0f - ratio);
			if (ratio >= 1.0f )
			{
				if (!m_moneyTips.empty())
				{
					i32 sum = 0;
					for (i32 count : m_moneyTips)
					{
						sum += count;
					}
					m_moneyTips.clear();
					refreshOriginState(sum);
				}
				else
				{
					m_isPlaying = false;
					getParent()->showBuyGoodsTip(false);
				}
			}
		}
	}

	bool gui_buyGoodsTip::showContent(int count)
	{
		if (checkGameType(ClientGameType::PixelGunHall))
		{
			return false;
		}
		m_tipBg->SetVisible(PlayerPrefs::Instance()->getBoolPrefs("IsShowGetGoodsBg"));
		m_iconImage->SetOverlayImage(UICommon::getCurrencyIcon().c_str());
		m_iconImage->SetImage(UICommon::getCurrencyIcon().c_str());
		m_iconImage->SetRenderAsBlock(false);
		m_iconImage->SetVisible(true);
		m_countText->SetVisible(true);
		if (isShown() && checkGameType(ClientGameType::BirdSimulator))
		{
			m_moneyTips.push_back(count);
			if (!isShown())
			{
				m_isPlaying = true;
				getParent()->showBuyGoodsTip(true);
			}
		}
		else
		{
			refreshOriginState(count);
		}
		return true;
	}

	bool gui_buyGoodsTip::showItemContent(int itemId, int meta, int count)
	{
		if (checkGameType(ClientGameType::PixelGunHall))
		{
			return false;
		}
		m_tipBg->SetVisible(PlayerPrefs::Instance()->getBoolPrefs("IsShowGetGoodsBg"));
		m_countText->SetVisible(!checkGameType(ClientGameType::Build_War));
		refreshOriginState(count);
		updateIcon(itemId, meta);
		m_iconImage->SetVisible(true);
		return true;
	}

	void gui_buyGoodsTip::onGetCommonData(String key, String data)
	{
		if (key == "NotifySellItemFail")
		{
			m_rootWindow->SetAlpha(1.0f);
			m_rootWindow->SetPosition(m_originPos);
			m_sellFailText->SetTextColor(LORD::Color::RED);
			m_sellFailText->SetVisible(true);
			m_tipBg->SetVisible(false);
			m_countText->SetVisible(false);
			m_iconImage->SetVisible(false);
			m_totalTime = 0;
			m_isPlaying = true;
			getParent()->showBuyGoodsTip(true);
			return ;
		}

	}

	bool gui_buyGoodsTip::showGoodsContent(const String & icon, int count)
	{
		if (checkGameType(ClientGameType::PixelGunHall))
		{
			return false;
		}
		if (checkGameType(ClientGameType::HASHall))
		{
			m_stayTime = 1000;
		}
		m_tipBg->SetVisible(PlayerPrefs::Instance()->getBoolPrefs("IsShowGetGoodsBg"));
		m_iconImage->SetImage(icon.c_str());
		m_iconImage->SetOverlayImage(icon.c_str());
		m_iconImage->SetRenderAsBlock(false);
		m_iconImage->SetVisible(true);
		m_countText->SetVisible(count > 0);
		refreshOriginState(count);
		return true;
	}

	bool gui_buyGoodsTip::showGoodsContentByItem(int itemId, int meta, int count, const String & icon)
	{
		int cur_blockId = itemId;

		if (BlockUtil::isValidBlockId(cur_blockId))
		{
			Block* pBlock = BlockManager::sBlocks[cur_blockId];
			if (pBlock != NULL)
			{
				if (!BlockUtil::isBasicBlock(cur_blockId))
				{
					cur_blockId = cur_blockId + Item::EC_BLOCK_TO_ITEM_OFFSET;
				}
			}
		}

		GuiItemStack::showItemIcon(m_iconImage, cur_blockId, meta, icon);
		m_tipBg->SetVisible(PlayerPrefs::Instance()->getBoolPrefs("IsShowGetGoodsBg"));
		m_countText->SetVisible(count > 0);
		refreshOriginState(count);
		return true;
	}

	void gui_buyGoodsTip::refreshOriginState(int count)
	{
		m_countText->SetTextColor(m_tipBg->IsVisible() ? Color::BLACK : Color::WHITE);
		m_countText->SetTextBoader(m_tipBg->IsVisible() ? Color::TRANS : Color::BLACK);
		String text = StringUtil::Format("%c%s", count >= 0 ? '+' : ' ', UICommon::numberToString(count).c_str());
		m_countText->SetText(text.c_str());
		m_rootWindow->SetPosition(m_originPos);
		m_rootWindow->SetAlpha(1.0f);
		m_totalTime = 0;
		m_isPlaying = true;
		getParent()->showBuyGoodsTip(true);

		if (checkGameType(ClientGameType::BirdSimulator))
		{
			String addMessage = StringUtil::Format(getString("gui_bird_add_message").c_str(), getString("fruitcoin_9000001").c_str(), UICommon::numberToString((i64)count).c_str());
			BirdAddMessageEvent::emit(addMessage);
		}
	}

	void gui_buyGoodsTip::updateIcon(int itemId, int meta)
	{
		if (itemId >= 10000)
		{
			if (m_iconImage)
			{
				m_iconImage->SetOverlayImage(UICommon::getCurrencyIcon().c_str());
				m_iconImage->SetImage(UICommon::getCurrencyIcon().c_str());
				m_iconImage->SetRenderAsBlock(false);
			}
			return;
		}
		GuiItemStack::showItemIcon(m_iconImage, itemId, meta);
	}
}