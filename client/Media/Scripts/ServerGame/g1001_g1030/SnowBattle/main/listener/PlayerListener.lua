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
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerUseThrowableItemEvent.registerCallBack(self.onUseThrowableItem)
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

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)

    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)

    if hurtFrom ~= nil then
        local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)

        if os.time() - hurter.lastSpawn < 5 then
            return false
        end

        if hurter ~= nil and fromer ~= nil then
            if hurter.team.id == fromer.team.id then
                return false
            end
        end
    end

    if damageType == "outOfWorld" then
        return true
    end

    if damageType == "explosion.player" or damageType == "explosion" then

        if hurter ~= nil then
            local hp = hurter:getHealth()
            if hp - hurtValue.value <= 0 then
                hurter:clearInv() --- clear inv before death
            end
        end

        return true
    end

    if damageType == "thrown" then
        hurtValue.value = 4
    else
        hurtValue.value = 0
    end

    if hurter ~= nil then
        local hp = hurter:getHealth()
        if hp - hurtValue.value <= 0 then
            hurter:clearInv() --- clear inv before death
        end
    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)

    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)

    if (dier ~= nil) then

        if GameMatch:isGameStart() == false then
            return true
        end
        dier:clearInv()
        dier:onDie()

        local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
        if iskillByPlayer and killer.rakssid ~= dier.rakssid then
            if killer then
                killer:onKill()
            end
        end
    end
    GameMatch:ifGameOver()

    return true
end

function PlayerListener.onUseThrowableItem(itemId)
    if GameMatch:isGameStart() then
        return true
    end
    return false
end

function PlayerListener.onPlayerReSpawn(player)
    player:teleRandomPos()
    player:addItem(269, 1, 0)
    player.lastSpawn = os.time()
    return 43200
end

return PlayerListener
--endregion
