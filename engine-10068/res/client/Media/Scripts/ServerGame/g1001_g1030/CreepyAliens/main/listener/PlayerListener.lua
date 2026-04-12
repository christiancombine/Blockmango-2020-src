--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "config.RespawnConfig"
require "Match"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerAttackCreatureEvent.registerCallBack(self.onPlayerAttackCreature)
    PlayerUpdateRealTimeRankEvent.registerCallBack(self.onPlayerUpdateRealTimeRank)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerBuySwitchablePropEvent.registerCallBack(self.onPlayerBuySwitchableProp)
    PlayerBuyUpgradePropEvent.registerCallBack(self.onPlayerBuyUpgradeProp)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerWatchRespawnEvent.registerCallBack(self.onPlayerWatchRespawn)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    local callback = function()
        if PlayerManager:getPlayerCount() == 1 and GameMatch.isFirstReady == false then
            GameMatch.isFirstReady = true
        end
    end
    return GameOverCode.Success, player, 1, callback

end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    GameMatch:getDBDataRequire(player)
    RankNpcConfig:getPlayerRankData(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 86400
end

function PlayerListener.onPlayerRespawn(player)
    player:initPlayer()
    MsgSender.sendCenterTipsToTarget(player.rakssid, 1, " ")
    return 86400
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local player = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if player == nil then
        return true
    end
    player.isDead = true
    MsgSender.sendCenterTipsToTarget(diePlayer:getRaknetID(), 180, Messages:autoRespawnHint())
    RespawnConfig:showBuyRespawn(diePlayer:getRaknetID(), 0)
    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerAttackCreature(rakssid, entityId, attackType)
    if attackType == 0 then
        return false
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    local monster = GameMatch:getCurGameScene():getMonster(entityId)
    if monster == nil then
        return false
    end
    monster:onAttacked(player)
    GameMatch:ifCanUpgradeGameLevel()
    return true
end

function PlayerListener.onPlayerUpdateRealTimeRank(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    GameMatch:sendRealRankData(player)
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    return false
end

function PlayerListener.onPlayerBuySwitchableProp(rakssid, uniqueId, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local arm = ArmsConfig:getArmById(uniqueId)
    if arm == nil then
        return
    end
    if player:hasArm(arm.level) then
        player:useArm(arm.level)
        player:sendArmsData()
        return
    end
    if player:buyArm(arm.level) then
        player:sendArmsData()
    end
end

function PlayerListener.onPlayerBuyUpgradeProp(rakssid, uniqueId, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local equipment = EquipmentConfig:getEquipmentById(uniqueId)
    if equipment == nil then
        return
    end
    if player:upgradeEquipment(equipment.part) then
        player:sendEquipmentData()
    end
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if itemId == ItemID.PROP_GOLD then
        if player.isGoldAddition then
            msg.value = Messages:hasBuy()
            return false
        else
            isAddItem.value = false
            player.isGoldAddition = true
            HostApi.notifyGetItem(player.rakssid, itemId, 0, 1)
            return true
        end
    end
    if itemId == ItemID.PROP_EXP then
        if player.isExpAddition then
            msg.value = Messages:hasBuy()
            return false
        else
            isAddItem.value = false
            player.isExpAddition = true
            HostApi.notifyGetItem(player.rakssid, itemId, 0, 1)
            return true
        end
    end
    if itemId == ItemID.GOLD_SHOES then
        if player.isSpeedAddition then
            msg.value = Messages:hasBuy()
            return false
        else
            isAddItem.value = false
            player.isSpeedAddition = true
            player:addEffect(PotionID.MOVE_SPEED, 60 * 60 * 24 * 30, 1)
            HostApi.notifyGetItem(player.rakssid, itemId, 0, 1)
            return true
        end
    end
    return true
end

function PlayerListener.onPlayerWatchRespawn(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    RespawnConfig:showBuyRespawn(rakssid, 0)
end

return PlayerListener
--endregion
