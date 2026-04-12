--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "data.GameMerchant"
require "config.BackpackConfig"
require "config.MerchantConfig"
require "Match"

PlayerListener = {}

function PlayerListener:init()
    DBManager:setPlayerDBSaveFunction(function(player, immediate)
        GameMatch:savePlayerData(player, immediate)
    end)

    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(GetDataFromDBEvent, self.onGetDataFromDB)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerClickTeleportEvent.registerCallBack(self.onClickTeleportEvent)
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
    HostApi.sendPlaySound(player.rakssid, 10013)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    GameMatch:getPlayerRankData(player)
    GameMatch:getDBDataRequire(player)
    return 86400
end

function PlayerListener.onPlayerMove(player, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end

    if y == 1 and GameMatch.isStartCrash == false then
        GameMatch.isStartCrash = true
    end

    if MerchantConfig.location:inArea(VectorUtil.newVector3(x, y, z)) == true then
        local p = PlayerManager:getPlayerByPlayerMP(player)
        if p ~= nil then
            if p.score ~= 0 then
                local money = p.score * MerchantConfig.exchangeMultiple
                p:addMoney(money)
                p.score = 0
                local backpack = BackpackConfig:getBackpackByLevel(p.backpackLevel)
                p:resetBackpack(p.score, backpack.capacity)
            end
        end
    end
    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    return false
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    return player ~= nil
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local goodsId = goodsInfo.goodsId
    local goodsNum = goodsInfo.goodsNum
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    for i, v in pairs(BackpackConfig.backpack) do
        if goodsId == v.id then
            if player.backpackLevel < BackpackConfig.maxLevel then
                player:upgradePackage(v.id)
                attr.isAddGoods = false
                return true
            else
                return false
            end
        end
    end
    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    return false
end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum, itemEntityId)
    return false
end

function PlayerListener.onGetDataFromDB(player, role, data)
    player:getDataFromDB(data)
end

function PlayerListener.onClickTeleportEvent(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:teleportPos(GameConfig.initPos)
    end
end

return PlayerListener
--endregion
