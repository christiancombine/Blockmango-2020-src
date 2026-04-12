--region *.lua
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

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    HostApi.sendPlaySound(clientPeer:getRakssid(), 10021)
    if PlayerManager:getPlayerCount() == 1 then
        GameMatch.gameFirstTick = GameMatch.curTick
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

    if x == 0 and y == 0 and z == 0 then
        return true
    end

    local p = PlayerManager:getPlayerByPlayerMP(player)
    if p ~= nil then
        local otherTeam
        for i, v in pairs(GameMatch.Teams) do
            if v.id ~= p.team.id then
                otherTeam = v
            end
        end
        local vec3 = VectorUtil.newVector3i(x, y, z)
        if p.team:locInFlagArea(vec3) then
            MsgSender.sendMsgToTarget(player:getRaknetID(), Messages:moveFlagArea())
            return false
        end
        if otherTeam ~= nil then
            if otherTeam:locInBornArea(vec3) then
                MsgSender.sendMsgToTarget(player:getRaknetID(), Messages:moveOtherBornArea())
                return false
            end
        end
        if GameMatch:isGameStart() == false then
            if p.team:locInBornArea(vec3) == false then
                MsgSender.sendBottomTipsToTarget(player:getRaknetID(), 2, Messages:moveGameNotStart())
                return false
            end
        end
    end
    return true
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

            if (GameMatch.lastKiller == killer.name) then
                GameMatch.lastKillerKills = GameMatch.lastKillerKills + 1
                if (GameMatch.lastKillerKills >= 5) then
                    MsgSender.sendMsg(IMessages:msgFiveKill(killer:getDisplayName()))
                elseif (GameMatch.lastKillerKills >= 3) then
                    MsgSender.sendMsg(IMessages:msgThirdKill(killer:getDisplayName()))
                elseif (GameMatch.lastKillerKills == 2) then
                    MsgSender.sendMsg(IMessages:msgDoubleKill(killer:getDisplayName()))
                end
            else
                GameMatch.lastKiller = killer.name
                GameMatch.lastKillerKills = 1
            end

            killer:onKill()
        end
    end

    if (dier ~= nil) then
        dier:onDie()
    end

    return true
end

function PlayerListener.onPlayerRespawn(player)
    player:teleInitPos()
    player:resetInv()
    return 43200
end

return PlayerListener
--endregion
