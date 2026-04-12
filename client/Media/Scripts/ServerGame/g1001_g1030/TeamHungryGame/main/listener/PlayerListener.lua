--region *.lua
require "messages.Messages"
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

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerBuyRespawnResultEvent.registerCallBack(self.onBuyRespawnResult)
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
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerMove(player, x, y, z)
    return GameMatch.allowMove
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)

    if (GameMatch.allowPvp == false) then
        return false
    end

    if (hurtFrom == nil) then
        return true
    end

    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)

    if hurter ~= nil and fromer ~= nil then
        if hurter.team.id == fromer.team.id then
            return false
        end
    end

    if hurtValue.value < 1 then
        hurtValue.value = 1
    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)

    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if (iskillByPlayer and killPlayer ~= nil and dier ~= nil) then
        local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)

        if (killer ~= nil) then

            MsgSender.sendMsg(IMessages:msgPlayerKillPlayer(dier:getDisplayName(), killer:getDisplayName()))

            if (GameMatch.firstKill == false) then
                GameMatch.firstKill = true
                MsgSender.sendMsg(IMessages:msgFirstKill(killer:getDisplayName()))
            end

            killer:onKill()

        end
    end

    if (dier ~= nil) then
        GameMatch:onPlayerDie(dier)
    end

    return true
end

function PlayerListener.onPlayerRespawn(player)
    if GameMatch.curStatus == GameMatch.Status.GameTime then
        player:teleInitPos()
    end
    if GameMatch.curStatus == GameMatch.Status.FinaFightTime then
        player:teleffPos()
    end
    return 43200
end

function PlayerListener.onBuyRespawnResult(rakssid, code)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        GameMatch:ifGameOver()
        return
    end
    if code == 0 then
        player:onDie()
        GameMatch:ifGameOver()
    end
    if code == 1 then
        MsgSender.sendMsg(IMessages:msgRespawn(player:getDisplayName()))
    end
end

return PlayerListener
--endregion
