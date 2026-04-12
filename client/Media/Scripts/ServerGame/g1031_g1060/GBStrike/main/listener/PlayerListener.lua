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

    PlayerBuyCustomPropEvent.registerCallBack(self.onPlayerBuyCustomProp)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerReviewPlayerEvent.registerCallBack(self.onPlayerReviewPlayer)
    PlayerBackHallEvent.registerCallBack(self.onPlayerBackHall)
    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerBuyBulletEvent.registerCallBack(self.onPlayerBuyBullet)
end

function PlayerListener.onPlayerLogin(clientPeer)
    if GameMatch:ifGameEnd() then
        return GameOverCode.GameOver
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    if player.team ~= nil then
        player.team:onPlayerQuit()
    end
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    DbUtil:getPlayerData(player)
    RankNpcConfig:getPlayerRankData(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    if GameMatch:ifGameRunning() then
        player:prepareInitPorps()
    end
    return 43200
end

function PlayerListener.onPlayerRespawn(player)
    player:playerRespawn()
    player:sendNameToOtherPlayers()
    player:changeTeamActor(player.teamId)
    return 43200
end

function PlayerListener.onPlayerMove(Player, x, y, z)
    local player = PlayerManager:getPlayerByPlayerMP(Player)
    if player == nil then
        return GameMatch.canMove
    end
    player:onMove(x, y, z)
    return GameMatch.canMove
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if GameMatch:ifGameRunning() == false then
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
    if hurter:isInvincible() then
        return false
    end
    if hurter:getTeamId() == attacker:getTeamId() then
        return false
    end
    if damageType == "explosion.player" or damageType == "explosion" then
        hurtValue.value = math.max(hurtValue.value * 1.3, 10)
    end
    if damageType == "player.gun.headshot" then
        attacker.headshot = 1
    else
        attacker.headshot = 0
    end
    hurtValue.value = hurter:subDefense(hurtValue.value)
    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier == nil then
        return true
    end
    local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
    if killer == nil then
        dier:onDie(false)
        return true
    end
    GameMatch.curKills = GameMatch.curKills + 1
    killer:onKill(dier)
    if GameMatch:ifGameEnd() then
        dier:onDie(true)
    else
        dier:onDie(false)
    end
    killer.headshot = 0
    return true
end

function PlayerListener.onPlayerBuyCustomProp(rakssid, uniqueId, msg)
    if GameMatch:ifGameRunning() == false then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local prop = PropsConfig:getProp(uniqueId)
    if prop == nil then
        return
    end
    if player:buyProp(prop) == false then
        msg.value = Messages:msgPurchaseFailedTip()
    end
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    local canAdd, num = ItemSettingConfig:canAddItem(player, itemId, itemNum)
    return canAdd
end

function PlayerListener.onPlayerReviewPlayer(rakssid1, rakssid2)
    local player1 = PlayerManager:getPlayerByRakssid(rakssid1)
    local player2 = PlayerManager:getPlayerByRakssid(rakssid2)
    if player1 == nil or player2 == nil then
        return
    end
    if player1:getTeamId() ~= player2:getTeamId() then
        player1:setShowName(" ", player2.rakssid)
    else
        player1:setShowName(player1:buildShowName(), player2.rakssid)
    end
end

function PlayerListener.onPlayerBackHall(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    BackHallManager:addPlayer(player)
end

function PlayerListener.onPlayerBuyBullet(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if player.money < BulletConfig.Price then
        MsgSender.sendBottomTips(3, Messages:msgLackOfMoney())
        return
    end
    local items = BulletConfig:getItems()
    for _, item in pairs(items) do
        player:addItem(item.id, item.num, 0, true)
    end
    player:subMoney(BulletConfig.Price)
    HostApi.notifyGetItem(player.rakssid, BulletConfig.NotifyId, 0, 1)
end

return PlayerListener
--endregion
