--region *.lua
require "messages.Messages"
require "util.GameManager"
require "util.RealRankUtil"
require "config.TeamConfig"
require "config.BasicPropConfig"
require "data.GamePlayer"
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
    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerAttackCreatureEvent.registerCallBack(self.onPlayerAttackCreature)
    PlayerUpdateRealTimeRankEvent.registerCallBack(self.onPlayerUpdateRealTimeRank)
    PlayerBuySuperPropEvent.registerCallBack(self.onPlayerBuySuperProp)
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
     RankNpcConfig:getPlayerRankData(player)
     GameConfig:syncMerchants(player)
     MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if GameMatch.hasStartGame == false then
        return false
    end
    if damageType == "outOfWorld" then
        hurtValue.value = 10000
        return true
    end
    if hurtFrom == nil then
        return false
    end

    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    local attacker = PlayerManager:getPlayerByPlayerMP(hurtFrom)

    if hurter == nil or attacker == nil then
        return false
    end

    if hurter:getTeamId() == attacker:getTeamId() then
        return false
    end
    local itemId = attacker:getHeldItemId()
    if itemId == 0 or itemId == 757 or itemId == 274
    or itemId == 257 or itemId == 285 or itemId == 278 then
        hurtValue.value = 20
    else
        hurtValue.value = attacker.attack * (1 - hurter.defenseX) * AttackXConfig:getAttackX(attacker.type, hurter.type)
    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier == nil then
        return true
    end
    dier:onDie()

    if killPlayer == nil then
        return true
    end
    local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
    if killer == nil then
        return true
    end

    MsgSender.sendTopTips(3, Messages:msgKillTip(killer:getDisplayName(), dier:getDisplayName()))
    MsgSender.sendMsg(Messages:msgKillTip(killer:getDisplayName(), dier:getDisplayName()))
    killer:onKill(dier)
    return true
end

function PlayerListener.onPlayerRespawn(player)
    player:teleRebirthPosAndYaw()
    return 43200
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currncy)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    return true
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    isAddItem.value = true
    return true
end

function PlayerListener.onPlayerUpdateRealTimeRank(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    RealRankUtil:sendRealRankData(player)
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    return false
end

function PlayerListener.onPlayerAttackCreature(rakssid, entityId, attackType)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    local npc = GameManager:getNpcByEntityId(entityId)
    if npc == nil then
        return false
    end
    return npc:onPlayerHit(player, attackType)
end

function PlayerListener.onPlayerBuySuperProp(rakssid, uniqueId, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local distance = player:getTeamDistance()
    if distance > 15 then
        msg.value = Messages:msgDontBuyOcc()
        return
    end
    if player:tryChangeOccupation(uniqueId) then
        player:sendOccupationData(uniqueId)
        return
    end
end

return PlayerListener
--endregion
