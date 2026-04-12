---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "config.PlayerLevelConfig"
require "config.EquipmentConfig"
require "config.ArmsConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.isInGame = false
    self.isDead = false
    self.money = 0
    self.score = 0
    self.kills = 0
    self.gameScore = 0
    self.gameLevel = 0
    self.scorePoints = {}

    self.hp = 0
    self.fullHp = 20
    self.attack = 0
    self.defense = 0
    self.speed = 0
    self.attackX = 0
    self.exp = 0
    self.level = 0
    self.isGoldAddition = false
    self.isExpAddition = false
    self.isSpeedAddition = false
    self.createTime = os.time()
    self.inSceneTime = os.time()

    self.arms = {}
    self.arms[1] = 3

    self.equipment = {}
    self.equipment[EquipmentConfig.PART_NAME_HELMET] = 0
    self.equipment[EquipmentConfig.PART_NAME_PLATE] = 0
    self.equipment[EquipmentConfig.PART_NAME_LEGS] = 0
    self.equipment[EquipmentConfig.PART_NAME_BOOTS] = 0

    self:reset()

end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
end

function GamePlayer:setWeekRank(id, rank, score)
    local npc = RankNpcConfig:getRankNpc(id)
    if npc then
        RankManager:addPlayerWeekRank(self, npc.key, rank, score, function(npc)
            npc:sendRankData(self)
        end, npc)
    end
end

function GamePlayer:setDayRank(id, rank, score)
    local npc = RankNpcConfig:getRankNpc(id)
    if npc then
        RankManager:addPlayerDayRank(self, npc.key, rank, score, function(npc)
            npc:sendRankData(self)
        end, npc)
    end
end

function GamePlayer:reset()
    self.isDead = false
    self.score = 0
    self.kills = 0
    self.gold = 0
    self:teleInitPos()
    self:changeMaxHealth(self.fullHp)
    self:initScorePoint()
    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        self.money = data.money
        self.level = data.level
        self.arms = data.arms or {}
        if data.equipment == nil then
            self.equipment = {}
            self.equipment[EquipmentConfig.PART_NAME_HELMET] = 0
            self.equipment[EquipmentConfig.PART_NAME_PLATE] = 0
            self.equipment[EquipmentConfig.PART_NAME_LEGS] = 0
            self.equipment[EquipmentConfig.PART_NAME_BOOTS] = 0
        else
            self.equipment = data.equipment
        end
        self.exp = data.exp or 0
        self.isGoldAddition = data.isGoldAddition or false
        self.isExpAddition = data.isExpAddition or false
        self.isSpeedAddition = data.isSpeedAddition or false
    end
    self:initPlayer()
end

function GamePlayer:initPlayer()
    if self.isInGame then
        self:teleScenePos()
    else
        self:teleInitPos()
    end
    self:setFoodLevel(20)
    self:setCurrency(self.money)
    self:setShowName(self:buildShowName())
    self:resetBaseAttr()
    self:clearInv()
    self:equipArmors()
    self:initArm()
    if self.isSpeedAddition then
        self:addEffect(PotionID.MOVE_SPEED, 60 * 60 * 24 * 30, 1)
    end
    self:sendArmsData()
    self:sendEquipmentData()
    self:updateExp(self.level, self.exp, PlayerLevelConfig:getUpgradeExpByLv(self.level + 1))
    self.attackX = self:getAttackX()
    self.isDead = false
end

function GamePlayer:addExp(exp)
    if exp == 0 then
        return
    end
    if self.level >= GameConfig.playerAttr.maxLv then
        return
    end
    if self.isExpAddition then
        exp = 2 * exp
    end
    self.exp = self.exp + exp
    local upgradeExp = PlayerLevelConfig:getUpgradeExpByLv(self.level + 1)
    while self.exp >= upgradeExp do
        self.exp = self.exp - upgradeExp
        self:upgrade()
        upgradeExp = PlayerLevelConfig:getUpgradeExpByLv(self.level + 1)
    end
    self:updateExp(self.level, self.exp, upgradeExp)
end

function GamePlayer:upgrade()
    self.level = self.level + 1
    self.hp = self:getHealth()
    self:resetBaseAttr()
    if self.level == GameConfig.playerAttr.maxLv then
        self.exp = 0
    end
    self:setShowName(self:buildShowName())
    self.hp = self.hp + self.fullHp * GameConfig.PlayerUpgradeHealthReply
    self.hp = math.min(self.fullHp, self.hp)
    self:setHealth(self.hp)
    MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:upgradePlayerHint())
end

function GamePlayer:initArm()
    for lv, status in pairs(self.arms) do
        if status == 3 then
            local arm = ArmsConfig:getArmByLv(lv)
            if arm ~= nil then
                self:addGunItem(arm.itemId, 1, 0, 100)
            end
        end
    end
end

function GamePlayer:equipArmors()
    self:equipArmorByPart(EquipmentConfig.PART_NAME_HELMET)
    self:equipArmorByPart(EquipmentConfig.PART_NAME_PLATE)
    self:equipArmorByPart(EquipmentConfig.PART_NAME_LEGS)
    self:equipArmorByPart(EquipmentConfig.PART_NAME_BOOTS)
end

function GamePlayer:equipArmorByPart(part)
    local equipment = EquipmentConfig:getEquipment(part, self.equipment[part])
    if equipment == nil then
        return
    end
    self:equipArmor(equipment.itemId, 0)
end

function GamePlayer:addMoney(money)
    if self.isGoldAddition then
        money = 1.5 * money
    end
    money = NumberUtil.getIntPart(money)
    self:addCurrency(money)
    self.score = self.score + money
    self.gameScore = self.gameScore + money
    self.appIntegral = self.appIntegral + money
end

function GamePlayer:subMoney(money)
    self:subCurrency(money)
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:resetBaseAttr()
    self:setSpeedAddition(0)
    self.fullHp = GameConfig.playerAttr.hp * (1 + GameConfig.growthX * self.level ^ GameConfig.correctionX)
    self.attack = GameConfig.playerAttr.attack * (1 + GameConfig.growthX * self.level ^ GameConfig.correctionX)
    self.defense = GameConfig.playerAttr.defense * (1 + GameConfig.growthX * self.level ^ GameConfig.correctionX)
    self.addHp = GameConfig.playerAttr.addHp
    self.speed = 0
    self.attack = self.attack + self:getEquipArmAttack()
    self:addEquipmentAttr(EquipmentConfig.PART_NAME_HELMET)
    self:addEquipmentAttr(EquipmentConfig.PART_NAME_PLATE)
    self:addEquipmentAttr(EquipmentConfig.PART_NAME_LEGS)
    self:addEquipmentAttr(EquipmentConfig.PART_NAME_BOOTS)
    self:changeMaxHealth(self.fullHp)
    self:setSpeedAddition(self.speed)
end

function GamePlayer:addEquipmentAttr(part)
    local equipment = EquipmentConfig:getEquipment(part, self.equipment[part])
    if equipment == nil then
        return
    end
    if equipment.hp ~= 0 then
        self.fullHp = self.fullHp + equipment.hp
    end
    if equipment.attack ~= 0 then
        self.attack = self.attack + equipment.attack
    end
    if equipment.defense ~= 0 then
        self.defense = self.defense + equipment.defense
    end
    if equipment.speed ~= 0 then
        self.speed = self.speed + equipment.speed
    end
    if equipment.addHp ~= 0 then
        self.addHp = self.addHp + equipment.addHp
    end
end

function GamePlayer:upgradeEquipment(part)
    local upEquipment = EquipmentConfig:getEquipment(part, self.equipment[part] + 1)
    if upEquipment == nil then
        return false
    end
    if self.money < upEquipment.price then
        return false
    end
    self:subMoney(upEquipment.price)
    local equipment = EquipmentConfig:getEquipment(part, self.equipment[part])
    if equipment ~= nil then
        self:removeItem(EquipmentConfig:getEquipment(part, self.equipment[part]).itemId, 1)
    end
    self.equipment[part] = self.equipment[part] + 1
    self.hp = self:getHealth()
    self:resetBaseAttr()
    self:setHealth(self.hp)
    self:equipArmorByPart(part)
    return true
end

function GamePlayer:canBuyArm(level)
    local arm = ArmsConfig:getArmByLv(level)
    if arm ~= nil then
        if self.money < arm.price then
            return false
        end
    end
    local maxLv = 1
    for lv, status in pairs(self.arms) do
        if status >= 2 then
            maxLv = math.max(maxLv, lv)
        end
    end
    return level <= maxLv + 1
end

function GamePlayer:hasArm(level)
    local arm = self.arms[level]
    if arm then
        return arm >= 2
    end
    return false
end

function GamePlayer:buyArm(level)
    if self:canBuyArm(level) then
        local arm = ArmsConfig:getArmByLv(level)
        if arm ~= nil then
            self:subMoney(arm.price)
            if self.arms[level] == nil then
                self.arms[level] = 2
                self:useArm(level)
                return true
            end
        end
    end
    return false
end

function GamePlayer:useArm(level)
    if self.arms[level] == nil then
        return
    end
    self.attack = self.attack - self:getEquipArmAttack()
    for lv, status in pairs(self.arms) do
        if status == 3 then
            self.arms[lv] = 2
            local arm = ArmsConfig:getArmByLv(lv)
            if arm ~= nil then
                self:removeItem(arm.itemId, 1)
            end
        end
    end
    self.arms[level] = 3
    local arm = ArmsConfig:getArmByLv(level)
    if arm ~= nil then
        self:addGunItem(arm.itemId, 1, 0, 100)
    end
    self.attack = self.attack + self:getEquipArmAttack()
    self.attackX = self:getAttackX()
end

function GamePlayer:getEquipArmAttack()
    for lv, status in pairs(self.arms) do
        if status == 3 then
            local arm = ArmsConfig:getArmByLv(lv)
            if arm ~= nil then
                return arm.attack
            end
        end
    end
    return 0
end

function GamePlayer:getAttackX()
    for lv, status in pairs(self.arms) do
        if status == 3 then
            local arm = ArmsConfig:getArmByLv(lv)
            if arm ~= nil then
                return GameConfig.playerAttr.attackX * arm.attackX
            end
        end
    end
    return GameConfig.playerAttr.attackX
end

function GamePlayer:teleInitPos()
    self.isInGame = false
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:teleScenePos()
    self.isInGame = true
    self.inSceneTime = os.time()
    self:teleportPos(GameMatch:getCurGameScene().initPos)
end

function GamePlayer:sendArmsData()
    if self.entityPlayerMP == nil then
        return
    end
    local config = ArmsConfig:getArms()
    local data = {}
    data.props = {}
    local maxLv = 1
    for lv, status in pairs(self.arms) do
        if status >= 2 then
            maxLv = math.max(maxLv, lv)
        end
    end
    for lv1, item in pairs(config) do
        local arm = {}
        arm.id = item.id
        arm.name = item.name
        arm.image = item.image
        arm.price = item.price
        arm.desc = item.desc
        if item.level <= maxLv + 1 then
            arm.status = 1
        else
            arm.status = 0
        end
        for lv2, status in pairs(self.arms) do
            if lv1 == lv2 then
                arm.status = status
            end
        end
        table.insert(data.props, arm)
    end
    self.entityPlayerMP:changeSwitchableProps(json.encode(data))
end

function GamePlayer:sendEquipmentData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = {}
    data.props = {}
    local helmet = self:getEquipmentItemData(EquipmentConfig.PART_NAME_HELMET)
    local plate = self:getEquipmentItemData(EquipmentConfig.PART_NAME_PLATE)
    local legs = self:getEquipmentItemData(EquipmentConfig.PART_NAME_LEGS)
    local boots = self:getEquipmentItemData(EquipmentConfig.PART_NAME_BOOTS)
    if helmet then
        table.insert(data.props, helmet)
    end
    if plate then
        table.insert(data.props, plate)
    end
    if legs then
        table.insert(data.props, legs)
    end
    if boots then
        table.insert(data.props, boots)
    end
    self.entityPlayerMP:changeUpgradeProps(json.encode(data))
end

function GamePlayer:getEquipmentItemData(part)
    local equipment = EquipmentConfig:getEquipment(part, self.equipment[part])
    if equipment == nil then
        return nil
    end
    local item = {}
    item.id = equipment.id
    item.level = equipment.level
    item.name = equipment.name
    item.image = equipment.image
    item.desc = equipment.desc
    item.price = -1
    item.value = self:getEquipmentAttrValue(equipment)
    item.nextValue = self:getEquipmentAttrValue(equipment)
    local upEquipment = EquipmentConfig:getEquipment(part, self.equipment[part] + 1)
    if upEquipment ~= nil then
        item.nextValue = self:getEquipmentAttrValue(upEquipment)
        item.price = upEquipment.price
    end
    return item
end

function GamePlayer:getEquipmentAttrValue(equipment)
    if equipment.part == EquipmentConfig.PART_NAME_HELMET then
        return equipment.addHp
    end
    if equipment.part == EquipmentConfig.PART_NAME_PLATE then
        return equipment.defense
    end
    if equipment.part == EquipmentConfig.PART_NAME_LEGS then
        return equipment.hp
    end
    if equipment.part == EquipmentConfig.PART_NAME_BOOTS then
        return equipment.speed
    end
    return 0
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    -- pureName line
    local namePrefix = TextFormat.colorGold .. "[Lv" .. tostring(self.level) .. "]"
    PlayerIdentityCache:buildNameList(self.userId, nameList, self.name, namePrefix)
    return table.concat(nameList, "\n")
end

function GamePlayer:onKill()
    self.kills = self.kills + 1
    self.scorePoints[ScoreID.KILL] = self.kills
end

function GamePlayer:onGameLevel(gold, exp)
    if self.isDead then
        return
    end
    self.gameLevel = self.gameLevel + 1
    self:addMoney(gold)
    self:addExp(exp)
end

function GamePlayer:onTick()
    if (os.time() - self.createTime) % 10 ~= 0 then
        return
    end
    self:onLifeRecovery()
    if self.isInGame == false then
        MsgSender.sendTopTipsToTarget(self.rakssid, 12, IMessages:msgGameRunning())
    end
end

function GamePlayer:onLifeRecovery()
    self:addHealth(self.addHp)
end

function GamePlayer:onAttacked(damage, effect)
    damage = math.max(0, damage)
    if damage ~= 0 then
        self:subHealth(damage)
    end
    if effect and effect.id ~= 0 then
        self:addEffect(effect.id, effect.time, effect.lv)
    end
end

function GamePlayer:onQuit()
    local rank = GameMatch:getPlayerRank(self)
    if self.isInGame then
        RewardManager:getUserGoldReward(self.userId, rank, function(data)

        end)
    end
    ReportManager:reportUserData(self.userId, NumberUtil.getIntPart(self.kills / 20), rank)
end

return GamePlayer

--endregion


