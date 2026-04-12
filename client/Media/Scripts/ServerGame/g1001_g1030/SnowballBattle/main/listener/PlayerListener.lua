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
    PlayerUseThrowableItemEvent.registerCallBack(self.onUseThrowableItem)
    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerBuyPickupItemEvent.registerCallBack(self.onPlayerBuyPickupItem)
    PlayerBuyPickupItemResultEvent.registerCallBack(self.onPlayerBuyPickupItemResult)
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
    player:teleInitPos()
    HostApi.sendPlaySound(player.rakssid, 10007)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerMove(player, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end
    local p = PlayerManager:getPlayerByPlayerMP(player)
    if p ~= nil then
        if p.team:inArea(VectorUtil.newVector3i(x, y, z)) then
            return true
        end
    end
    return false
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)

    if  hurtFrom ~= nil then
        local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
        local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)
        if hurter ~= nil and fromer ~= nil then
            if hurter.team.id == fromer.team.id then
                return false
            end
        end
    end

    if damageType == "outOfWorld" then
        return true
    end

    if damageType == "thrown" then
        hurtValue.value = 2
    else
        hurtValue.value = 0
    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)

    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)

    local killer

    if killPlayer ~= nil then
        killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
    end

    if killer ~= nil and dier ~= nil and iskillByPlayer then
        killer:onKill()
    end

    if (dier ~= nil) then

        if GameMatch:isGameStart() == false then
            dier:overGame(true, false)
            return true
        end

        dier:onDie()

        GameMatch.curDeaths = GameMatch.curDeaths + 1
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            if v.isLife then
                v.kills = ScoreConfig:getScore(GameMatch.curDeaths, false)
            end
        end

        GameMatch:ifGameOver()

    end

    return true
end

function PlayerListener.onUseThrowableItem(itemId)
    if GameMatch:isGameStart() then
        return true
    end
    return false
end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum, itemEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if player.isPickupItem then
        return
    end

    if itemId == BlockID.TNT then
        local pickUpPrice = GameConfig.TNTPrice.blockymodsPrice
        local moneyType = GameConfig.TNTPrice.coinId
        HostApi.sendPickUpItemOrder(rakssid, itemEntityId, itemId, pickUpPrice, moneyType)
        player.isPickupItemTime = os.time()
        player.isPickupItem = true
        return false
    end

    return true
end

function PlayerListener.onPlayerBuyPickupItem(rakssid, itemId, itemEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if itemId == BlockID.TNT then
        local pickUpPrice = GameConfig.TNTPrice.blockymodsPrice
        local moneyType = GameConfig.TNTPrice.coinId
        player.clientPeer:pickupItemPay(itemId, 0, 1, pickUpPrice, itemEntityId, moneyType)
    end

end

function PlayerListener.onPlayerBuyPickupItemResult(rakssid, buySuccess, itemId, itemMeta, num, price, itemEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if buySuccess then
        player:addItem(itemId, num, itemMeta)
        local entity = EngineWorld:getEntity(itemEntityId)
        if entity ~= nil then
            entity:setDead()
        end
    end
end

return PlayerListener
--endregion
