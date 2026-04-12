--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "data.GamePlayer"
require "Match"
require "messages.Messages"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerChangeItemInHandEvent.registerCallBack(self.onChangeItemInHandEvent)
end

function PlayerListener.onPlayerLogin(clientPeer)
    if GameMatch.hasStartGame then
        return GameOverCode.GameStarted
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    if GameMatch.startWait == false then
        GameMatch.startWait = true
        WaitingPlayerTask:start()
    end
    if PlayerManager:isPlayerFull() then
        WaitPlayerTask:stop()
    end
    return GameOverCode.Success, player
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    HostApi.sendPlaySound(player.rakssid, 10012)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerMove(player, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end
    local p = PlayerManager:getPlayerByPlayerMP(player)
    if p ~= nil then
        p.positionX = x
        p.positionY = y
        p.positionZ = z
        p.lastMoveTime = tonumber(HostApi.curTimeString())
        if GameMatch:isGameStart() then
            p:addRemoveBlock(x, y, z)
            p:removeFootBlock()
            GameMatch:removeFootBlock()
        end
    end
    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if damageType == "outOfWorld" then
        return true
    end
    hurtValue.value = 0
    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)

    if (dier ~= nil) then
        if GameMatch:isGameStart() == false then
            dier:overGame(true, false)
            return true
        end
        dier:onDie()
        GameMatch:ifGameOver()
    end

    return true
end

function PlayerListener.onChangeItemInHandEvent(rakssid, itemId, itemMeta)

end

return PlayerListener
--endregion
