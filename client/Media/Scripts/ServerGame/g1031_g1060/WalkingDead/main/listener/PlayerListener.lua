--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerAttackCreatureEvent.registerCallBack(self.onPlayerAttackCreature)
    PlayerBuyGoodsSuccessEvent.registerCallBack(self.onPlayerBuyGoodsSuccess)

    FormulationSubmitEvent.registerCallBack(self.onFormulationSubmit)
    PlayerUseCommonItemEvent.registerCallBack(self.onPlayerUseCommonItem)
    PlayerEquipArmorEvent.registerCallBack(self.onPlayerEquipArmor)
    PlayerArmorChangeEvent.registerCallBack(self.PlayerArmorChange)
    PlayerChangeItemInHandEvent.registerCallBack(self.ChangeItemInHand)
    PlayerFoodLevelChangeEvent.registerCallBack(self.onFoodLevelChange)
    EntityLivingBaseFallNoNeedCalBlockIdEvent.registerCallBack(self.onPlayerFall)

    OpenMovingEnderChestEvent.registerCallBack(self.onPlayerOpenMovingEnderChest)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    player:onPlayerRespawn()
    if GameMatch.startWait == false then
        GameMatch.startWait = true
        GameMatch:startMatch()
        --HostApi.sendStartGame(PlayerManager:getPlayerCount(), true)
        MsgSender.sendMsg(IMessages:msgGameStart())
        --HostApi.sendGameStatus(0, 1)
    end
    return GameOverCode.Success, player
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    DbUtil:getPlayerData(player)
    HostApi.sendPlaySound(player.rakssid, 10044)
    ChestManager:syncChestInfo(player.rakssid)
    HostApi.setAutoShootEnable(player.rakssid, true)
    return 0
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)
    if player == nil then
        return false
    end

    local hash = VectorUtil.hashVector3(VectorUtil.newVector3(x, y, z))
    if hash ~= player.lastMoveHash then
        player.lastMoveTime = os.time()
    end
    player.lastMoveHash = hash

    return GameMatch.allowMove
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    --被击者
    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if hurter == nil then
        return true
    end

    if damageType == "fall" then
        if hurtValue.value >= GameConfig.PlayerAttribute.FallDamageResistance then
            BuffManager:createBuff("fall", hurter.userId, GameConfig.PlayerStateBuffId.Fracture)
            return true
        end
    end

    if damageType == "drown" or damageType == "starve" or damageType == "lava" then
        return true
    end

    if damageType == "inFire" or damageType == "onFire" then
        if hurter:inFire() == true then
            hurtValue.value = GameConfig.InFireDamage
            return true
        end
        return false
    end

    if damageType == "outOfWorld" then
        hurtValue.value = 10000
        return true
    end

    --攻击者
    local attacker = PlayerManager:getPlayerByPlayerMP(hurtFrom)
    if attacker ~= nil then
        if  hurter:onAttacked(attacker, Define.TargetType.TargetPlayer, damageType) then
            attacker:onAttack(hurter, Define.TargetType.TargetPlayer, damageType)
        end
    end

    return false
end

function PlayerListener.onPlayerAttackCreature(rakssid, entityId, attackType)
    --if attackType == 0 then
    --    HostApi.log("onPlayerAttackCreature attackType == 0")
    --    return false
    --end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        --HostApi.log("onPlayerAttackCreature player == nil")
        return false
    end

    --怪物被攻击
    local monster = MonsterManager:getMonster(entityId)
    if monster ~= nil then
        monster:onAttacked(player, Define.TargetType.TargetPlayer, attackType)
    end

    --机器人被攻击
    local robot = RobotManager:getRobot(entityId)
    if robot ~= nil then
        robot:onAttackedByPlayer(player, attackType)
    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if (dier ~= nil) then
        dier:onDie()
    end

    return true
end

function PlayerListener.onPlayerRespawn(player)
    if GameMatch.curStatus == GameMatch.Status.GameTime then
        player:teleInitPos()
        player:onPlayerRespawn()
    end
    --player:PlayerItemInit()
    return 0
end

function PlayerListener.onPlayerBuyGoodsSuccess(rakssid, type, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
end

function PlayerListener.onPlayerUseCommonItem(rakssid, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    player:onUsedItem(player, itemId)
end

function PlayerListener.PlayerArmorChange(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local ArmorInv = InventoryUtil:getPlayerArmorInventoryInfo(player)
    local sumExtraInvCount = 0
    local newUseArmor = {}
    newUseArmor = {}
    for _, item in pairs(ArmorInv) do
        sumExtraInvCount = sumExtraInvCount + ArmorConfig:getCountByArmorId(item.id)

        local armor = ArmorConfig:getArmorValueById(item.id)
        if armor ~= nil and armor.multiple ~= 0 then
            newUseArmor[item.stackIndex] = {}
            newUseArmor[item.stackIndex] = { item.id, armor.ArmorValue - item.meta / armor.multiple, item.stackIndex }
        end
    end
    player:onChangeArmor(player, newUseArmor)
    player:setExtraInventoryCount(sumExtraInvCount)
end

function PlayerListener.ChangeItemInHand(rakssid, itemId, itemDamage)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local gun = GunConfig:getGunByItemId(tostring(itemId))
    if gun ~= nil then
        local type = gun.type
        player:changeDamage(gun.attack, type, itemId)
        return true
    end
    player:changeDamage(GameConfig.PlayerAttribute.CloseDamage, Define.PlayerDamageType.CloseDamage, 0)
end

function PlayerListener.onFoodLevelChange(rakssid, newFoodLevel, oldfoodlevel)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        local changeValue = player.curFoodLevel - newFoodLevel
        if changeValue >= 0 then
            player:subFoodLevel(changeValue)
        else
            player:addFoodLevel(newFoodLevel - player.curFoodLevel)
        end
    end
end

function PlayerListener.onPlayerFall(rakssid)
    return true
end

function PlayerListener.onPlayerOpenMovingEnderChest(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    player:syncVipInfo()
    GameStore:syncSupplyData(player)
    local level, time = player:getMaxVip()
    local curtime = math.max(0, time - os.time())
    if level > 0 and curtime > 0 then
        return true
    end
    player.vip_data.level = 0
    HostApi.sendCommonTip(player.rakssid, "moyingxiang_buy_tip")
    return false
end

return PlayerListener
--endregion
