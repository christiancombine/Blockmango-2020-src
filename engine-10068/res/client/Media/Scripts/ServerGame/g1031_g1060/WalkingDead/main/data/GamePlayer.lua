--GamePlayer.lua

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.kills = 0
    self.score = 0
    self.isLife = true
    self.realLife = true
    self.chest_FreeTime = 0
    self.chest_time = 0
    self.purchaseBox = {}
    self.noPurchaseBox = {}
    self.onHurtSubFoodLevel = 5
    self.maxDefense = 0
    self.curDefense = 0
    self.maxHealth = 100
    self.curHealth = 100
    self.maxFoodLevel = 100
    self.curFoodLevel = 100
    self.satietyHeal = 1
    self.maxWaterLevel = 100
    self.waterLevelSubValue = 1
    self.waterLevelSubTime = 10
    self.curWaterLevel = 100
    self.lastSubWaterTime = 0
    self.addSpeed = 0
    self.closeDamage = 1
    self.remoteDamage = 1
    self.gunId = 0
    self.infection = 0
    self.curUseArmorId = {}
    self.lastAttackTime = os.time() --上次攻击的时间
    self.robotId = 0
    self.hasRobot = false --是否生成机器人
    self.lastPosition = nil
    self.buffData = {}
    self.inventoryInfo = {}
    self.armorInventoryInfo = {}
    self.enderInventory = {}
    self.storeEntityId = 1
    self.lastMoveTime = os.time()
    self.lastMoveHash = ""
    self.lastHurtInFire = 0
    self.randomBoxRewards = nil
    self.dropItemChest = nil
    --生存日记相关
    self.killMonsterNum = 0 --击杀怪物数
    self.killPlayerNum = 0 --击杀玩家数
    --self.loginDayCount = WorldTimeManager.DayCount --登录时过了多少天
    self.surviveDays = 0 --存活天数

    self.lifeTime = 0 --登录时间
    self.killDeadNum = 0
    self.killDeadScore = 0
    self.killBossNum = 0 --击杀boss数
    self.killBossScore = 0

    self.titleType = {}
    self.titles = {}

    self.vip_data = { level = 0, time = 0 }
    self.vip_record = { 0, 0 }

    self.vip_day_key = 0
    --self.vip_day_key = DateUtil.getYearDay()
    self.tiro_box = false
    self.daily_box = 0
    self.vip_box = 0
    self:teleInitPos()
    HostApi.sendPlaySound(self.rakssid, 10044)
end

function GamePlayer:initGameDataFromDB(data)
    if not data.__first then
        self.kills = data.kills or 0
        self.score = data.score or 0
        self.curHealth = data.curHealth or self.curHealth
        self.curFoodLevel = data.curFoodLevel or self.curFoodLevel
        self.curWaterLevel = data.curWaterLevel or self.curWaterLevel
        self.lastSubWaterTime = data.lastSubWaterTime or self.lastSubWaterTime
        self.addSpeed = data.addSpeed or GameConfig.PlayerAttribute.AddSpeed
        self.closeDamage = data.closeDamage or GameConfig.PlayerAttribute.CloseDamage
        self.remoteDamage = data.remoteDamage or GameConfig.PlayerAttribute.RemoteDamage
        self.lastPosition = data.lastPosition
        self.buffData = data.buffData or {}
        self.inventoryInfo = data.inventoryInfo or {}
        self.armorInventoryInfo = data.armorInventoryInfo or {}
        self.killMonsterNum = data.killMonsterNum or 0
        self.killPlayerNum = data.killPlayerNum or 0
        self.surviveDays = data.surviveDays or 0
        self.enderInventory = data.enderInventory or {}
        self.vip_data = data.vip_data or { level = 0, time = 0 }
    end
    self:syncVipInfo()
    self:fillAllInvetory(self.inventoryInfo)
    self:fillEnderInvetory(self.enderInventory)
    if not (InventoryUtil:getPlayerInventoryHaveItem(self, 3279) or InventoryUtil:getPlayerEnderInventoryHaveItem(self, 3279)) then
        self:addItem(3279, 1)
    end
    self.entityPlayerMP:getFoodStats():setFoodLevel(self.curFoodLevel)
    self.entityPlayerMP:getFoodStats():setWaterLevel(self.curWaterLevel)
    self:subHealth(self.maxHealth - self.curHealth)
    BuffManager:ReloadPlayerBuffData(self.buffData, self.userId)
    self:updateDefense()
    self:updateSurviveDiary()
    self:refreshWaterLevel()
    self:refreshFoodLevel()

    self:teleInitPos()
end

function GamePlayer:checkHangUp(ticks)
    if os.time() - self.lastMoveTime >= HangUpManager.checkTime then
        BaseListener.onPlayerLogout(self.userId)
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
    end
end

function GamePlayer:sendPlayerSettlement()
    local settlement = {}
    settlement.rank = GameMatch:getPlayerRank(self)
    settlement.name = self.name or ""
    settlement.isWin = 0
    settlement.gold = 0
    --settlement.points = nil
    settlement.available = self.available
    settlement.hasGet = self.hasGet
    settlement.vip = self.vip
    settlement.kills = self.kills
    settlement.adSwitch = self.adSwitch or 0
    settlement.result_add_exp = 0
    settlement.result_add_yaoshi = 0
    settlement.result_add_integral = 0
    if settlement.gold <= 0 then
        settlement.adSwitch = 0
    end

    --死亡结算相关
    --TODO
    --[[settlement.liveTime = os.clock() - self.loginTime --存活时间
    settlement.killMonsterNum = self.killMonsterNum --击杀丧尸数量
    settlement.liveNights = WorldTimeManager.DayCount - self.loginDayCount --度过黑夜
    settlement.killBossNum = self.killBossNum --击杀boss
    --]]
    local data = {}

    data.designation = ""
    for _, title in pairs(self.titles) do
        if title.status == GameTitle.TitleStatus.Use then
            local config = TitleConfig:getTitleById(title.id)
            if config then
                data.designation = config.Name
            end
        end
    end

    data.name = self.name or ""

    --data.grade----------------

    local lifetime = self.lifeTime or 0
    local lifetimeH = math.floor(lifetime / 3600)
    local lifetimeM = math.floor((lifetime % 3600) / 60)
    local lifetimeS = math.floor(lifetime % 60)
    if lifetimeH >= 10 then
        data.lifetimeText = lifetimeH .. ":"
    else
        data.lifetimeText = "0" .. lifetimeH .. ":"
    end
    if lifetimeM >= 10 then
        data.lifetimeText = data.lifetimeText .. lifetimeM .. ":"
    else
        data.lifetimeText = data.lifetimeText .. "0" .. lifetimeM .. ":"
    end
    if lifetimeS >= 10 then
        data.lifetimeText = data.lifetimeText .. lifetimeS
    else
        data.lifetimeText = data.lifetimeText .. "0" .. lifetimeS
    end
    data.lifetimeScore = "+" .. tostring(lifetime * GameConfig.LifetimeScore)

    data.killDeadText = tostring(self.killDeadNum)
    data.killDeadScore = "+" .. self.killDeadScore

    data.killBossText = tostring(self.killBossNum)
    data.killBossScore = "+" .. self.killBossScore

    data.totalText = tostring(self.killBossScore + self.killDeadScore + lifetime * GameConfig.LifetimeScore)

    data.grade = GradeConfig:findGradeByScore(tonumber(data.totalText))

    --HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), true)

    GamePacketSender:sendWalkingDeadDeathSettlement(self.rakssid, json.encode(data))
end

function GamePlayer:updateSurviveDiary()
    local itemList = {}
    local data = {}
    data.itemId = 3279
    local formatStr = TextFormat:getTipType("gui_walkingdead_1") .. "%d\n" ..
            TextFormat:getTipType("gui_walkingdead_2") .. "%d\n" ..
            TextFormat:getTipType("gui_walkingdead_3") .. "%d"
    --self.surviveDays = WorldTimeManager.DayCount - self.loginDayCount
    data.detail = string.format(formatStr,
            self.killMonsterNum, self.killPlayerNum, self.surviveDays)
    table.insert(itemList, data)
    GamePacketSender:sendWalkingDeadDeathItemsDetail(self.rakssid, itemList)
end

function GamePlayer:initSupplyDataFromDB(data)
    if data.__first then
        self.vip_day_key = DateUtil.getYearDay()
    end
    if not data.__first then
        self.tiro_box = data.tiro_box or false
        self.daily_box = data.daily_box or 0
        self.vip_day_key = data.vip_day_key or 0
        self.vip_box = data.vip_box or 0
    end
end

function GamePlayer:initVIPBoxData()
    local todayKey = DateUtil.getYearDay()
    if todayKey ~= self.vip_day_key then
        local vip = self:getMaxVip()
        if vip > 0 then
            self.vip_day_key = todayKey
            self.vip_box = 0
        end
    end
end
function GamePlayer:onAddRobot(robotId)
    self.robotId = robotId
    self.hasRobot = true
end

function GamePlayer:checkRefreshRobot(ticks)
    if os.time() - self.lastAttackTime >= MonsterRuleConfig.robotRefreshTime then
        RobotManager:refreshRobot(self.rakssid)
    end
end

function GamePlayer:checkPlayerAttribute()
    self.lastSubWaterTime = self.lastSubWaterTime + 1
    if self.lastSubWaterTime >= self.waterLevelSubTime then
        self:subWaterLevel(self.waterLevelSubValue)
        self.lastSubWaterTime = 0
    end
end

function GamePlayer:playerInRegion(minX, maxX, minY, maxY, minZ, maxZ)
    local pos = self:getPosition()
    return (pos.x >= minX and pos.x <= maxX)
            and (pos.y >= minY and pos.y <= maxY)
            and (pos.z >= minZ and pos.z <= maxZ)
end

function GamePlayer:teleInitPos()
    self.realLife = true
    if self.lastPosition then
        self:teleportPos(VectorUtil.newVector3(self.lastPosition[1], self.lastPosition[2], self.lastPosition[3]))
    else
        self:teleportPos(GameConfig:getRandomPos())
    end
end

function GamePlayer:onPlayerRespawn()
    self:initPlayerAttribute()
    HostApi.log(string.format("GamePlayer:onPlayerRespawn %s", tostring(self.rakssid)))

    self.entityPlayerMP:inventoryForceUpdate()
    self.entityPlayerMP:markInventoryDirty()

    self:changeMaxHealth(self.maxHealth)
    self.entityPlayerMP:getFoodStats():setmaxFoodLevel(self.maxFoodLevel)
    self.entityPlayerMP:getFoodStats():setFoodLevel(self.curFoodLevel)
    self.entityPlayerMP:getFoodStats():setFoodHealValue(self.satietyHeal)
    self.entityPlayerMP:getFoodStats():setmaxWaterLevel(self.maxWaterLevel)
    self.entityPlayerMP:getFoodStats():setWaterLevel(self.curWaterLevel)
    self.isLife = true
    self:updateDefense()
    self:changeSpeed(self.addSpeed)
    self.entityPlayerMP:setFireDurationTime(GameConfig.FireDurationTime)
    if self.enderInventory.item ~= nil or self.enderInventory.gun ~= nil then
        self:fillEnderInvetory(self.enderInventory)
        if not (InventoryUtil:getPlayerInventoryHaveItem(self, 3279) or InventoryUtil:getPlayerEnderInventoryHaveItem(self, 3279)) then
            self:addItem(3279, 1)
        end
    end
    self:refreshWaterLevel()
    self:refreshFoodLevel()
    self:updateSurviveDiary()
end

function GamePlayer:initPlayerAttribute()
    self.maxHealth = GameConfig.PlayerAttribute.MaxHealth
    self.curHealth = self.maxHealth
    self.maxFoodLevel = GameConfig.PlayerAttribute.MaxFoodLevel
    self.curFoodLevel = self.maxFoodLevel
    self.onHurtSubFoodLevel = GameConfig.PlayerAttribute.onHurtSubFoodLevel
    self.maxWaterLevel = GameConfig.PlayerAttribute.MaxWaterLevel
    self.curWaterLevel = GameConfig.PlayerAttribute.MaxWaterLevel
    self.maxDefense = GameConfig.PlayerAttribute.MaxDefense
    self.curDefense = GameConfig.PlayerAttribute.MaxDefense
    self.addSpeed = GameConfig.PlayerAttribute.AddSpeed
    self.closeDamage = GameConfig.PlayerAttribute.CloseDamage
    self.remoteDamage = GameConfig.PlayerAttribute.RemoteDamage
    self.satietyHeal = GameConfig.PlayerAttribute.SatietyHeal
    self.waterLevelSubValue = GameConfig.PlayerAttribute.waterLevelSubValue
    self.waterLevelSubTime = GameConfig.PlayerAttribute.waterLevelSubTime
    self.lastPosition = nil
    self.killMonsterNum = 0
    self.killPlayerNum = 0
    --self.loginDayCount = WorldTimeManager.DayCount
    self.surviveDays = 0 --存活天数
end

--删除依存的机器人
function GamePlayer:onRemoveRobot()
    --HostApi.log(string.format("GamePlayer:onRemoveRobot %d", self.robotId))
    local robot = RobotManager:getRobot(self.robotId)
    if robot ~= nil then
        robot:onRemove()

        self.robotId = 0
        self.hasRobot = false
    end
end

function GamePlayer:onKill(dier)
    --HostApi.log("GamePlayer:onKill")
    self.killPlayerNum = self.killPlayerNum + 1
    self:updateSurviveDiary()
    MsgSender.sendMsg(IMessages:msgPlayerKillPlayer(dier.name, self.name))
    local pos = dier:getPosition()
    local params = {
        userId = dier.userId,
        x = pos.x,
        y = pos.y,
        z = pos.z
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(self.rakssid, "syncDiePlayerMsg", "attacked", data)

end

function GamePlayer:getMaxVip()
    return self.vip_data.level, self.vip_data.time
end

function GamePlayer:syncVipInfo()
    local vip, time = self:getMaxVip()
    if TableUtil.isSameTable(self.vip_record, { vip, time }) then
        return
    end
    self.vip_record = { vip, time }
    local vipTime = math.max(0, time - os.time())
    GamePacketSender:sendWalkingDeadShowVipInfo(self.rakssid, vip, vipTime)
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self.realLife = false
    end
    --self:clearInv()
    --HostApi.log("self:clearInv " .. tostring(self.rakssid))
    self:initPlayerAttribute()
    HostApi.log(string.format("GamePlayer:onDie %s", tostring(self.rakssid)))
    BuffManager:removePlayerAllBuff(self.userId)
    self:sendPlayerSettlement()
    self:saveData()
    self.lifeTime = 0 --登录时间
    self.killDeadNum = 0
    self.killDeadScore = 0
    self.killBossNum = 0 --击杀boss数
    self.killBossScore = 0
    self.isNeedItemInit = true
    DbUtil:SavePlayerGameData(self, true)
end

function GamePlayer:onQuit()
    local isCount = 0
    if RewardManager:isUserRewardFinish(self.userId) then
        isCount = 1
    end
    ReportManager:reportUserData(self.userId, self.kills, GameMatch:getPlayerRank(self), isCount)

    self:onRemoveRobot()
end

function GamePlayer:saveData()
    local position = self:getPosition()
    self.inventoryInfo = InventoryUtil:getPlayerInventoryInfo(self)
    self.armorInventoryInfo = InventoryUtil:getPlayerArmorInventoryInfo(self)
    self.enderInventory = InventoryUtil:getPlayerEnderInventoryInfo(self)
    self.buffData = BuffManager:getPlayerBuffData(self.userId)
    if self.isLife then
        self.lastPosition = { position.x, position.y, position.z }
        self.curHealth = self:getHealth()
    else
        self.curHealth = GameConfig.PlayerAttribute.MaxHealth
    end
end

function GamePlayer:onAttack(target, type, damageType)
    if damageType == "player.gun" or damageType == "player" then
        self:onAttackPlayer(target, type)
        if ItemsConfig:isLongRangeWeapon(self:getHeldItemId()) then
            local params = {
                userId = target.userId
            }
            local data = DataBuilder.new():fromTable(params):getData()
            PacketSender:sendServerCommonData(self.rakssid, "doAttackMsg", "attack", data)
        end
    end
end

function GamePlayer:onAttackPlayer(target, type)
    self.lastAttackTime = os.time()
    local properties = GunConfig:getGunPropertyByItemId(tostring(self.gunId))
    if properties ~= nil then
        for _, propertyid in pairs(properties) do
            if propertyid ~= nil then
                local property = PropertyConfig:getPropertyById(propertyid)
                if property ~= nil then
                    property:onPlayerUsed(target, self.gunId)
                end
            end
        end
    end
end

function GamePlayer:onAttacked(attacker, hurtFrom, damageType)
    if self.isLife == true then
        local damage = 0
        if hurtFrom == Define.TargetType.TargetMonster then
            damage = attacker.attack
        else
            if damageType == "player" then
                damage = attacker.closeDamage
            elseif damageType == "player.gun" then
                damage = attacker.remoteDamage
            end
        end
        damage = damage + self.infection
        damage = self:subDefense(damage)

        if damage > 0 then
            if self.curHealth - damage >= 0 then
                self.curHealth = self.curHealth - damage
            else
                self.curHealth = 0
            end
            self:subFoodLevel(self.onHurtSubFoodLevel)
            self:subHealth(damage)
            if self:getHealth() <= 0 and (damageType == "player" or damageType == "player.gun") then
                attacker:onKill(self)
            end
        elseif damage == 0 then
            self:doHurt(0)
        end
        if damageType == "player.gun" or damageType == "player" then
            if attacker:getHeldItemId() ~= nil and ItemsConfig:isLongRangeWeapon(attacker:getHeldItemId()) then
                local params = {
                    userId = attacker.userId
                }
                local data = DataBuilder.new():fromTable(params):getData()
                PacketSender:sendServerCommonData(self.rakssid, "doAttackMsg", "attacked", data)
            end
        end
        return true
    end
    return false
end

function GamePlayer:doHurt(hurtValue)
    if self.entityPlayerMP == nil then
        return
    end

    self.entityPlayerMP:doHurt(hurtValue or 0)

    local data = DataBuilder.new():getData()
    PacketSender:sendServerCommonData(self.rakssid, "doHurtMsg", "", data)
end

function GamePlayer:subFoodLevel(value)
    if self.isLife then
        if self.curFoodLevel - value >= 0 then
            self.curFoodLevel = self.curFoodLevel - value
        else
            self.curFoodLevel = 0
        end
        self.entityPlayerMP:getFoodStats():setFoodLevel(self.curFoodLevel)
        if self.curFoodLevel < self.maxFoodLevel * 0.2 then
            BuffManager:createBuff(self.userId, self.userId, tonumber(GameConfig.PlayerStateBuffId.Hunger))
        elseif self.curFoodLevel < self.maxFoodLevel * 0.9 then
            local uniqueId = tostring(self.userId) .. "#" .. GameConfig.PlayerStateBuffId.Satiety
            BuffManager:cleanBuff(uniqueId)
        end
    end
end

function GamePlayer:addFoodLevel(value)
    if self.isLife then
        if self.curFoodLevel + value > self.maxFoodLevel then
            self.curFoodLevel = self.maxFoodLevel
        else
            self.curFoodLevel = self.curFoodLevel + value
        end
        self.entityPlayerMP:getFoodStats():setFoodLevel(self.curFoodLevel)
        if self.curFoodLevel >= self.maxFoodLevel * 0.9 then
            BuffManager:createBuff(self.userId, self.userId, GameConfig.PlayerStateBuffId.Satiety)
        end
        if self.curFoodLevel >= self.maxFoodLevel * 0.2 then
            local uniqueId = tostring(self.userId) .. "#" .. GameConfig.PlayerStateBuffId.Hunger
            BuffManager:cleanBuff(uniqueId)
        end
    end
end

function GamePlayer:refreshFoodLevel()
    self:addFoodLevel(0)
    self:subFoodLevel(0)
end

function GamePlayer:refreshWaterLevel()
    self:subWaterLevel(0)
    self:addWaterLevel(0)
end

function GamePlayer:subWaterLevel(value)
    if self.isLife then
        if self.curWaterLevel - value >= 0 then
            self.curWaterLevel = self.curWaterLevel - value
        else
            self.curWaterLevel = 0
        end
        self.entityPlayerMP:getFoodStats():setWaterLevel(self.curWaterLevel)
        if self.curWaterLevel == 0 then
            if self.isLife then
                BuffManager:createBuff(self.userId, self.userId, GameConfig.PlayerStateBuffId.Thirst)
            end
        end
        if self.curWaterLevel ~= 0 then
            local uniqueId = tostring(self.userId) .. "#" .. GameConfig.PlayerStateBuffId.Thirst
            BuffManager:cleanBuff(uniqueId)
        end
        if self.curWaterLevel < self.maxWaterLevel then
            local uniqueId = tostring(self.userId) .. "#" .. GameConfig.PlayerStateBuffId.DrinkFull
            BuffManager:cleanBuff(uniqueId)
        end
    end
end

function GamePlayer:addWaterLevel(value)
    if self.isLife then
        if self.curWaterLevel + value > self.maxWaterLevel then
            self.curWaterLevel = self.maxWaterLevel
        else
            self.curWaterLevel = self.curWaterLevel + value
        end
        self.entityPlayerMP:getFoodStats():setWaterLevel(self.curWaterLevel)
        if self.curWaterLevel == self.maxWaterLevel then
            BuffManager:createBuff(self.userId, self.userId, GameConfig.PlayerStateBuffId.DrinkFull)
        end
        if self.curWaterLevel ~= 0 then
            local uniqueId = tostring(self.userId) .. "#" .. GameConfig.PlayerStateBuffId.Thirst
            BuffManager:cleanBuff(uniqueId)
        end
    end
end

function GamePlayer:updateNowDefense(player)
    local newCurDamage = 0
    local newMaxDefense = 0
    if self.curUseArmorId ~= nil then
        for _, item in pairs(self.curUseArmorId) do
            local armorValue = ArmorConfig:getArmorValueById(item[1]).ArmorValue
            if armorValue ~= nil then
                newCurDamage = newCurDamage + item[2]
                newMaxDefense = newMaxDefense + armorValue
            end
        end
    end
    player.maxDefense = newMaxDefense
    player.curDefense = newCurDamage
    player:updateDefense()
end

function GamePlayer:subDefense(damage)
    if self.curDefense == nil or self.curDefense <= 0 then
        return damage
    end
    local result = 0
    if self.curDefense - damage >= 0 then
        result = self:subArmorMeta(damage)
        self:updateNowDefense(self)
        return result
    else
        result = damage - self.curDefense
        self:subArmorMeta(damage)
        self:updateNowDefense(self)
        return result
    end
end

function GamePlayer:subArmorMeta(value)
    local inven = self.entityPlayerMP:getInventory()
    local result = value
    local damage = {}
    for k = 0, result do
        if result == 0 then
            break
        end
        if (self.curUseArmorId[100 + 0] == nil or self.curUseArmorId[100 + 0][2] == 0)
                and (self.curUseArmorId[100 + 1] == nil or self.curUseArmorId[100 + 1][2] == 0)
                and (self.curUseArmorId[100 + 2] == nil or self.curUseArmorId[100 + 2][2] == 0)
                and (self.curUseArmorId[100 + 3] == nil or self.curUseArmorId[100 + 3][2] == 0)
        then
            break
        end
        for i = 0, 3 do
            if self.curUseArmorId[100 + i] ~= nil and self.curUseArmorId[100 + i][2] > 0 then
                if damage[i] == nil then
                    damage[i] = 0
                end
                damage[i] = damage[i] + 1
                result = result - 1
                self.curUseArmorId[100 + i][2] = self.curUseArmorId[100 + i][2] - 1
                if result == 0 then
                    break
                end
            end
        end
    end
    for i = 3, 0, -1 do
        if self.curUseArmorId[100 + i] ~= nil then
            local armor = ArmorConfig:getArmorValueById(self.curUseArmorId[100 + i][1])
            if armor ~= nil and damage[i] ~= nil then
                inven:damageItem(100 + i, damage[i] * armor.multiple)
                if self.curUseArmorId[100 + i][2] == 0 then
                    inven:destroyArmor(100 + i)
                    self.curUseArmorId[100 + i] = nil
                end
            end
        end
    end
    return result
end

function GamePlayer:updateDefense()
    if self.entityPlayerMP == nil then
        return
    end
    self.entityPlayerMP:changeDefense(self.curDefense, self.maxDefense)
end

function GamePlayer:changeMaxHealthSingle(health)
    if self.entityPlayerMP ~= nil then
        self.maxHealth = self.maxHealth + health
        self.entityPlayerMP:changeMaxHealthSingle(health)
    end
end

function GamePlayer:onChangeArmor(player, newUseArmors)
    if newUseArmors == nil then
        --当没有装备时
        for _, oldArmor in pairs(self.curUseArmorId) do
            self:onUnloadArmor(player, oldArmor[1])
            self.curUseArmorId[oldArmor[3]] = nil
        end
        return
    end
    for _, oldArmor in pairs(self.curUseArmorId) do
        --被换下的装备
        if newUseArmors[oldArmor[3]] == nil then
            self:onUnloadArmor(player, oldArmor[1])
            self.curUseArmorId[oldArmor[3]] = nil
        end
    end
    for _, newArmor in pairs(newUseArmors) do
        --新的装备
        if self.curUseArmorId[newArmor[3]] == nil or self.curUseArmorId[newArmor[3]][1] ~= newArmor[1] then
            self:onEquipArmor(player, newArmor[1])
        end
        self.curUseArmorId[newArmor[3]] = newArmor
    end
    self:updateNowDefense(self)
end

function GamePlayer:onEquipArmor(player, itemId)
    local properties = ItemsConfig:getItemPropertyByItemId(itemId)
    if properties ~= nil then
        for i, propertyid in pairs(properties) do
            if propertyid ~= nil then
                local property = PropertyConfig:getPropertyById(propertyid)
                if property ~= nil then
                    property:onPlayerEquip(player, itemId)
                end
            end
        end
    end
end

function GamePlayer:onUnloadArmor(player, itemId)
    local properties = ItemsConfig:getItemPropertyByItemId(itemId)
    if properties ~= nil then
        for i, propertyid in pairs(properties) do
            if propertyid ~= nil then
                local property = PropertyConfig:getPropertyById(propertyid)
                if property ~= nil then
                    property:onPlayerUnload(player, itemId)
                end
            end
        end
    end
end

function GamePlayer:onUsedItem(player, itemId)
    local properties = ItemsConfig:getItemPropertyByItemId(itemId)
    if properties ~= nil then
        for _, propertyid in pairs(properties) do
            if propertyid ~= nil then
                local property = PropertyConfig:getPropertyById(propertyid)
                if property ~= nil then
                    property:onPlayerUsed(player, itemId)
                end
            end
        end
    end
end

function GamePlayer:changeSpeed(speed)
    self.addSpeed = self.addSpeed + speed
    self.entityPlayerMP:setSpeedAdditionLevel(self.addSpeed)
end

function GamePlayer:changeDamage(Damage, type, gunId)
    if type == 0 then
        self.closeDamage = Damage
        self.remoteDamage = GameConfig.PlayerAttribute.RemoteDamage
    else
        self.remoteDamage = Damage
        self.closeDamage = GameConfig.PlayerAttribute.CloseDamage
    end
    self.gunId = gunId
end

function GamePlayer:addLifeTime()
    if self.lifeTime == nil or self.isLife == false then
        return
    end
    self.lifeTime = self.lifeTime + 1
end

function GamePlayer:fillAllInvetory(inventory)
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
                local armor = ArmorConfig:getArmorValueById(self.ItemId)
                local maxMeta = 0
                if armor ~= nil then
                    maxMeta = armor.ArmorValue * armor.multiple
                end
                self:addItem(item.id, item.num, item.meta, false, maxMeta)
            end
        end
    end
end

function GamePlayer:addSettlementGold()
    local time = os.time() - self.inGameTime
    local gold = ((time - time % 60) / 60) * 5
    if gold > 150 then
        gold = 150
    end
    RewardManager:getUserGoldReward(self.userId, gold, function()
    end)
end

function GamePlayer:inFire()
    local time = os.time()
    if time - self.lastHurtInFire >= GameConfig.FireDamageCd then
        self.lastHurtInFire = time
        return true
    end
    return false
end

function GamePlayer:PlayerItemInit()
    if self.isNeedItemInit == false then
        return
    end
    for _, item in pairs(PlayerItemInitConfig.ItemInit) do
        self:addItem(item.ItemId, item.Num, 0, false)
    end
    self.isNeedItemInit = false
end

function GamePlayer:onDropItem(itemID, itemDamage, itemCount)
    if itemID == 3279 then
        return false
    end

    local item = ItemsConfig:getItemsByItemId(itemID)
    if item == nil then
        return true
    end

    local config = PlayerDropItemProbabilityConfig.settings[item.quality]
    if config == nil then
        return true
    end

    if config.probability > math.random(100) and self.entityPlayerMP then
        self:dropItemIntoChest(itemID, itemDamage, itemCount)
        return true
    end

    return false
end

function GamePlayer:createChest()
    local pos = VectorUtil.toVector3i(self:getPosition())
    self.dropItemChest = ChestManager:createChest(pos, self.name)
end

function GamePlayer:dropItemIntoChest(itemID, itemDamage, itemCount)
    if self.dropItemChest == nil then
        return
    end

    --TODO:添加掉落物品到箱子中
    self.dropItemChest:addItem(itemID, itemDamage, itemCount)
end

return GamePlayer

--endregion
