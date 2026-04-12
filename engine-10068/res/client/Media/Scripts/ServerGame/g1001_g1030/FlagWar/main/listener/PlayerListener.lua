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
    PlayerAttackActorNpcEvent.registerCallBack(self.onPlayerAttackActorNpc)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerPickupItemEndEvent.registerCallBack(self.onPlayerPickupItemEnd)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerBuySwitchablePropEvent.registerCallBack(self.onPlayerBuySwitchableProp)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    local callback = function()
        if PlayerManager:getPlayerCount() == 1 and GameMatch.isFirstReady == false then
            GameMatch.isFirstReady = true
        end
    end
    return GameOverCode.Success, player, 1, callback
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    GameMatch:getDBDataRequire(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerRespawn(player)
    if GameMatch:isGameRunning() then
        player:respawnInGame()
    else
        player:teleInitPos()
    end
    return 43200
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    if GameMatch:isGameRunning() == false then
        return true
    end

    if x == 0 and y == 0 and z == 0 then
        return true
    end


    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)
    if player == nil then
        return true
    end

    local pos = VectorUtil.newVector3(x, y, z)

    local teams = GameMatch.Teams
    for index, team in pairs(teams) do
        if team:inArea(pos) then
            if team:isSameTeam(player) then
                team:onPlayerEnter(player)
            else
                team:onEnemyEnter(player)
            end
        end
    end

    local transferGates = GameMatch:getCurGameScene():getTransferGates()
    for index, transferGate in pairs(transferGates) do
        if transferGate:inArea(pos) then
            if transferGate:isSameTeam(player) then
                transferGate:onPlayerEnter(player)
            else
                transferGate:onEnemyEnter(player)
            end
        else
            transferGate:onPlayerQuit(player)
        end
    end

    local bloodPools = GameMatch:getCurGameScene():getBloodPools()
    for index, bloodPool in pairs(bloodPools) do
        if bloodPool:inArea(pos) then
            if bloodPool:isSameTeam(player) then
                bloodPool:onPlayerEnter(player)
            else
                bloodPool:onEnemyEnter(player)
            end
        else
            bloodPool:onPlayerQuit(player)
        end
    end

    local flagStations = GameMatch:getCurGameScene():getFlagStations()
    for index, flagStation in pairs(flagStations) do
        if flagStation:inArea(pos) then
            if flagStation:isSameTeam(player) then
                flagStation:onPlayerEnter(player)
            else
                flagStation:onEnemyEnter(player)
            end
        end
    end

    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if GameMatch:isGameRunning() == false then
        return false
    end
    if hurtPlayer == nil or hurtFrom == nil then
        return true
    end
    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    local attacker = PlayerManager:getPlayerByPlayerMP(hurtFrom)
    if hurter == nil or attacker == nil then
        return true
    end
    if hurter:getTeamId() == attacker:getTeamId() then
        return false
    end
    local itemId = attacker:getHeldItemId()
    local arms = ArmsConfig:getArms(itemId)
    local value = hurtValue.value
    if arms ~= nil then
        value = arms.damage
        if arms.effect and arms.effect.id ~= 0 then
            hurter:addEffect(arms.effect.id, arms.effect.time, arms.effect.lv)
        end
    end
    value = (attacker.attackX * value + attacker.damage - hurter.addDefense) * (1 - hurter.defense)
    if itemId == ItemID.ARROW then
        value = value / 2
    end
    value = math.max(0.5, value)
    hurtValue.value = value
    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier ~= nil then
        RespawnHelper.sendRespawnCountdown(dier, RespawnConfig:getRespawnCd(GameMatch:getGameRunningTime()))
        GameMatch:getCurGameScene():onPlayerDie(dier)
    end
    if killPlayer ~= nil then
        local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
        if killer ~= nil then
            killer:onKill()
        end
    end
    return true
end

function PlayerListener.onPlayerAttackActorNpc(rakssid, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local npc = OccupationConfig:getOccupationNpc(entityId)
    if npc ~= nil then
        npc:onPlayerHit(player)
        return
    end

    npc = GameMatch:getCurGameScene():getFlagByEntityId(entityId)
    if npc ~= nil then
        npc:onPlayerHit(player)
        return
    end

end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    return false
end

function PlayerListener.onPlayerPickupItemEnd(rakssid, itemId, itemNum)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local effect = PotionEffectConfig:getPotionEffect(itemId, GameMatch:getGameRunningTime())
    if effect then
        if effect.effectId ~= 0 then
            player:addEffect(effect.effectId, effect.effectTime * itemNum, effect.effectLv)
        end
        player:removeItem(itemId, itemNum)
    end
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerBuySwitchableProp(rakssid, uniqueId, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local fashion = FashionStoreConfig:getFashion(uniqueId)
    if fashion == nil then
        return
    end
    if player:hasFashion(fashion) then
        player:useFashion(fashion)
        player:sendFashionData()
        return
    end
    if player:buyFashion(fashion) then
        player:sendFashionData()
    end
end

return PlayerListener
--endregion
