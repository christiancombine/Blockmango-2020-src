#include "UICommon.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/ClientEvents.h"
#include "game.h"
#include "Game/BirdSimulator.h"
#include "Game/BlockCity.h"
#include "Setting/GameTypeSetting.h"
#include "GUI/RootGuiLayout.h"
#include "Util/LanguageManager.h"
#include "Entity//EntityBulletin.h"
#include "Entity/EntitySessionNpc.h"
#include "cEntity//EntityActorCannonClient.h"
#include "cWorld/WorldClient.h"
#include "Network/ClientNetwork.h"
#include "Setting/ActionAreaManager.h"
#include "Entity/EntityDecoration.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Audio/SoundSystem.h"
#include "GUI/gui_tipDialog.h"

namespace BLOCKMAN {

	void UICommon::checkMoney(i32 price, i32 type, std::function<void(bool)> func)
	{
		if (!checkWalletParam())
		{
			func(false);
			return;
		}
		Wallet* wallet = Blockman::Instance()->m_pPlayer->m_wallet;
		i64 currency = 0;
		if (type == CurrencyType::OTHER_MONEY)
			currency = getOtherCurrency();
		else
			currency = wallet->getMoneyByCoinId(type);

		LordLogInfo("UICommon::checkMoney price: %d, type: %d, currency: %d", price, type, (int)currency);
		if (price > currency)
		{
			ShowLackOfMoneyTipDialogEvent::emit((i32)(price - currency), (CurrencyType)type);
			func(false);
		}
		else
		{
		    func(true);
		}
	}

	void UICommon::checkMoney(i32 price, CurrencyType type, std::function<void(bool)> func)
	{
		checkMoney(price, (int)type, func);
	}

	bool UICommon::checkRanchItems(const std::vector<RanchCommon>& items)
	{
		vector<RanchCommon>::type newItems = vector<RanchCommon>::type();
		for (auto item : items)
		{
			newItems.push_back(item);
		}
		return checkRanchItems(newItems);
	}

	bool UICommon::checkRanchItems(const vector<RanchCommon>::type& items)
	{
		if (!checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			if (items.size() > 0)
			{
				ShowRanchLockItemsEvent::emit(items);
				return false;
			}
			return true;
		}

		RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		vector<RanchCommon>::type newItems = vector<RanchCommon>::type();

		RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		i32 prosperity = info->prosperity;
		i64 currency = Blockman::Instance()->m_pPlayer->m_wallet->getCurrency();

		i32 needCurrency = 0;
		i32 needProsperity = 0;

		for (auto item : items)
		{
			i32 lack = item.num - storage->getItemNum(item.itemId);
			if (lack > 0 && item.itemId < 900000)
			{
				newItems.push_back(RanchCommon(item.itemId, lack, item.price));
			}

			if (item.itemId == 900001)
			{
				needCurrency = (int)(item.num - currency);
			}

			if (item.itemId == 900002)
			{
				needProsperity = item.num - prosperity;
			}
		}

		if (newItems.size() > 0)
		{
			ShowRanchLockItemsEvent::emit(newItems);
			return false;
		}

		if (needProsperity > 0)
		{
			ShowRanchCommonTipEvent::emit("gui_ranch_check_prosperity");
			return false;
		}

		if (needCurrency > 0)
		{
			ShowLackOfMoneyTipDialogEvent::emit(needCurrency, CurrencyType::GAME_MONEY);
			return false;
		}

		return true;
	}

	bool UICommon::checkRanchStorageCapacity(const vector<RanchCommon>::type& items)
	{
		if (!checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			ShowRanchStorageFullTipEvent::emit("gui_ranch_storage_full_operation_failure");
			return false;
		}
		i32 num = 0;
		for (auto item : items)
		{
			num += item.num;
		}

		if (!Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->canSave(num))
		{
			ShowRanchStorageFullTipEvent::emit("gui_ranch_storage_full_operation_failure");
			return false;
		}

		return true;
	}

	bool UICommon::checkRanchLevel(i32 needLevel)
	{
		if (checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= needLevel)
		{
			return true;
		}

		ShowRanchCommonTipEvent::emit("gui_ranch_check_level");
		return false;
	}

	bool UICommon::isRanchIconVisible()
	{
		if (!checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			return false;
		}
		auto ranchInfo = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		const auto& ranchStartPos = Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->startPos;
		const auto& ranchEndPos = Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->endPos;
		const auto& pos = Blockman::Instance()->m_pPlayer->position;
		return (pos.x >= ranchStartPos.x) && (pos.z >= ranchStartPos.z) && (pos.x <= ranchEndPos.x) && (pos.z <= ranchEndPos.z);
	}

	bool UICommon::checkOpenFlying()
	{
		//if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher)
		//{
		//	if (checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		//	{
		//		return !Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->isCanFlying;
		//	}
		//}


		return false;
	}

	vector<RanchCommon>::type UICommon::getLackRanchItems(const std::vector<RanchCommon>& items)
	{
		vector<RanchCommon>::type newItems = vector<RanchCommon>::type();
		RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		for (auto item : items)
		{
			i32 lack = item.num - storage->getItemNum(item.itemId);
			if (lack > 0)
			{
				newItems.push_back(RanchCommon(item.itemId, lack, item.price));
			}
		}
		return newItems;
	}

	vector<RanchCommon>::type UICommon::getLackRanchItems(const vector<RanchCommon>::type & items)
	{
		vector<RanchCommon>::type newItems = vector<RanchCommon>::type();
		RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		for (auto item : items)
		{
			i32 lack = item.num - storage->getItemNum(item.itemId);
			if (lack > 0)
			{
				newItems.push_back(RanchCommon(item.itemId, lack, item.price));
			}
		}

		return newItems;
	}

	bool UICommon::checkBirdMiniTip(const BirdScope& scope)
	{
		return scope.id > 0 && !RootGuiLayout::Instance()->isBirdViewShow();
	}

	bool UICommon::checkBirdConvert()
	{
		return checkBirdParam() && Blockman::Instance()->m_pPlayer->m_birdSimulator->isConvert();
	}

	String UICommon::getBirdMiniTipText(const BirdScope& scope)
	{
		switch (scope.type)
		{
		case 1:
			return LanguageManager::Instance()->getString("gui_bird_tip_open_store");
		case 2:
		{
			String title = LanguageManager::Instance()->getString("gui_bird_mini_tip_open");
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(scope.id));
			if (!entity)
			{
				return  StringUtil::Format(title.c_str(), "npc");
			}
			return StringUtil::Format(title.c_str(), entity->getName().c_str());
		}
		case 3:
			return LanguageManager::Instance()->getString("gui_bird_mini_tip_open_lottery");
		case 4:
			return LanguageManager::Instance()->getString(checkBirdConvert() ? "gui_bird_mini_tip_stop_convert" : "gui_bird_mini_tip_convert");
		case 5:
			return LanguageManager::Instance()->getString("gui_bird_tip_open_chest");
		case 6:
			return LanguageManager::Instance()->getString("gui_bird_tip_open_cannon");
		case 7:
			return LanguageManager::Instance()->getString("dead.summary.rank");
		case 8:
			return LanguageManager::Instance()->getString("gui_bird_tip_open_egg_chest");
		}
		return "";
	}

	void UICommon::onMiniTipClick()
	{
		if (!checkBirdParam())
		{
			return;
		}
		BirdScope  item = Blockman::Instance()->m_pPlayer->m_birdSimulator->getScopeByPosition(Blockman::Instance()->m_pPlayer->position);
		switch (item.type)
		{
		case 1:
			ShowBirdStoreEvent::emit(item.id, 0);
			break;
		case 2:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(item.id));
			if (!entity)
			{
				return;
			}
			ShowBirdTaskTipEvent::emit(entity->getSessionContent());
		}
			break;
		case 3:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(item.id));
			if (!entity)
			{
				return;
			}
			ShowBirdLotteryByNpcEvent::emit(entity->entityId);
		}
			break;
		case 4:
			ClientNetwork::Instance()->getSender()->sendBirdBagOperation(0, 6);
			break;
		case 5:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(item.id));
			if (!entity)
			{
				return;
			}
			Blockman::Instance()->m_pPlayer->attackSessionNpc(entity);
		}
		case 6:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntityActorCannonClient*>(dataCache->getEntityByServerId(item.id));
			if (!entity)
			{
				return;
			}
			entity->m_hasBeenLaunched = true;
			ClientNetwork::Instance()->getSender()->sendUseCannon(entity->entityId);
			SoundSystem::Instance()->playEffectByType(ST_Cannon);
		}
		case 7:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntityBulletin*>(dataCache->getEntityByServerId(item.id));
			if (!entity)
			{
				return;
			}
			String content = LanguageManager::Instance()->getString("gui_bird_tip_my_rank");
			content = StringUtil::Format(content.c_str(), numberToString(entity->getRank().score).c_str(), entity->getRank().rank);
			vector<String>::type tips = vector<String>::type();
			tips.push_back(content);
			ShowBirdNpcCommonTipEvent::emit(entity->getNameLang(), tips);
		}
		case 8:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(item.id));
			if (!entity)
			{
				return;
			}
			Blockman::Instance()->m_pPlayer->attackSessionNpc(entity);
		}
		break;
		}

	}

	bool UICommon::checkPlayerParam()
	{
		return Blockman::Instance() && Blockman::Instance() && Blockman::Instance()->m_pPlayer;
	}

	bool UICommon::checkWalletParam()
	{
		return checkPlayerParam() && Blockman::Instance()->m_pPlayer->m_wallet;
	}

	bool UICommon::checkRanchParam()
	{
		return checkPlayerParam() && Blockman::Instance() && Blockman::Instance()->m_pPlayer->m_ranch;
	}

	bool UICommon::checkBirdParam()
	{
		return checkPlayerParam() && Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_pPlayer->m_birdSimulator;
	}

	bool UICommon::checkBirdBagParam()
	{
		return checkBirdParam() && Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
	}

	bool UICommon::checkBlockCityParam()
	{
		return checkPlayerParam() && Blockman::Instance()->m_pPlayer->m_blockCity;
	}

	bool UICommon::checkBlockFortParam()
	{
		return checkPlayerParam() && Blockman::Instance()->m_pPlayer->m_blockFort;
	}

	bool UICommon::checkItemPlaceInManor(Vector3 beginPos, Vector3 endPos)
	{
		bool isCanPlace = false;
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::BlockCity:
			if (Blockman::Instance()->m_pPlayer->m_blockCity && Blockman::Instance()->m_pPlayer->m_blockCity->getManorInfo())
			{
				Vector3 canPlaceStartPos = Blockman::Instance()->m_pPlayer->m_blockCity->getManorInfo()->canPlaceStartPos;
				Vector3 canPlaceEndPos = Blockman::Instance()->m_pPlayer->m_blockCity->getManorInfo()->canPlaceEndPos;
				isCanPlace = checkRangeLimit(beginPos, endPos, canPlaceStartPos, canPlaceEndPos);
			}
			break;
		case ClientGameType::BlockFort:
			if (Blockman::Instance()->m_pPlayer->m_blockFort && Blockman::Instance()->m_pPlayer->m_blockFort->getManorInfo())
			{
				Vector3 canPlaceStartPos = Blockman::Instance()->m_pPlayer->m_blockFort->getManorInfo()->canPlaceStartPos;
				Vector3 canPlaceEndPos = Blockman::Instance()->m_pPlayer->m_blockFort->getManorInfo()->canPlaceEndPos;
				isCanPlace = ((WorldClient*)Blockman::Instance()->m_pWorld)->checkRangeBlockIsAir(beginPos, endPos + 1.f) && checkRangeLimit(beginPos, endPos, canPlaceStartPos, canPlaceEndPos);
			}
			break;
		}
		return isCanPlace;
	}

	bool UICommon::checkRangeLimit(Vector3 beginPos, Vector3 endPos, Vector3 rangeBeginPos, Vector3 rangeEndPos)
	{
		return rangeBeginPos.x <= beginPos.x && beginPos.x <= rangeEndPos.x &&
			rangeBeginPos.y <= beginPos.y && beginPos.y <= rangeEndPos.y &&
			rangeBeginPos.z <= beginPos.z && beginPos.z <= rangeEndPos.z &&
			rangeBeginPos.x <= endPos.x   && endPos.x <= rangeEndPos.x &&
			rangeBeginPos.y <= endPos.y   && endPos.y <= rangeEndPos.y &&
			rangeBeginPos.z <= endPos.z   && endPos.z <= rangeEndPos.z;
	}

	bool UICommon::checkBlockCityMiniTip()
	{
		return !RootGuiLayout::Instance()->isShowBlockCityView();
	}

	void UICommon::onBlockCityMiniTipClick()
	{
		if (!checkBlockCityParam())
		{
			return;
		}

		auto scope = ActionAreaManager::ActionAreaByPosition(Blockman::Instance()->m_pPlayer->position);
		if (!scope)
		{
			return;
		}

		switch ((AreaType)scope->type)
		{
		case AreaType::BLOCK_CITY_TIGER_LOTTERY:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(scope->id));
			if (!entity)
			{
				return;
			}
			Blockman::Instance()->m_pPlayer->attackSessionNpc(entity);
		}
		break;

		case AreaType::BLOCK_CITY_CANNON:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntityActorCannonClient*>(dataCache->getEntityByServerId(scope->id));
			if (!entity || Blockman::Instance()->m_pPlayer->isOnVehicle())
			{
				return;
			}
			entity->m_hasBeenLaunched = true;
			ClientNetwork::Instance()->getSender()->sendUseCannon(entity->entityId);
			SoundSystem::Instance()->playEffectByType(ST_Cannon);
		}
		break;

		case AreaType::BLOCK_CITY_MAILBOX:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(scope->id));
			if (!entity)
			{
				return;
			}

			// todo : show MailBox UI

		}
		break;

		case AreaType::BLOCK_CITY_DECORATION:
		case AreaType::BLOCK_CITY_PUBLIC_CHAIR:
		{
			auto dataCach = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntityDecoration *>(dataCach->getEntityByServerId(scope->id));
			if (entity)
			{
				interactionWithDecoration(entity);
			}
		}
		break;

		case AreaType::BLOCK_CITY_MANAGER:
		{
			auto dataCache = ClientNetwork::Instance()->getDataCache();
			auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(scope->id));
			if (!entity)
			{
				return;
			}
			Blockman::Instance()->m_pPlayer->attackSessionNpc(entity);

		}
		break;

		case AreaType::BLOCK_CITY_HOUSE_SHOP:
		{
			ShowBlockCityHouseShopEvent::emit(scope->id, 0);
		}
		break;
		default:
			break;
		}
	}

	void UICommon::interactionWithDecoration(EntityDecoration * entity)
	{
		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entity);
		if (!actor || !actor->getHasInited())
			return;
		auto nodes = entity->getNodes();
		map<String, float>::type nodeDis = map<String, float>::type();
		auto playerPosition = Blockman::Instance()->m_pPlayer->position;
		for (const auto item : nodes)
		{
			if (item.second == 0)
			{
				SceneNode* node = actor->getSocketSceneNode(item.first);
				nodeDis.insert({ item.first, node->getWorldPosition().distanceTo(playerPosition)});
			}
		}

		String nodeStr = "";
		float dis = 0.f;
		int index = 0;
		for (auto& node : nodeDis)
		{
			if (index == 0 || node.second < dis)
			{
				nodeStr = node.first;
				dis = node.second;
				index++;
			}
		}

		if (!Blockman::Instance()->m_pPlayer->isOnDecoration())
		{
			Blockman::Instance()->setPersonView(1);
			SoundSystem::Instance()->playEffectByType(ST_sit_down);
		}

		ClientNetwork::Instance()->getCommonSender()->sendInteractionWithDecoration(entity->hashCode(), nodeStr);
	}

	bool UICommon::checkBlockCityFlyBtnShow()
	{
		if (checkBlockCityParam() && Blockman::Instance()->m_pPlayer->m_blockCity->getPlayerInfo())
		{
			const auto &isCanFlying = Blockman::Instance()->m_pPlayer->m_blockCity->getPlayerInfo()->isCanFlying;
			const auto &isEditMode = Blockman::Instance()->m_pPlayer->m_blockCity->getPlayerInfo()->isEditMode;
			return !isCanFlying && isEditMode && checkBlockCityIsInManor();
		}

		return false;
	}

	bool UICommon::checkBlockCityEditModeBtnShow()
	{
		return checkBlockCityIsInManor();
	}

	bool UICommon::checkBlockCityIsInManor()
	{
		if (checkBlockCityParam() && Blockman::Instance()->m_pPlayer->m_blockCity->getManorInfo())
		{
			const auto &startPos = Blockman::Instance()->m_pPlayer->m_blockCity->getManorInfo()->startPos;
			const auto &endPos = Blockman::Instance()->m_pPlayer->m_blockCity->getManorInfo()->endPos;
			const auto &pos = Blockman::Instance()->m_pPlayer->position;

			return (pos.x >= startPos.x) && (pos.x <= endPos.x) && (pos.y >= startPos.y) && (pos.y <= endPos.y) && (pos.z >= startPos.z) && (pos.z <= endPos.z);
		}

		return false;
	}

	bool UICommon::checkBlockFortIsInManor()
	{
		if (checkBlockFortParam() && Blockman::Instance()->m_pPlayer->m_blockFort->getManorInfo())
		{
			const auto &startPos = Blockman::Instance()->m_pPlayer->m_blockFort->getManorInfo()->startPos;
			const auto &endPos = Blockman::Instance()->m_pPlayer->m_blockFort->getManorInfo()->endPos;
			const auto &pos = Blockman::Instance()->m_pPlayer->position;

			return (pos.x >= startPos.x) && (pos.x <= endPos.x) && (pos.y >= startPos.y) && (pos.y <= endPos.y) && (pos.z >= startPos.z) && (pos.z <= endPos.z);
		}

		return false;
	}
	
	ui64 UICommon::checkBlockCityManorUserId()
	{
		if (checkBlockCityParam())
		{
			Vector3 position = Blockman::Instance()->m_pPlayer->position;
			auto manorAreas = Blockman::Instance()->m_pPlayer->m_blockCity->getManorArea();

			for (const auto &area : manorAreas)
			{
				if (position.x > area.startPos.x && position.y > area.startPos.y && position.z > area.startPos.z && 
					position.x < area.endPos.x && position.y < area.endPos.y && position.z < area.endPos.z && 
					area.userId != Blockman::Instance()->m_pPlayer->getPlatformUserId())
				{
					return area.userId;
				}
			}
		}

		return 0;
	}

	i64 UICommon::getOtherCurrency()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::BirdSimulator:
			return checkBirdParam() ? Blockman::Instance()->m_pPlayer->m_birdSimulator->getEggTicket() : 0;
		default:
			return 999999999999;
		}
	}

	String UICommon::getCurrencyIcon()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::RanchersExplore:
			return "set:ranchersexplore.json image:dianshu";
		case ClientGameType::BirdSimulator:
			return "set:bird_main.json image:fruit_currency";
		case ClientGameType::PixelGunHall:
		case ClientGameType::PixelGunGameTeam:
		case ClientGameType::PixelGunGamePerson:
		case ClientGameType::PixelGunGame1v1:
		case ClientGameType::PixelGunChicken:
			return "set:pixelgungameresult.json image:gold";
		case ClientGameType::SkyBlockMain:
		case ClientGameType::SkyBlockMineCave:
		case ClientGameType::SkyBlockProduct:
			return "set:items.json image:currency_money";
		case ClientGameType::BlockCity:
			return "set:blockcity_main_ui.json image:r_currency";
		default:
			return "set:jail_break.json image:jail_break_currency";
		}
	}

	String UICommon::getKeyIcon()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::PixelGunHall:
		case ClientGameType::PixelGunGameTeam:
		case ClientGameType::PixelGunGamePerson:
		case ClientGameType::PixelGunGame1v1:
			return "set:chest_lottery.json image:ic_key";
		case ClientGameType::BED_WAR_HALL:
			return "set:app_shop.json image:bedwar_key";
		default:
			return "";
		}
	}

	String UICommon::getOtherCurrencyIcon()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::BirdSimulator:
			return "set:bird_main.json image:egg_ticket_mini";
		case ClientGameType::PixelGunHall:
		case ClientGameType::PixelGunGameTeam:
		case ClientGameType::PixelGunGamePerson:
		case ClientGameType::PixelGunGame1v1:
			return "set:pixelgungameresult.json image:chip";
		case ClientGameType::BED_WAR_HALL:
			return "set:dress_shop.json image:chip";
		default:
			return "";
		}
	}

	String UICommon::getPlatformGold()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::BlockCity:
			return "set:blockcity_manor.json image:money_icon";
		case ClientGameType::BlockFort:
			return "set:blockfort_common.json image:money_icon";
		default:
			return "set:app_shop.json image:app_shop_gold";
		}
	}

	String UICommon::getCurrencyIconByType(i32 currencyType)
	{
		return getCurrencyIconByType((CurrencyType) currencyType);
	}

	String UICommon::getCurrencyIconByType(CurrencyType currencyType)
	{
		switch (currencyType)
		{
		case CurrencyType::PLATFORM_MONEY:
			return"set:app_shop.json image:Diamond-icon-gold";
		case CurrencyType::PLATFORM_GOLD:
			return getPlatformGold();
		case CurrencyType::GAME_MONEY:
			return getCurrencyIcon();
		case CurrencyType::OTHER_MONEY:
			return getOtherCurrencyIcon();
		case CurrencyType::KEY_MONEY:
			return getKeyIcon();
		default:
			return "";
		}
	}

	String UICommon::timeFormat(i32 time)
	{
		String h = StringUtil::ToString(time / 1000 / 60 / 60);
		h = (h.length() == 1 ? ("0" + h) : h);
		String m = StringUtil::ToString(time / 1000 / 60 % 60);
		m = (m.length() == 1 ? ("0" + m) : m);
		String s = StringUtil::ToString(time / 1000 % 60);
		s = (s.length() == 1 ? ("0" + s) : s);
		return StringUtil::Format("%s:%s:%s", h.c_str(), m.c_str(), s.c_str());
	}

	String UICommon::luckyBlockTimeFormat(i32 time)
	{
		String s = StringUtil::ToString(time / 1000 );
		return StringUtil::Format("%s",s.c_str());
	}

	String UICommon::numberToString(i64 num)
	{
		String str = StringUtil::ToString(num);
		if (str.length() < 4)
		{
			return str;
		}
		String newStr = str;
		for (int i = 0; i < str.length() / 3; i++)
		{
			int index = newStr.length() - 3 * (i + 1) - i;
			if (index > 0)
			{
				newStr.insert(index, ",");
			}
		}
		return newStr;
	}

}
