--region *.lua
require "util.GameManager"
require "messages.Messages"
require "data.GamePlayer"
require "data.GamePotionEffect"
require "config.MerchantConfig"
require "Match"

PlayerListener = {}

function PlayerListener:init()

    BaseListener.registerCallBack(PlayerLoginEvent,self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent,self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent,self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent,self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
    PlayerAttackActorNpcEvent.registerCallBack(self.onPlayerAttackActorNpc)
    PlayerUpdateRealTimeRankEvent.registerCallBack(self.onPlayerUpdateRealTimeRank)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerPotionEffectFinishedEvent.registerCallBack(self.onPlayerPotionEffectFinished)
end

function PlayerListener.onPlayerLogin(clientPeer)
    if GameMatch:isGameRunning() then
        return GameOverCode.GameStarted
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    player:request_rank()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    player:getDBDataRequire()
    MerchantConfig:syncPlayer(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if GameMatch:isGameRunning() == false then
        return false
    end

    if damageType == "fall" or damageType == "inWall" then
        return false
    end

    local player = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if player == nil then
        return false
    end

    if hurtFrom == nil then
        return true
    end
    local attacker = PlayerManager:getPlayerByPlayerMP(hurtFrom)
    if attacker == nil then
        return false
    end

    if player == attacker then
        return false
    end

    if attacker.domain ~= nil then
        hurtValue.value = 0
    end

    if damageType == "explosion.player" then
        hurtValue.value = attacker.ownWeaponDamage["46"] or 10
    else
        hurtValue.value = attacker.ownWeaponDamage[tostring(attacker:getHeldItemId())] or 0
    end
 
    hurtValue.value = math.max(1, attacker.attackX * (attacker.damage + attacker.p_addAttack + attacker.addAttack + attacker.s_addAttack + hurtValue.value - (player.addDefense + player.p_addDefense + player.s_addDefense)) * (1 - player.defense))
    attacker:attackEffect(hurtValue.value)
    return true
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
    player.position = pos
    local domains = GameManager:getDomains()
    for i, domain in pairs(domains) do
        if domain:isDomainPlayer(player) then
            if domain:inArea(pos) then
                domain:onPlayerEnter(player)
            else
                domain:onPlayerQuit(player)
            end
        end
    end

    local resources = GameManager:getPublicResources()
    for i, resource in pairs(resources or {}) do
        if resource.receiveNpc ~= nil then
            if resource.receiveNpc:inArea(pos) then
                resource.receiveNpc:onPlayerEnter(player)
            else
                resource.receiveNpc:onPlayerQuit(player)
            end
        end
    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier == nil then
        return true
    end

    local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
    if killer == nil then
        killer = PlayerManager:getPlayerByRakssid(dier.killerBySkill)
    end

    if killer == nil then
        RespawnHelper.sendRespawnCountdown(dier, RespawnConfig:getRespawnCd(GameMatch:getGameRunningTime()))
        GamePotionEffect:onRemovePotionEffect(dier)
        return true
    end

    RespawnHelper.sendRespawnCountdown(dier, RespawnConfig:getRespawnCd(GameMatch:getGameRunningTime()))
    dier:dorpMoneyByPercent(killer, dier)
    dier.isDead = true
    dier.killerBySkill = 0
    GamePotionEffect:onRemovePotionEffect(dier)

    if killer ~= nil then
        killer:onKill()
    end

    return true
end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum)
    if itemId == 65 then
        return true
    end
    return false
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, damage)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return true
    end
    for i, EquipId in pairs(player.ownEquipIds) do
        if EquipId == itemId then
            player.ownEquipIds[i] = nil
            return true
        end
    end
    return true
end

function PlayerListener.onPlayerRespawn(player)
    if GameMatch:isGameRunning() then
        player:respawnInGame()
    else
        player:teleInitPos()
    end
    return 43200
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

function PlayerListener.onPlayerUpdateRealTimeRank(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    GameMatch:sendRealRankData(player)
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    return player:BuyCommodity(type, goodsInfo, attr, msg)
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    if OccupationConfig:isOccupationId(itemId) then
        if GameMatch:isGameRunning() then
            return true
        else
            msg.value = Messages:canBuyInGameRunning()
            return false
        end
    end
    if GameConfig:isCoinbyItemId(itemId) then
        if GameMatch:isGameRunning() then
            msg.value = Messages:noBuyCoin()
            return false
        end
    end
    if not GameConfig:isCoinbyItemId(itemId) and itemId < 10000 then
        if GameMatch:isGameRunning() then
            return true
        end
        msg.value = Messages:canBuyInGameRunning()
        return false
    end
    return true
end

function PlayerListener.onPlayerPotionEffectFinished(rakssid, id)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if id <= 0 then
        return
    end
    if id == tonumber(PotionID.MOVE_SPEED) or id == tonumber(PotionID.MOVE_SLOWDOWN) then
        for i, potionId in pairs(player.ownPotionIds) do
            local equipBoxInfo = EquipBoxConfig:getEquipBoxInfoById(potionId)
            if equipBoxInfo ~= nil and tonumber(equipBoxInfo.effect.id) == tonumber(GamePotionEffect.ADD_MOVE_SPEED) then
                player:setSpeedAddition(tonumber(equipBoxInfo.effect.value))
            end
        end
    end

end

return PlayerListener
--endregion
