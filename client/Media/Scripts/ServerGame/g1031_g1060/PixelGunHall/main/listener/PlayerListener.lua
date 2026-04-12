--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "messages.Messages"
require "data.GamePlayer"
require "data.GameChestLottery"
require "data.GameReward"
require "data.GameArmor"
require "data.GameAppShop"
require "data.GameSignIn"
require "data.GiftBagController"
require "Match"

PlayerListener = {}

function PlayerListener:init()

    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerBuyGoodsSuccessEvent.registerCallBack(self.onPlayerBuyGoodsSuccess)
    PlayerDropItemEvent.registerCallBack(function()
        return false
    end)
    SendGuideInfoEvent.registerCallBack(self.onSendGuideInfo)
    WatchAdvertFinishedEvent.registerCallBack(self.onWatchAdvertFinished)

    PlayerStartGameEvent:registerCallBack(self.onPlayerStartGame)
    PlayerUpgradeArmorEvent:registerCallBack(self.onPlayerUpgradeArmor)
    PlayerUnlockMapEvent:registerCallBack(self.onPlayerUnlockMap)
    PlayerOpenLotteryChestEvent:registerCallBack(self.onPlayerOpenLotteryChest)
    PlayerReceiveSignInEvent:registerCallBack(self.onPlayerReceiveSignInReward)
    PlayerBuyGiftBagEvent:registerCallBack(self.onPlayerBuyGiftBag)
    PlayerRefreshGiftBagEvent:registerCallBack(self.onPlayerRefreshGiftBag)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    DbUtil:getPlayerData(player)
    HostApi.sendPlaySound(player.rakssid, 10036)
    HostApi.setDisarmament(player.rakssid, true)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    NotificationConfig:onPlayerReady(player)
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    return true
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end
    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)
    if player == nil then
        return true
    end
    FuncNpcConfig:onPlayerMove(player, x, y, z)
    player:onMove(x, y, z)
    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerUnlockMap(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local mapId = DataBuilder.new():fromData(data):getNumberParam("mapId")
    local config = ModeSelectMapConfig:getMapInfo(mapId)
    if not config then
        return
    end
    player:consumeDiamonds(1, config.openMoneyNum, Define.UnlockMap .. "#" .. mapId, true)
end

function PlayerListener.onPlayerOpenLotteryChest(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local chestId = DataBuilder.new():fromData(data):getNumberParam("chestId")
    GameChestLottery:onPlayerOpenLotteryChest(player, chestId)
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    isAddItem.value = false
    return GameAppShop:CanBuyGoods(player, itemId)
end

function PlayerListener.onPlayerBuyGoodsSuccess(rakssid, type, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    GameAppShop:onPlayerShopping(player, itemId)
end

function PlayerListener.onPlayerUpgradeArmor(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end
    local operateType = DataBuilder.new():fromData(data):getNumberParam("operateType")
    GameArmor:upgradeArmor(player, operateType)
end

function PlayerListener.onPlayerStartGame(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end
    local builder = DataBuilder.new():fromData(data)
    local gameType = builder:getParam("gameType")
    local mapName = builder:getParam("mapName")
    local _gameType, _mapId, _is_random = ModeSelectConfig:getFightMapInfo(player, gameType, mapName)

    if _gameType and _mapId then
        if _is_random then
            player.cur_is_random = 1
        end
        player.cur_map_id = _mapId
        EnterRoomManager:addEnterRoomQueue(player, tostring(_gameType), tostring(_mapId))
    end
end

function PlayerListener.onSendGuideInfo(rakssid, guideId, guideStatus)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    local guide = { Id = guideId, Status = guideStatus }
    GameGuide:updateStatus(player, guide)
end

function PlayerListener.onWatchAdvertFinished(rakssid, type, params, code)
    if code ~= 1 then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if type == Define.AdType.Gold then
        AdvertConfig:doGoldAdReward(player)
        return
    end
    if type == Define.AdType.Shop then
        AdvertConfig:doShopAdReward(player)
        return
    end
    if type == Define.AdType.Gift then
        AdvertConfig:doGiftAdReward(player)
        return
    end
    if type == Define.AdType.Task then
        AdvertConfig:doTaskAdReward(player, params)
        return
    end
    if type == Define.AdType.FreeChest then
        AdvertConfig:doChestAdReward(player)
        return
    end
    if type == Define.AdType.FreeVip then
        AdvertConfig:doVipAdReward(player)
        return
    end
end

function PlayerListener.onPlayerBuyGiftBag(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end
    local giftId = DataBuilder.new():fromData(data):getNumberParam("giftId")
    local gift = GiftBagConfig:getGiftBagById(giftId)
    if gift == nil then
        return false
    end
    if gift.moneyType == Define.Money.DIAMOND then
        player:consumeDiamonds(100000 + giftId, gift.price * gift.discount / 100, Define.BuyGiftBag .. "#" .. giftId)
    elseif gift.moneyType == Define.Money.MONEY then
        player:subCurrency(gift.price * gift.discount / 100)
        player:onBuyGiftBag(giftId)
    end

    return false
end

function PlayerListener.onPlayerRefreshGiftBag(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    if player.giftBagController.isFirst ~= true then
        player.giftBagController:refreshGiftBags()
    end
end

function PlayerListener.onPlayerReceiveSignInReward(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local signInId = DataBuilder.new():fromData(data):getNumberParam("signInId")
    GameSignIn:receiveSignInReward(player, signInId)
end

return PlayerListener
--endregion
