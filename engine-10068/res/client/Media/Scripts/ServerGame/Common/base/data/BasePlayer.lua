--region *.lua
local CBasePlayer = class("BasePlayer")

---检查未成年人
local function checkMinors(player)
    if player.gameTimeToday < 0 then
        ---成年人
        return
    end
    LogUtil.log("[CheckMinors] [userId]=" .. tostring(player.userId) ..
            " [gameTimeToday]=" .. tostring(player.gameTimeToday), LogUtil.LogLevel.Info)
    local hour = tonumber(os.date("%H"))
    if hour >= 22 or hour < 8 then
        ---每日22点到次日8点，不能给未成年人提供游戏
        HostApi.sendGameover(player.rakssid, 167, GameOverCode.GameOver)
        return
    end
    local userId = player.userId
    LuaTimer:schedule(function()
        player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            return
        end
        HostApi.sendGameover(player.rakssid, 166, GameOverCode.GameOver)
    end, player.gameTimeToday * 1000, nil, userId)
end

function CBasePlayer:initMemberVariables()
    self.clientPeer = nil
    self.entityPlayerMP = nil
    self.userId = 0
    self.rakssid = 0
    self.name = ""
    self.gold = 0
    self.available = 0
    self.hasGet = 0
    self.scorePoints = {}
    self.defaultRank = 10
    self.respawnTimes = -1
    self.isReady = false
    self.isLoginSuccess = false
    self.inGameTime = os.time()
    self.appIntegral = 0
    self.enableIndie = false
    self.gameTimeToday = 0
    self.isKickOut = false
    self.vip = 0

    self.staticAttr = {}
    self.staticAttr.clanId = 0 -- clanId
    self.staticAttr.clanName = "" -- clanName
    self.staticAttr.role = -1 -- role
    self.staticAttr.props = {} --props

    self.dynamicAttr = {}
    self.dynamicAttr.team = 0
end

function CBasePlayer:getEntityPlayer()
    return self.entityPlayerMP
end

function CBasePlayer:ctor(clientPeer)
    self:initMemberVariables()
    self:respawn(clientPeer)
    self:initDynamicAttr()
end

function CBasePlayer:respawn(clientPeer)
    self.clientPeer = clientPeer
    self.entityPlayerMP = clientPeer:getEntityPlayer()
    self.userId = clientPeer:getPlatformUserId()
    self.rakssid = clientPeer:getRakssid()
    self.name = clientPeer:getName()
    self.respawnTimes = self.respawnTimes + 1
    self:useAppProps()
end

function CBasePlayer:initStaticAttr(teamId)
    WebService:GetUserAttr(self.userId, function(data, code)
        if data == nil then
            self.isLoginSuccess = false
            self:sendLoginResult(false, teamId or self.dynamicAttr.team, "",
                    PlayerManager:getPlayerCount(), PlayerManager:getMaxPlayer())
        else
            self.isLoginSuccess = true
            UserExpManager:getUserExpCache(self.userId)
            self:sendLoginResult(true, teamId or self.dynamicAttr.team, "",
                    PlayerManager:getPlayerCount(), PlayerManager:getMaxPlayer())
            self:setShowName(self:buildShowName())
            self:useAppProps()
        end
        checkMinors(self)
    end)
end

function CBasePlayer:sendLoginResult(isSuccess, teamId, teamName, playerNum, maxPlayer)
    if self.clientPeer ~= nil then
        self.clientPeer:sendLoginResult(isSuccess, teamId, teamName, playerNum, maxPlayer)
    end
end

function CBasePlayer:onPlayerLogout()
    self.clientPeer = nil
    self.entityPlayerMP = nil
    self.isReady = false
    ReportManager:onPlayerLogout(self)
end

function CBasePlayer:getDisplayName()
    return self.name
end

function CBasePlayer:checkReady()
    return self.isReady
end

function CBasePlayer:addNightVisionPotionEffect(seconds)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addNightVisionPotionEffect(seconds)
    end
end

function CBasePlayer:useAppProps()
    AppProps:useAppProps(self, self.respawnTimes == 0)
end

function CBasePlayer:onUseCustomProp(propId, itemId)

end

function CBasePlayer:addItem(id, num, damage, isReverse, maxDamage, stackIndex)
    if not self.entityPlayerMP then
        return
    end
    if stackIndex then
        return self.entityPlayerMP:addItemByIndex(id, num, damage, stackIndex)
    else
        return self.entityPlayerMP:addItem(id, num, damage, isReverse or false, maxDamage or 0)
    end
end

function CBasePlayer:addGunItem(id, num, damage, bullets, isReverse)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addGunItem(id, num, damage, bullets, isReverse or false)
    end
end

function CBasePlayer:addItemToEnderChest(slot, id, num, damage, enchantList)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addItemToEnderChest(slot, id, num, damage, enchantList)
    end
end

function CBasePlayer:addGunItemToEnderChest(slot, id, num, damage, bullets)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addGunItemToEnderChest(slot, id, num, damage, bullets)
    end
end

function CBasePlayer:addGunDamage(gunId, damage)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addGunDamage(gunId, damage)
    end
end

function CBasePlayer:addGunBulletNum(gunId, bulletNum)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addGunBulletNum(gunId, bulletNum)
    end
end

function CBasePlayer:subGunRecoil(gunId, recoil)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:subGunRecoil(gunId, recoil)
    end
end

function CBasePlayer:removeItem(id, num, meta)
    if self.entityPlayerMP ~= nil then
        local inv = self.entityPlayerMP:getInventory()
        inv:removeItem(id, num, meta or -1)
    end
end

function CBasePlayer:decrStackSize(slot, count)
    if self.entityPlayerMP ~= nil then
        local inv = self.entityPlayerMP:getInventory()
        inv:decrStackSize(slot, count)
    end
end

function CBasePlayer:equipArmor(id, damage)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:equipArmor(id, damage)
    end
end

function CBasePlayer:replaceItem(id, num, damage, stackIndex, clipBullet)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:replaceItem(id, num, damage, stackIndex, clipBullet)
    end
end

function CBasePlayer:addEnchantmentItem(id, num, damage, enchantmentId, enchantmentLevel)
    local enchantments = { { enchantmentId, enchantmentLevel } }
    self:addEnchantmentsItem(id, num, damage, enchantments)
end

function CBasePlayer:addEnchantmentsItem(id, num, damage, enchantmentList, stackIndex)
    if not self.entityPlayerMP then
        return
    end
    if stackIndex then
        self.entityPlayerMP:addEchantmentItemByIndex(id, num, damage, enchantmentList, stackIndex)
    else
        self.entityPlayerMP:addEchantmentItem(id, num, damage, enchantmentList)
    end
end

function CBasePlayer:getHealth()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getHealth()
    end
    return 20
end

function CBasePlayer:setHealth(health)
    if health == nil then
        return
    end
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:setEntityHealth(health)
    end
end

function CBasePlayer:addHealth(hp)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:heal(hp)
    end
end

function CBasePlayer:subHealth(hp)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:heal(-hp)
    end
end

function CBasePlayer:setEntityHealth(hp)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:setEntityHealth(hp)
    end
end

function CBasePlayer:updateExp(level, exp, maxExp)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:updateExp(level, exp, maxExp)
    end
end

function CBasePlayer:getPosition()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getPosition()
    end
    return VectorUtil.newVector3(0, 0, 0)
end

function CBasePlayer:getYaw()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getYaw()
    end
    return 0
end

function CBasePlayer:getPitch()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getPitch()
    end
    return 0
end

function CBasePlayer:setPositionAndRotation(vec, yaw, pitch)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:setPositionAndRotation(vec, yaw, pitch)
    end
end

function CBasePlayer:getSex()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getSex()
    end
    return 1
end

function CBasePlayer:getRotationYaw()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP.rotationYaw % 360
    end
    return 0
end

function CBasePlayer:getWidth()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getWidth()
    end
    return 1
end

function CBasePlayer:isWatchMode()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:isWatchMode()
    end
    return true
end

function CBasePlayer:setAllowFlying(flying)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setAllowFlying(flying)
    end
end

function CBasePlayer:getIsFlying()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getIsFlying()
    end

    return false
end

function CBasePlayer:setFoodLevel(value)
    if value == nil then
        return
    end
    if self.entityPlayerMP ~= nil and self.entityPlayerMP:getFoodStats() then
        self.entityPlayerMP:getFoodStats():setFoodLevel(value)
    end
end

function CBasePlayer:getFoodLevel()
    if self.entityPlayerMP ~= nil and self.entityPlayerMP:getFoodStats() then
        return self.entityPlayerMP:getFoodStats():getFoodLevel()
    end
    return 20
end

function CBasePlayer:setSpeedAddition(level)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:setSpeedAdditionLevel(level)
    end
end

function CBasePlayer:getItemNumById(id, meta)
    if self.entityPlayerMP ~= nil then
        local inv = self.entityPlayerMP:getInventory()
        return inv:getItemNum(id, meta or -1)
    end
    return 0
end

function CBasePlayer:clearInv()
    if self.entityPlayerMP ~= nil then
        local inv = self.entityPlayerMP:getInventory()
        inv:clear()
    end
end

function CBasePlayer:getInventory()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getInventory()
    end
end

function CBasePlayer:getWallet()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getWallet()
    end
end

function CBasePlayer:setExtraInventoryCount(count)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setExtraInventoryCount(count)
    end
end

function CBasePlayer:changeMaxHealth(health, unChangeCurrent)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeMaxHealth(health, unChangeCurrent or false)
    end
end

function CBasePlayer:changeMaxHealthSingle(health)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeMaxHealthSingle(health)
    end
end

function CBasePlayer:getMaxHealth()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getMaxHealth()
    end
    return 0
end

function CBasePlayer:getHeldItemId()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getHeldItemId()
    end
    return 0
end

function CBasePlayer:setCurrency(currency)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setCurrency(currency)
    end
end

function CBasePlayer:addCurrency(currency)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addCurrency(currency)
    end
end

function CBasePlayer:subCurrency(currency)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:subCurrency(currency)
    end
end

function CBasePlayer:setUnlockedCommodities(unlockedCommodities)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setUnlockedCommodities(unlockedCommodities)
    end
end

function CBasePlayer:getCurrency()
    if self.entityPlayerMP ~= nil then
        return tonumber(tostring(self.entityPlayerMP:getCurrency()))
    end
    return 0
end

function CBasePlayer:addBackpackCapacity(capacity)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addBackpackCapacity(capacity)
    end
end

function CBasePlayer:subBackpackCapacity(capacity)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:subBackpackCapacity(capacity)
    end
end

function CBasePlayer:resetBackpack(capacity, maxCapacity)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:resetBackpack(capacity, maxCapacity)
    end
end

function CBasePlayer:setArmItem(itemId)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setArmItem(itemId)
    end
end

function CBasePlayer:fillInvetory(inventory)
    self:clearInv()
    if inventory == nil then
        return
    end
    local armors = inventory.armor
    if armors ~= nil then
        for i, armor in pairs(armors) do
            if armor.id > 0 and armor.id < 10000 then
                self:equipArmor(armor.id, armor.meta)
            end
        end
    end
    local guns = inventory.gun
    if guns ~= nil then
        for i, gun in pairs(guns) do
            self:addGunItem(gun.id, gun.num, gun.meta, gun.bullets)
        end
    end
    local items = inventory.item
    if items ~= nil then
        for i, item in pairs(items) do
            if item.id > 0 and item.id < 10000 then
                self:addItem(item.id, item.num, item.meta)
            end
        end
    end
end

function CBasePlayer:fillEnderInvetory(enderInv)
    if enderInv == nil then
        return
    end
    local slot = 0
    local guns = enderInv.gun
    if guns ~= nil then
        for i, gun in pairs(guns) do
            self:addGunItemToEnderChest(slot, gun.id, gun.num, gun.meta, gun.bullets)
            slot = slot + 1
        end
    end
    local items = enderInv.item
    if items ~= nil then
        for i, item in pairs(items) do
            if item.id > 0 and item.id < 10000 then
                if item.enchantList == nil then
                    item.enchantList = {}
                end
                self:addItemToEnderChest(slot, item.id, item.num, item.meta, item.enchantList)
                slot = slot + 1
            end
        end
    end
end

function CBasePlayer:getEnderInventory()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getInventoryEnderChest()
    end
end

function CBasePlayer:addEffect(id, time, level)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:addEffect(id, time, level)
    end
end

function CBasePlayer:removeEffect(id)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:removeEffect(id)
    end
end

function CBasePlayer:clearEffects()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:clearEffects()
    end
end

function CBasePlayer:setRespawnPos(pos)
    if self.clientPeer ~= nil then
        self.clientPeer:setRespawnPos(VectorUtil.newVector3i(pos.x, pos.y + 0.5, pos.z))
    end
end

function CBasePlayer:teleportPos(pos)
    if not pos then
        return
    end
    HostApi.resetPos(self.rakssid, pos.x, pos.y + 0.5, pos.z)
end

function CBasePlayer:teleportPosWithYaw(pos, yaw)
    HostApi.resetPosWithYaw(self.rakssid, pos.x, pos.y + 0.5, pos.z, yaw)
end

function CBasePlayer:teleportPosWithYaw1(pos)
    HostApi.resetPosWithYaw(self.rakssid, pos.x, pos.y + 0.5, pos.z, pos.w)
end

function CBasePlayer:changeClothes(partName, actorId)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeClothes(partName, actorId)
    end
end

function CBasePlayer:resetClothes()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:resetClothes(self.userId)
    end
end

function CBasePlayer:playSkillEffect(name, duration, range, color)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:playSkillEffect(name, duration, range, color)
    end
end

function CBasePlayer:sendSkillEffect(effectId)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:sendSkillEffect(effectId)
    end
end

function CBasePlayer:buildShowName()
    local nameList = {}
    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end
    PlayerIdentityCache:buildNameList(self.userId, nameList, self.name)
    return table.concat(nameList, "\n")
end

function CBasePlayer:initDynamicAttr()
    local manager = GameServer:getRoomManager()
    if not manager:isUserAttrExisted(self.userId) then
        return
    end
    local attr = manager:getUserAttrInfo(self.userId)
    self.dynamicAttr = attr
    self.vip = attr.vip
    ---跟随通知
    local name = attr.name
    name = name:gsub(TextFormat.colorStart, "")
    name = name:gsub("\n", " ")
    if attr.followEnterType == 1 then
        ---跟随进入游戏，一起玩
        local follower = PlayerManager:getPlayerByUserId(attr.targetUserId)
        if follower then
            PacketSender:sendFollowerInfo(follower.rakssid, name)
            ---再次 进入游戏上报加入游戏成功
            for pos, userId in pairs(follower.watchList or {}) do
                if userId == tostring(self.userId) then
                    PacketSender:sendDataReport(self.rakssid, "follow_game_enter_suc", "")
                    table.remove(follower.watchList, pos)
                    break
                end
            end
        end
    elseif attr.followEnterType == 2 then
        ---进入观战模式
        local follower = PlayerManager:getPlayerByUserId(attr.targetUserId)
        if follower then
            PacketSender:sendWatcherInfo(follower.rakssid, name)
            ---记录 跟随进入的观战标志.
            follower.watchList = follower.watchList or {}
            for _, userId in pairs(follower.watchList) do
                if userId == tostring(self.userId) then
                    return
                end
            end
            table.insert(follower.watchList, tostring(self.userId))
        end
    end
end

function CBasePlayer:setShowName(name, targetId)
    if self.clientPeer ~= nil then
        self.clientPeer:setShowName(name, targetId or 0)
    end
end

function CBasePlayer:setTeam(team)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setTeam(team)
    end
end

function CBasePlayer:changeHeart(hp, maxHp)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeHeart(hp, maxHp)
    end
end

function CBasePlayer:getEntityId()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:hashCode()
    end
    return -1
end

function CBasePlayer:addVehicles(vehicles)
    if self.entityPlayerMP == nil or vehicles == nil then
        return
    end
    for i, vehicle in pairs(vehicles) do
        self.entityPlayerMP:addOwnVehicle(vehicle)
    end
    self.entityPlayerMP:syncOwnVehicle()
end

function CBasePlayer:leaveVehicle()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:getOffVehicle()
    end
end

function CBasePlayer:checkDiamondBlues(diamonds)
    if self.clientPeer ~= nil then
        return self.clientPeer:checkDiamondBlues(diamonds)
    end
    return true
end

function CBasePlayer:consumeDiamonds(uniqueId, diamonds, remark, isConsume)
    if self.isKickOut or not self.clientPeer then
        return
    end
    if isConsume == nil then
        self.clientPeer:consumeDiamonds(uniqueId, diamonds, remark, true)
    else
        self.clientPeer:consumeDiamonds(uniqueId, diamonds, remark, isConsume)
    end
end

function CBasePlayer:consumeGolds(uniqueId, diamonds, remark, isConsume)
    if self.isKickOut or not self.clientPeer then
        return
    end
    if isConsume == nil then
        self.clientPeer:consumeGolds(uniqueId, diamonds, remark, true)
    else
        self.clientPeer:consumeGolds(uniqueId, diamonds, remark, isConsume)
    end
end

function CBasePlayer:isVisitor()
    if self.clientPeer ~= nil then
        return self.clientPeer:isVisitor()
    end
    return true
end

function CBasePlayer:setPersonalShopArea(startPos, endPos)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setPersonalShopArea(startPos, endPos)
    end
end

function CBasePlayer:getDiamond()
    if self.entityPlayerMP ~= nil then
        return tonumber(tostring(self.entityPlayerMP:getDiamond()))
    end
    return 0
end

function CBasePlayer:getGold()
    if self.entityPlayerMP ~= nil then
        return tonumber(tostring(self.entityPlayerMP:getGold()))
    end
    return 0
end

function CBasePlayer:setOnFrozen(frozenTime)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setOnFrozen(frozenTime)
    end
end

function CBasePlayer:setOnFire(fireTime)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setOnFire(fireTime)
    end
end

function CBasePlayer:addCustomEffect(name, effectName, duration, scale)
    if self.entityPlayerMP ~= nil then
        scale = scale or 1.0
        self.entityPlayerMP:addCustomEffect(name, effectName, duration, scale)
    end
end

--清除所有特效
function CBasePlayer:clearCustomEffects()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:clearCustomEffects()
    end
end

function CBasePlayer:getRanch()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getRanch()
    end
end

function CBasePlayer:getBirdSimulator()
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getBirdSimulator()
    end
end

function CBasePlayer:setBagInfo(curCapacity, maxCapacity)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:setBagInfo(curCapacity, maxCapacity)
    end
end

function CBasePlayer:setBirdConvert(isConvert)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:setBirdConvert(isConvert)
    end
end

function CBasePlayer:setGlide(isOpenGlide)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:setGlide(isOpenGlide)
    end
end

function CBasePlayer:setFlyWing(isEquipWing, glideMoveAddition, glideDropResistance)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:setFlyWing(isEquipWing, glideMoveAddition, glideDropResistance)
    end
end

function CBasePlayer:getClothesId(partName)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:getClothesId(partName)
    end
end

function CBasePlayer:placeBlock(vec3, hitPos, face)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:placeBlock(vec3, hitPos, face)
    end
end

function CBasePlayer:getLanguage()
    if self.clientPeer ~= nil then
        return self.clientPeer:getLanguage()
    end
    return "en_US"
end

function CBasePlayer:getClanName()
    local name = TextFormat.colorGreen .. self.staticAttr.clanName
    if BaseMain:isChina() then
        if self.staticAttr.role == 0 then
            name = name .. TextFormat.colorWrite .. "[成员]"
        end
        if self.staticAttr.role == 10 then
            name = name .. TextFormat.colorRed .. "[长老]"
        end
        if self.staticAttr.role == 20 then
            name = name .. TextFormat.colorOrange .. "[酋长]"
        end
    else
        if self.staticAttr.role == 0 then
            name = name .. TextFormat.colorWrite .. "[Member]"
        end
        if self.staticAttr.role == 10 then
            name = name .. TextFormat.colorRed .. "[Elder]"
        end
        if self.staticAttr.role == 20 then
            name = name .. TextFormat.colorOrange .. "[Chief]"
        end
    end
    return name
end

function CBasePlayer:setFlying(isFlying)
    if self.entityPlayerMP ~= nil then
        return self.entityPlayerMP:setFlying(isFlying)
    end
end

function CBasePlayer:getTotalArmorValue()
    if self.entityPlayerMP then
        return self.entityPlayerMP:getTotalArmorValue()
    end
    return 0
end

function CBasePlayer:setScale(coe)
    if self.entityPlayerMP then
        self.entityPlayerMP:setScale(coe)
    end
end

function CBasePlayer:isGrab()
    if self.entityPlayerMP then
        return self.entityPlayerMP:isGrab()
    end
end

function CBasePlayer:isBeGrab()
    if self.entityPlayerMP then
        return self.entityPlayerMP:isBeGrab()
    end
end

function CBasePlayer:unbindParentEntity()
    if self.entityPlayerMP then
        self.entityPlayerMP:unbindParentEntity()
    end
end

function CBasePlayer:unbindAllEntity()
    if self.entityPlayerMP then
        self.entityPlayerMP:unbindAllEntity()
    end
end

function CBasePlayer:getFirstChildrenEntityId()
    if self.entityPlayerMP then
        return self.entityPlayerMP:getFirstChildrenEntityId()
    end
end

function CBasePlayer:isEntityAlive()
    if self.entityPlayerMP then
        return self.entityPlayerMP:isEntityAlive()
    end
end

function CBasePlayer:isFlyingByPulled()
    if self.entityPlayerMP then
        return self.entityPlayerMP.m_flyingByPulled
    end
end

function CBasePlayer:setConcealPower(value)
    if self.entityPlayerMP then
        self.entityPlayerMP:setConcealPower(value)
    end
end

function CBasePlayer:changeActor(actor)
    if self.entityPlayerMP then
        EngineWorld:changePlayerActor(self, actor)
    end
end

function CBasePlayer:recoverActor()
    local sex = self:getSex()
    if sex == 2 then
        self:changeActor("girl.actor")
    else
        self:changeActor("boy.actor")
    end
end

function CBasePlayer:isDead()
    if not self.entityPlayerMP then
        return false
    end
    return self.entityPlayerMP.isDead
end

function CBasePlayer:onTick(ticks)
    --TODO
end

---检查玩家能否接受通用活动的奖励
function CBasePlayer:checkCanReceiveCommonReward(rewardNum)
    return true
end

BasePlayer = CBasePlayer

return BasePlayer
--endregion
