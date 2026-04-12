---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "config.ScoreConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.money = 0
    self.fashions = {}
    self.team = nil
    self.score = 0
    self.occupationId = 0
    self.config = nil
    self.defense = 0
    self.damage = 0
    self.attackX = 0
    self.addDefense = 0
    self.kills = 0
    self.flag = nil
    self:initScorePoint()
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.FLAG] = 0
    self.scorePoints[ScoreID.KILL] = 0
end

function GamePlayer:reset()
    self.team = nil
    self.score = 0
    self.occupationId = 0
    self.config = nil
    self.defense = 0
    self.damage = 0
    self.attackX = 0
    self.addDefense = 0
    self.kills = 0
    self.flag = nil
    self:clearEffects()
    self:clearInv()
    self:resetFood()
    self:initScorePoint()
    if GameMatch:isSelectRole() then
        self:teleSelectRole()
    else
        self:teleInitPos()
    end
    self:resetClothes()
    self:useFashions()
    self:changeMaxHealth(20)
    self:sendSkillData()
    self:setShowName(self:buildShowName())
    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        self.money = data.money
        self.fashions = data.fashions or {}
    end
    self:reset()
    self:setCurrency(self.money)
    self:sendFashionData()
end

function GamePlayer:useFashions()
    for id, status in pairs(self.fashions) do
        local fashion = FashionStoreConfig:getFashion(id)
        if fashion ~= nil and status == 3 then
            if fashion.effect.id ~= 0 then
                self:addEffect(fashion.effect.id, fashion.effect.time, fashion.effect.lv)
            end
            self:changeClothes(fashion.actor.name, fashion.actor.id)
        end
    end
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:resetFood()
    self:setFoodLevel(20)
end

function GamePlayer:teleInitPos()
    self.isInGame = false
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:teleSelectRole()
    self.isInGame = true
    self:teleportPos(GameConfig.selectRolePos)
end

function GamePlayer:teleScenePos()
    if self.team ~= nil then
        self:teleportPos(self.team.initPos)
    end
    self:resetFood()
end

function GamePlayer:respawnInGame()
    self:teleScenePos()
    self:useFashions()
    self:onSelectRole(self.config)
end

function GamePlayer:getDisplayName()
    if self.team ~= nil then
        return self.name .. self.team:getDisplayName()
    else
        return self.name
    end
end

function GamePlayer:setTeam(team)
    if team then
        self.team = team
        team:addPlayer()
    end
    self:setShowName(self:buildShowName())
end

function GamePlayer:getTeamId()
    if self.team then
        return self.team.id
    end
    return 0
end

function GamePlayer:onSelectRole(config)
    if config ~= nil then
        self.config = config
        self.occupationId = self.config.id
        self.defense = config.defense
        self.damage = config.damage
        self.attackX = config.attackX
        self:changeMaxHealth(config.health)
        self:sendSkillData()
        for i, chothes in pairs(self.config.actors) do
            self:changeClothes(chothes.name, chothes.id)
        end
        for i, item in pairs(self.config.inventory) do
            if item.isEquip then
                self:equipArmor(item.id, item.meta)
            else
                self:addItem(item.id, item.num, item.meta)
            end
        end
        self:setOccupation()
    end
end

function GamePlayer:setOccupation()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setOccupation(self.occupationId)
    end
end

function GamePlayer:setAddDefense(addDefense)
    self.addDefense = addDefense
end

function GamePlayer:onKill()
    self.kills = self.kills + 1
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    self.appIntegral = self.appIntegral + 1
end

function GamePlayer:onWin()
    ReportManager:reportUserWin(self.userId)
    self.appIntegral = self.appIntegral + 10
end

function GamePlayer:onGetFlagAfterHint()
    if self.flag == nil then
        return
    end
    MsgSender.sendBottomTipsToTarget(self.rakssid, 3, Messages:msgGetFlagAfterHint())
end

function GamePlayer:onGetFlag(flag)
    self.flag = flag
    if self.config and self.config.isGetFlagBuff then
        self:addEffect(flag.buff.id, flag.buff.time, flag.buff.lv)
    end
    self:changeClothes(flag.flagPart.name, flag.flagPart.id)
    MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgGetFlagTarget())
end

function GamePlayer:onHandedInFlag()
    self.score = self.score + ScoreConfig.FLAG_SCORE
    self.scorePoints[ScoreID.FLAG] = self.scorePoints[ScoreID.FLAG] + 1
    self.appIntegral = self.appIntegral + 3
    if self.team then
        self.team:addFlag()
    end
    if self.flag ~= nil then
        self.flag:onPlayerHandedInFlag()
        self:changeClothes(self.flag.flagPart.name, "0")
    end
    if self.config and self.config.isGetFlagBuff then
        self:removeEffect(self.flag.buff.id)
    end
    self.flag = nil
    MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgHandedInFlagTarget())
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    if self.config and self.config.displayName then
        table.insert(nameList, self.config.displayName)
    end

    -- pureName line
    local disName = self.name
    if self.team ~= nil then
        disName = disName .. self.team:getDisplayName()
    end

    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:hasFashion(fashion)
    local result = self.fashions[fashion.id]
    if result then
        return result >= 2
    end
    return false
end

function GamePlayer:useFashion(fashion)
    for id, status in pairs(self.fashions) do
        local item = FashionStoreConfig:getFashion(id)
        if item ~= nil and item.type == fashion.type and status == 3 then
            self.fashions[id] = 2
            if item.effect.id ~= 0 then
                self:removeEffect(item.effect.id)
            end
        end
    end
    self.fashions[fashion.id] = 3
    if fashion.effect.id ~= 0 then
        self:addEffect(fashion.effect.id, fashion.effect.time, fashion.effect.lv)
    end
    self:changeClothes(fashion.actor.name, fashion.actor.id)
end

function GamePlayer:buyFashion(fashion)
    if self.money >= fashion.price then
        self:subCurrency(fashion.price)
        self:useFashion(fashion)
        return true
    end
    return false
end

function GamePlayer:sendFashionData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = {}
    data.title = "flag.war.shop.tab.fashion"
    data.props = {}
    local fashions = FashionStoreConfig:getFashions()
    for id, fashion in pairs(fashions) do
        local item = {}
        item.id = fashion.id
        item.name = fashion.name
        item.image = fashion.image
        item.price = fashion.price
        item.desc = fashion.desc
        item.status = self.fashions[item.id] or 1
        table.insert(data.props, item)
    end
    self.entityPlayerMP:changeSwitchableProps(json.encode(data))
end

function GamePlayer:sendSkillData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = {}
    data.title = "flag.war.shop.tab.skill"
    data.props = {}
    local skills = SkillConfig:getSkillsByOccupationId(self.occupationId)
    for i, skill in pairs(skills) do
        local item = {}
        item.id = skill.id
        item.name = skill.name
        item.image = skill.image
        item.desc = skill.desc
        item.level = -1
        item.price = 0
        item.value = 0
        item.nextValue = 0
        table.insert(data.props, item)
    end
    self.entityPlayerMP:changeUpgradeProps(json.encode(data))
end

function GamePlayer:onQuit()
    ReportManager:reportUserData(self.userId, self.kills)
end

return GamePlayer

--endregion


