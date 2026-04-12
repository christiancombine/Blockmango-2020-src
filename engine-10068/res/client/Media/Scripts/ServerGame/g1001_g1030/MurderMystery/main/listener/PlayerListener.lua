--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "data.GamePlayer"
require "Match"
require "messages.Messages"
require "config.ScoreConfig"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerChangeItemInHandEvent.registerCallBack(self.onChangeItemInHandEvent)
    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    local callback = function()
        if GameMatch.curStatus == GameMatch.Status.WaitingPlayer then
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
        end
    end
    return GameOverCode.Success, player, 0, callback
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    HostApi.sendPlaySound(player.rakssid, 10022)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)

    if GameMatch:isGameStart() == false then
        return false
    end

    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)

    if GameMatch:isGameStart() then
        if damageType == "outOfWorld" and hurter ~= nil then
            hurtValue.value = 999
            return true
        end
    end

    if hurtFrom == nil then
        return true
    end

    local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)

    if hurter == nil or fromer == nil then
        hurtValue.value = 0
        return true
    end

    if fromer.onHandItemId == 0 then
        hurtValue.value = 0
        return true
    end

    local hurt = fromer:getArmsHurt()
    if hurt > 0 then
        if hurter.role == GamePlayer.ROLE_CIVILIAN then
            if fromer.role == GamePlayer.ROLE_SHERIFF or fromer.role == GamePlayer.ROLE_CIVILIAN then
                fromer.hp = 0
                fromer:death()
            end
        end
        hurter.hp = hurter.hp - hurt
        if hurter.hp <= 0 then
            hurtValue.value = 999
            return true
        end
        hurter:showGameData()
    end

    hurtValue.value = 0
    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)

    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    local killer = (iskillByPlayer and {PlayerManager:getPlayerByPlayerMP(killPlayer)} or {nil})[1]

    if (dier ~= nil) then
        if dier.role == GamePlayer.ROLE_SHERIFF then
            MsgSender.sendMsg(Messages:sheriffKilled())
            GameMatch:transferSheriff(dier)
            if iskillByPlayer and killer then
                killer.score = killer.score + ScoreConfig.KILL_ENEMY
                killer.scorePoints[ScoreID.KILL] = killer.scorePoints[ScoreID.KILL] + 1
            end
        end
        if dier.role == GamePlayer.ROLE_CIVILIAN then
            local num = GameMatch:getPlayerNumForRole(GamePlayer.ROLE_CIVILIAN) - 1
            MsgSender.sendMsg(Messages:civilianKilled(num))
            if num == 0 then
                MsgSender.sendMsg(Messages:zeroOfCivilian())
            end
            if iskillByPlayer and killer then
                killer.score = killer.score + ScoreConfig.KILL_ENEMY
                killer.scorePoints[ScoreID.KILL] = killer.scorePoints[ScoreID.KILL] + 1
            end
        end
        if dier.role == GamePlayer.ROLE_MURDERER then
            MsgSender.sendMsg(Messages:murdererKilled())
            if iskillByPlayer and killer then
                killer.score = killer.score + ScoreConfig.KILL_MURDER
                killer.scorePoints[ScoreID.KILL] = killer.scorePoints[ScoreID.KILL] + 1
            end
        end
        dier:onDie()
        GameMatch:ifGameOverByPlayer()
    end

    return true
end

function PlayerListener.onPlayerRespawn(player)
    return 43200
end

function PlayerListener.onChangeItemInHandEvent(rakssid, itemId, itemMeta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player.onHandItemId = itemId
    end
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local goodsId = goodsInfo.goodsId
    local goodsNum = goodsInfo.goodsNum
    if GameMatch:isGameStart() == false then
        msg.value = IMessages:msgDontBuyGoodsByNoStart()
        return false
    end
    return true
end

return PlayerListener
--endregion
