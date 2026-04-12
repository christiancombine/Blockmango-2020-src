--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "Match"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerBuySwitchablePropEvent.registerCallBack(self.onPlayerBuySwitchableProp)
    PlayerBuyUpgradePropEvent.registerCallBack(self.onPlayerBuyUpgradeProp)
    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
end

function PlayerListener.onPlayerLogin(clientPeer)
    if GameMatch.hasStartGame then
        return GameOverCode.GameStarted
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    DbUtil:getPlayerData(player)
    RankNpcConfig:getPlayerRankData(player)
    GameConfig:syncMerchants(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if GameMatch:isGameRunning() == false then
        return false
    end
    local hunter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if hunter == nil then
        return false
    end
    if hunter.status ~= 0 then
        return false
    end
    if hurtFrom == nil then
        return false
    end
    local attacker = PlayerManager:getPlayerByPlayerMP(hurtFrom)
    if attacker == nil then
        return false
    end
    if attacker.status ~= 0 then
        return false
    end
    if damageType == "thrown" then
        hurtValue.value = 2
    end
    local damage = hurtValue.value
    damage = damage + attacker.attack * 0.5 - hunter.defense * 0.5
    hurtValue.value = math.max(1, damage)
    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier == nil then
        return true
    end
    SiteConfig:onPlayerDie(dier)
    return false
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerBuySwitchableProp(rakssid, uniqueId, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local title = TitleConfig:getTitle(uniqueId)
    if title == nil then
        return
    end
    if player:hasTitle(title) then
        player:useTitle(title)
        player:sendTitleData()
        return
    end
    if player:buyTitle(title) then
        player:sendTitleData()
    end
end

function PlayerListener.onPlayerBuyUpgradeProp(rakssid, uniqueId, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local talent = TalentConfig:getTalent(uniqueId)
    if talent == nil then
        return
    end
    if player:upgradeTalent(talent.type) then
        player:sendTalentData()
    end
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local goodsId = goodsInfo.goodsId
    local goodsNum = goodsInfo.goodsNum
    local coinId = goodsInfo.coinId
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if type == 13 then
        attr.isAddGoods = false
        attr.isConsumeCoin = false
        local inv = player:getInventory()
        if inv then
            local num = inv:getItemNum(goodsId)
            if num > 0 then
                local itemId = GameConfig:getItemIdByCoinId(coinId)
                player:removeItem(goodsId, 1)
                player:addItem(itemId, goodsNum, 0)
                HostApi.notifyGetItem(player.rakssid, itemId, 0, goodsNum)
                return true
            else
                msg.value = Messages:msgNoProps()
                return false
            end
        else
            return false
        end
    end
    if type == 14 then
        local equipmentSet = EquipmentSetConfig:getEquipmentSet(goodsId)
        if equipmentSet == nil then
            return false
        end
        attr.isAddGoods = false
        for _, item in pairs(equipmentSet.items) do
            player:addItem(item.itemId, item.num, 0)
        end
        return true
    end
    return true
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    return false
end

return PlayerListener
--endregion
