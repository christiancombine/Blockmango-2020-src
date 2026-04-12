--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "util.Lottery"
require "Match"

PlayerListener = {}

function PlayerListener:init()

    BaseListener.registerCallBack(PlayerLoginEvent,self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent,self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent,self.onPlayerReady)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerBuySwitchablePropEvent.registerCallBack(self.onPlayerBuySwitchableProp)
    PlayerAttackActorNpcEvent.registerCallBack(self.onPlayerAttackActorNpc)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerHallLotteryAgainEvent.registerCallBack(self.onPlayerHallLotteryAgain)

end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    return false
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    isAddItem.value = true
    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerMove(Player, x, y, z)
    local player = PlayerManager:getPlayerByPlayerMP(Player)
    if player == nil then
        return true
    end

    player:onMove(x, y, z)
    return true
end

function PlayerListener.onPlayerReady(player)
    DbUtil:getDBDataRequire(player, DbUtil.MONEY_TAG)
    DbUtil:getDBDataRequire(player, DbUtil.ACTORS_TAG)
    HostApi.changePlayerPerspece(player.rakssid, 1)
    RankNpcConfig:getPlayerRankData(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerBuySwitchableProp(rakssid, uniqueId, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local actor = ActorsConfig:getActorById(uniqueId)
    if actor == nil then
        return
    end

    if player:hasActor(actor.id) then
        player:useActor(actor.id)
        player:sendActorsData()
        return
    end
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

function PlayerListener.onPlayerHallLotteryAgain(rakssid, times, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local npc = GameManager:getNpcByEntityId(entityId)

    if npc then
        if npc.times == times then
            npc:onPlayerSelected(player)
        end
    end
end

return PlayerListener
--endregion
