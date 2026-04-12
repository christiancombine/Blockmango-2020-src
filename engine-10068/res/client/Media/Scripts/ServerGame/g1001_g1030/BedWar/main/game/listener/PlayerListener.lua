PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurtForSettlement)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)
    BaseListener.registerCallBack(PlayerDieTipEvent, self.onPlayerDiedTip)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerUseCommonItemEvent.registerCallBack(self.onPlayerUseCommonItem)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerExchangeItemEvent.registerCallBack(self.onPlayerExchangeItem)
    PlayerPickupItemEndEvent.registerCallBack(self.onPlayerPickupItemEnd)
    PotionCustomEvent.registerCallBack(self.onPotionCustom)
    ItemSkillNeedCustomSpeedEvent.registerCallBack(self.onItemSkillNeedCustomSpeed)
    KnockBackExtraStrengthEvent.registerCallBack(self.onKnockBackExtraStrength)
    PlayerBackHallEvent.registerCallBack(self.onPlayerBackHall)
    PlayerAttackCreatureEvent.registerCallBack(self.onPlayerAttackCreature)

    PlayerRequestPlayAgainEvent:registerCallBack(self.onPlayerRequestPlayAgain)
    PlayerBuyGoodEvent:registerCallBack(self.onPlayerBuyGood)
    PlayerBackBedWarHallEvent:registerCallBack(self.onPlayerBackHall)

    PlayerLicenseReceiveRewardEvent:registerCallBack(self.onPlayerReceiveReward)
    PlayerLicenseBuyLevelEvent:registerCallBack(self.onPlayerBuyLevel)
    PlayerBuyLicenseEvent:registerCallBack(self.onPlayerBuyLicense)
    PlayerExchangeRewardEvent:registerCallBack(self.onPlayerExchangeReward)
    PlayerReceiveTaskReward:registerCallBack(self.onPlayerReceiveTaskReward)
    PlayerClickMainControlEvent:registerCallBack(self.onPlayerClickMainControl)

    PlayerUseActionEvent:registerCallBack(self.onPlayerUseAction)
    PlayerIntoFollowModeEvent:registerCallBack(self.onPlayerIntoFollowMode)
end

function PlayerListener:onPlayerRequestPlayAgain(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    EngineUtil.sendEnterOtherGame(player.rakssid, GameType, player.userId, player.cur_map_id)
end

function PlayerListener.onPlayerExchangeItem(rakssid, type, itemId, itemMeta, itemNum, bullets, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if type == 1 then
        return ItemHelper:canDropItem(player, itemId)
    elseif type == 2 then
        local item = Item.getItemById(itemId)
        if item and item:getClassName() == "ItemSword" then
            player:removeWoodSword()
        end
        return true
    end
end

function PlayerListener.onPlayerLogin(clientPeer)
    if GameMatch.hasStartGame then
        return GameOverCode.GameStarted
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    GamePacketSender:sendMapId(player.rakssid, GameConfig.mapId)
    HostApi.sendGameStatus(player.rakssid, 1)
    return GameOverCode.Success, player
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
    if not GameMatch.allowPvp then
        GameInfoManager:clearInfo("PlayerInfo", player.userId)
    end
    GameResourceManager:onPlayerLogout(player)
end

function PlayerListener.onPlayerReady(player)
    HostApi.sendPlaySound(player.rakssid, 10000)
    DbUtil:getPlayerData(player)
    player:checkParty()

    if GameConfig.openLotterySwitch == 1 then
        GamePacketSender:sendBedWarOpenLottery(player.rakssid)
    end

    if GameMatch.allowPvp then
        HostApi.setAllowUseItemStatus(player.rakssid, 1)
    else
        HostApi.setAllowUseItemStatus(player.rakssid, 0)
    end
    GameInfoManager.Instance():onPlayerReady(player)
    player.lifeStatus = true
    GamePacketSender:sendMoveLimitCheck(player.rakssid)
    GamePacketSender:sendQuicklyBuildBlock(player.rakssid)
    GameLottery:addPlayerLotteryInfo(player.userId)
    CommonPacketSender:sendBedWarPlayerReady(player.rakssid)
    return 43200
end

function PlayerListener.onPlayerHurtForSettlement(hurtPlayer, hurtFrom, damageType, hurtValue)
    local needHurt = PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if needHurt then
        local hurter = PlayerManager:getPlayerByPlayerMP(hurtFrom)
        if hurter then
            SettlementManager:onPlayerHurt(hurtPlayer:getPlatformUserId(), hurtFrom:getPlatformUserId(), tonumber(hurtValue.value))
        else
            SettlementManager:onPlayerHurt(hurtPlayer:getPlatformUserId(), nil, tonumber(hurtValue.value))
        end

    end
    return needHurt
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if hurter == nil then
        return false
    end
    if hurter.isInvincible then
        ---无敌时间
        return false
    end
    if hurter.hurtLock then
        ---受伤保护时间
        return false
    end
    hurter.hurtLock = LuaTimer:schedule(function()
        hurter.hurtLock = nil
    end, 250)
    local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)
    if fromer ~= nil then
        hurter.damageType = "player"
    end
    if (fromer and AIManager:isAI(fromer.rakssid)) and not AIManager:isAI(hurter.rakssid or 0) then
        hurtValue.value = hurtValue.value * GameConfig.aiDamageCoefficient
    end

    if AIManager:isAI(hurter.rakssid) then
        local ai = AIManager:getAIByRakssid(hurter.rakssid)
        if ai then
            ai:onHurt()
        end
    end
    -- HostApi.log("onPlayerHurt " .. damageType)

    if (GameMatch.allowPvp == false) then
        if damageType == "outOfWorld" then
            hurter:teleWaitPos()
        end
        return false
    end

    if damageType == "fall" then
        local blockId = EngineWorld:getBlockId(VectorUtil.getNegY(hurtPlayer:getPosition()))
        if blockId == 0 or blockId == 165 then
            return false
        end
        hurter.damageType = "fall"
    end

    if damageType == "explosion.player" or damageType == "explosion" then
        hurtValue.value = hurter:subApple(hurtValue.value)
        hurter.damageType = "explosion"
        return true
    end

    if damageType == "outOfWorld" then
        if hurter.isLife == false and hurter.realLife == false then
            hurter:teleWatchPos()
            return false
        end
        hurter.damageType = damageType
        hurtValue.value = hurter:subApple(hurtValue.value)
        hurter.damageType = "outOfWorld"
        return true
    end

    if damageType == "arrow" then
        hurter.damageType = "arrow"
    end

    if damageType == "onFire" then
        hurter.damageType = "onFire"
    end

    if hurtFrom == nil then
        hurtValue.value = hurter:subApple(hurtValue.value)
        return true
    end

    if fromer ~= nil then
        if hurter.team and fromer.team and hurter.team.id == fromer.team.id then
            return false
        end
        RuneManager:onAttack(fromer, hurtValue, damageType, hurter)
    end

    RuneManager:onHurted(hurter, hurtValue)
    if hurtValue.value < 1 then
        hurtValue.value = 1
    end
    hurtValue.value = hurter:subApple(hurtValue.value)
    BuffManager:onPlayerByInterrupt(hurter.userId)
    return true
end

function PlayerListener.onPlayerDiedTip(diePlayer, killPlayer)
    return false
end

function PlayerListener.onPlayerDied(diePlayer, _, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if not dier then
        return true
    end

    if not GameMatch:isGameStart() then
        dier:setInventory()
        dier:clearInv()
        RespawnHelper.addRespawnCountdown(dier, 0)
        return true
    end

    local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
    if killer then
        killer:onKill(dier)
    end

    dier:clearEffects()
    dier:clearArmorInfo()
    dier:setInventory()
    dier:setEnderInventory()
    LuaTimer:schedule(function()
        dier:teleWatchPos()
    end, 0)
    dier:resetApple()
    dier.curApple = 0
    dier.maxApple = 0
    dier:updateApple()
    dier.deathNum = dier.deathNum + 1

    if dier.deathNum == 1 and AIManager:isAI(dier.rakssid) == false then
        if killer == nil then
            ReportUtil.reportDeathPeriod(dier, dier.damageType, 0)
        else
            ReportUtil.reportDeathPeriod(dier, dier.damageType, killer:getHeldItemId())
        end
    end

    if killer == nil then
        ReportUtil.reportDeathReason(dier, dier.initLevel, dier.damageType, 0)
    else
        ReportUtil.reportDeathReason(dier, dier.initLevel, dier.damageType, killer:getHeldItemId())
    end
    GameMatch:onPlayerDie(dier)
    dier.multiKill = 0
    dier.lifeStatus = false
    local ai = AIManager:getAIByRakssid(dier.rakssid)
    if ai ~= nil then
        ai:refreshInv()
        ai.isLife = false
    end

    return true
end

function PlayerListener.onPlayerRespawn(player)
    if GameMatch:isGameStart() == false then
        AddMaxHPPrivilege:onPlayerRespawn(player)
        player:fillInvetory(player.inventory)
        player:teleWaitPos()
        return 43200
    end

    RuneManager:onRespawn(player)
    player:onSpawnInGame()
    return 43200
end

function PlayerListener.onPlayerUseCommonItem(rakssId, itemId)

    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return
    end

    if itemId >= 467 and itemId <= 471 then
        local pos = player:getPosition()
        if BridgeConfig:buildBridge(itemId, player:getYaw(), VectorUtil.toBlockVector3(pos.x, pos.y - 1, pos.z)) then
            SettlementManager:addBuildScore(player.userId, Define.BuildType.Egg)
        end
    end
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return true
    end
    BuffManager:onPlayerByInterrupt(player.userId)
    return ItemHelper:canDropItem(player, itemId)
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end
    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)
    if player == nil then
        return true
    end

    if y <= GameConfig.deathLineY then
        if (GameMatch.allowPvp == false) then
            player:teleWaitPos()
        else
            player.damageType = "outOfWorld"
            player:subHealth(99999)
        end

    end
    TeamConfig:onPlayerMove(player, x, y, z)
    GameResourceManager:onPlayerMove(player, x, y, z)
    if player.realLife and movePlayer.onGround == true then
        local curPos = VectorUtil.newVector3(math.floor(x), math.floor(y), math.floor(z))
        if not player.lastGroundPos or VectorUtil.hashVector3(curPos) ~= VectorUtil.hashVector3(player.lastGroundPos) then
            player.lastGroundPos = curPos
        end
    end
    return true
end

function PlayerListener.onPlayerPickupItemEnd(rakssid, itemId, itemNum, _, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    GameResourceManager:onPickUpItem(player, entityId, itemId, itemNum)
    ItemHelper:tryChangePickUpItem(player, itemId)
end

function PlayerListener.onPotionCustom(entityId, itemId)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if not player then
        return true
    end
    local effect = ItemPotionEffect:getPotionEffect(itemId)
    if not effect then
        return true
    end
    player:addEffect(effect.Type, effect.Duration, effect.Level)
    return false
end

function PlayerListener.onItemSkillNeedCustomSpeed(rakssid, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return true
    end

    if ItemID.FLARE_BOMB == itemId then
        return false
    end

    return true
end

function PlayerListener.onKnockBackExtraStrength(hurterEntityId, attackerEntityId, knockBackValue)
    local hurter = PlayerManager:getPlayerByEntityId(hurterEntityId)
    local attacker = PlayerManager:getPlayerByEntityId(attackerEntityId)

    if hurter and attacker then
        knockBackValue.value = knockBackValue.value + tonumber(attacker.info:getValue("runeAddKnockBackValue"))
    end
    return true
end

function PlayerListener.onPlayerBackHall(id)
    local player = PlayerManager:getPlayerByRakssid(id)
    if player == nil then
        player = PlayerManager:getPlayerByUserId(id)
        if player == nil then
            return
        end
    end
    BackHallManager:addPlayer(player)
end

function PlayerListener.onPlayerReceiveReward(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local Lv = DataBuilder.new():fromData(data):getNumberParam("Lv")
    local rewardId = DataBuilder.new():fromData(data):getNumberParam("rewardId")
    local rewardType = DataBuilder.new():fromData(data):getParam("rewardType")
    local getAll = DataBuilder.new():fromData(data):getBoolParam("getAll")
    local season = DataBuilder.new():fromData(data):getNumberParam("season")
    GameLicense:onPlayerGetReward(userId, Lv, getAll, season, rewardId, rewardType)
end

function PlayerListener.onPlayerBuyLevel(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local season = DataBuilder.new():fromData(data):getNumberParam("season")
    local level = DataBuilder.new():fromData(data):getNumberParam("level")
    local free = DataBuilder.new():fromData(data):getNumberParam("free")
    GameLicense:onPlayerBuyLevel(player, season, level, free)
end

function PlayerListener.onPlayerBuyLicense(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local season = DataBuilder.new():fromData(data):getNumberParam("season")
    local buyType = DataBuilder.new():fromData(data):getNumberParam("buyType")
    GameLicense:onPlayerBuy(player, season, buyType)
end

function PlayerListener.onPlayerExchangeReward(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local season = DataBuilder.new():fromData(data):getNumberParam("season")
    local itemId = DataBuilder.new():fromData(data):getNumberParam("itemId")
    local rewardNum = DataBuilder.new():fromData(data):getNumberParam("rewardNum")
    GameLicense:onPlayerExchange(userId, itemId, season, rewardNum)
end

function PlayerListener.onPlayerUseAction(userId, actionId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    CommonPacketSender:sendUseActionToAll(player:getEntityId(), actionId)
end

function PlayerListener.onPlayerReceiveTaskReward(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local taskId = DataBuilder.new():fromData(data):getNumberParam("taskId")
    GameLicense:onPlayerReceiveSeasonTaskReward(player, taskId)
end

function PlayerListener.onPlayerClickMainControl(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    BuffManager:onPlayerByInterrupt(userId)
end

function PlayerListener.onPlayerIntoFollowMode(player, follower)
    GamePacketSender:sendMapId(player.rakssid, GameConfig.mapId)
    if GameMatch:isGameStart() then
        GameResourceManager:synTeamResource(player)
    end
end

function PlayerListener.onPlayerAttackCreature(rakssid, entityId, attackType, position, damage, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    local monster = MonsterManager:getMonster(entityId)
    if monster == nil then
        return false
    end
    return monster:onAttacked(player, damage)
end

return PlayerListener
--endregion
