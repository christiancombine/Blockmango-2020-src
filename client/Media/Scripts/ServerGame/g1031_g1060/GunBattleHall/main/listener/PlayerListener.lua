--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "config.GunsConfig"
require "Match"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerAttackActorNpcEvent.registerCallBack(self.onPlayerAttackActorNpc)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerBuyCustomPropEvent.registerCallBack(self.onPlayerBuyCustomProp)
    PlayerSelectRoleEvent.registerCallBack(self.onPlayerSelectRole)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
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
    RankNpcConfig:getPlayerRankData(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerMove(Player, x, y, z)
    local player = PlayerManager:getPlayerByPlayerMP(Player)
    if player == nil then
        return true
    end
    player:onMove(x, y, z)
    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    return false
end

function PlayerListener.onPlayerAttackActorNpc(rakssid, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local npc = GameManager:getNpcByEntityId(entityId)
    if npc ~= nil then
        npc:onPlayerHit(player)
    end
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    return true
end

function PlayerListener.onPlayerBuyCustomProp(rakssid, propId, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local gun = GunsConfig:getGunById(propId)
    if gun ~= nil then
        if player:updateGunRandomForgeValue(gun) then
            MsgSender.sendMsgToTarget(rakssid, Messages:msgGunForgingSuccessTip())
        else
            MsgSender.sendMsgToTarget(rakssid, Messages:msgGunForgingFailureTip())
        end
        return
    end
    local sprayPaint = SprayPaintConfig:getSprayPaintById(propId)
    if sprayPaint ~= nil then
        if player:updateGunSprayPaint(sprayPaint) then
            MsgSender.sendMsgToTarget(rakssid, Messages:msgSuccessBuySPTip())
        else
            MsgSender.sendMsgToTarget(rakssid, Messages:msgPurchaseFailedTip())
        end
    end
    local playerActor = SpecialClothingConfig:getSpecialClothingById(propId)
    if playerActor ~= nil then
        if player:updatePlayerActor(playerActor) then
            MsgSender.sendMsgToTarget(rakssid, Messages:msgSuccessBuySPActorTip())
        else
            MsgSender.sendMsgToTarget(rakssid, Messages:msgFailedBuySPActorTip())
        end
    end
end

function PlayerListener.onPlayerSelectRole(rakssid, role)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if role == 1 then
        player.teamId = 2
    else
        player.teamId = 1
    end
    local npc = GameManager:getModeNpcByGameType(player.gameType)
    if npc ~= nil then
        npc:onPlayerSelectRole(player)
    end
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

return PlayerListener
--endregion
