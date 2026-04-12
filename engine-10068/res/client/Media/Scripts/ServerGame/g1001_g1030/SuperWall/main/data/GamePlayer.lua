---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "config.ScoreConfig"
require "config.TeamConfig"
require "config.RespawnConfig"
require "config.GameConfig"
require "config.OccupationConfig"
require "config.CannotDropItemConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr()

    self.money = 0
    self.score = 0
    self.resourceScore = 0
    self.kills = 0
    self.dies = 0
    self.rank = 0
    self.isLife = true
    self.addScore = 0
    self.team = nil
    self.toolRecord = {}
    self.inventory = {}
    self.config = nil
    self.occupationId = 0
    self.occupations = OccupationConfig:initNormalOccupation()
    self.isSingleReward = false

    self.type = GameConfig.defaultAttr.type
    self.speed = GameConfig.defaultAttr.speed
    self.attack = GameConfig.defaultAttr.attack
    self.defense = GameConfig.defaultAttr.defense
    self.defenseX = GameConfig.defaultAttr.defenseX

    self:initTeam()
    self:initScorePoint()
    self:setCurrency(self.money)

    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
end

function GamePlayer:teleTeamPosAndYaw()
    if self.team ~= nil then
        self:teleportPosWithYaw(self.team.PreparePos, self.team.Yaw)
    end
    self:changeMaxHealth(GameConfig.defaultAttr.health)
    self:changeOtherAttrs()
    self:initBasicProp()
    self:sendOccupationData(0)
end

function GamePlayer:teleRebirthPosAndYaw()
    if self.team ~= nil then
        if self.team:isDeath() then
            self:subHealth(99999)
            return
        else
            self:teleportPosWithYaw(self.team.RebirthPos, self.team.Yaw)
        end
    end
    self.isLife = true
    self:setCurrency(self.money)
    self:initBasicProp()
    if self.config == nil then
        self:changeMaxHealth(GameConfig.defaultAttr.health)
    else
        self:changeMaxHealth(self.config.health)
        for _, item in pairs(self.config.inventory) do
            if item.isReget then
                if item.isEquip then
                    self:equipArmor(item.id, item.meta)
                else
                    self:addItem(item.id, item.num, item.meta)
                end
            end
        end
        self:fillInvetory(self.inventory)
        self:setSpeedAddition(self.speed)
        self:resetClothes()
        for _, chothes in pairs(self.config.actors) do
            self:changeClothes(chothes.name, chothes.id)
        end
        self:setOccupation()
    end
end

function GamePlayer:fillInvetory(inventory)
    self:clearInv()
    if inventory == nil then
        return
    end
    local armors = inventory.armor
    for i, armor in pairs(armors) do
        if armor.id > 0 and armor.id < 10000 and CannotDropItemConfig:isCannotDrop(armor.id) then
            self:equipArmor(armor.id, armor.meta)
        end
    end
    local guns = inventory.gun
    for i, gun in pairs(guns) do
        if CannotDropItemConfig:isCannotDrop(gun.id) then
            self:addGunItem(gun.id, gun.num, gun.meta, gun.bullets)
        end
    end
    local items = inventory.item
    for i, item in pairs(items) do
        if item.id > 0 and item.id < 10000 and CannotDropItemConfig:isCannotDrop(item.id) then
            self:addItem(item.id, item.num, item.meta)
        end
    end
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:addMoney(money)
    if money ~= 0 then
        self:addCurrency(money)
    end
end

function GamePlayer:subMoney(money)
    self:subCurrency(money)
end

function GamePlayer:initBasicProp()
    for _, item in pairs(BasicPropConfig.items) do
        self:addItem(item.id, item.num, item.meta)
    end
end

function GamePlayer:initTeam()
    local team = GameMatch.Teams[self.dynamicAttr.team]
    if team == nil then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.NoThisTeam)
        return
    end
    self.team = team
    team:addPlayer()
    self:setShowName(self:buildShowName())
    self:setRespawnPos(self.team.PreparePos)
end

function GamePlayer:addHP()
    if self.config == nil then
        return
    end
    self:addHealth(self.config.regeneratesHP)
end

function GamePlayer:tryChangeOccupation(occupationId)
    local lv = 0
    for id, level in pairs(self.occupations) do
        if id == occupationId then
            lv = level
        end
    end
    local occupation = OccupationConfig:getOccupation(occupationId, lv)
    if occupation == nil then
        return false
    end
    local isUpgrade = true
    local price = occupation.price
    if self.occupationId ~= tonumber(occupationId) then
        if lv ~= 0 then
            price = occupation.togglePrice
            isUpgrade = false
        end
    end
    if isUpgrade and price == 0 then
        return false
    end
    if self.money < price then
        return false
    end
    self:subMoney(price)
    if isUpgrade then
        lv = lv + 1
        self.occupations[occupationId] = lv
        occupation = OccupationConfig:getOccupation(occupationId, lv)
    end
    if occupation ~= nil then
        self:changeOccupation(occupation)
    end
    return true
end

function GamePlayer:changeOccupation(occupation)
    local inventory
    if self.occupationId ~= 0 then
        local curOcc = OccupationConfig:getOccupation(tostring(self.occupationId),
        self.occupations[tostring(self.occupationId)])
        if curOcc ~= nil then
            inventory = curOcc.inventory
        end
    end
    local isChange = (self.occupationId ~= tonumber(occupation.id))
    self.config = occupation
    self.occupationId = tonumber(occupation.id)
    self.speed = occupation.speed
    self.attack = occupation.attack
    self.defense = occupation.defense
    self.defenseX = occupation.defenseX
    self.type = occupation.type
    self:changeOtherAttrs()
    self:setSpeedAddition(self.speed)
    self:changeMaxHealth(occupation.health)
    self:setShowName(self:buildShowName())
    if isChange then
        self:resetClothes()
        for _, chothes in pairs(occupation.actors) do
            self:changeClothes(chothes.name, chothes.id)
        end
        if inventory ~= nil then
            for _, item in pairs(inventory) do
                self:removeItem(item.id, item.num)
            end
        end
        for _, item in pairs(occupation.inventory) do
            if item.isEquip then
                self:equipArmor(item.id, item.meta)
            else
                self:addItem(item.id, item.num, item.meta)
            end
        end
        self:setOccupation()
    end
end

function GamePlayer:changeOtherAttrs()
    if self.entityPlayerMP == nil then
        return
    end
    self.entityPlayerMP:changeDefense(self.defense, self.defense)
    self.entityPlayerMP:changeAttack(self.attack, self.attack)
end

function GamePlayer:setOccupation()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setOccupation(self.occupationId)
    end
end

function GamePlayer:sendOccupationData(occupationId)
    if self.entityPlayerMP == nil then
        return
    end
    local data = {}
    data.title = "super.wall.occ.npc.name"
    data.props = {}
    for id, level in pairs(self.occupations) do
        local occupation = OccupationConfig:getOccupation(id, level)
        if occupation ~= nil then
            local item = {}
            item.id = occupation.id
            item.name = occupation.name
            item.image = occupation.image
            item.content = occupation.content
            if level == 0 then
                item.price = occupation.price
                item.status = 1
            else
                item.price = occupation.togglePrice
                item.status = 2
            end
            if id == occupationId then
                item.price = occupation.price
                if item.price ~= 0 then
                    item.status = 3
                else
                    item.status = 7
                end
            end
            table.insert(data.props, item)
        end
    end
    self.entityPlayerMP:changeSuperProps(json.encode(data))
end

function GamePlayer:getTeamId()
    if self.team ~= nil then
        return self.team.ID
    end
    return 0
end

function GamePlayer:getTeamDistance()
    if self.team ~= nil then
        local pos = self:getPosition()
        local teamPos = self.team.PreparePos
        local sqr = math.pow(pos.x - teamPos.x, 2) + math.pow(pos.y - teamPos.y, 2) + math.pow(pos.z - teamPos.z, 2)
        return math.sqrt(sqr)
    end
    return 256
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

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    if self.config ~= nil then
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

function GamePlayer:addPlayerScore(score)
    self.score = self.score + score
    self.addScore = self.addScore + score
    self.appIntegral = self.appIntegral + score
end

function GamePlayer:onAttacked(damage)
    damage = math.max(0, damage)
    if damage ~= 0 then
        self:subHealth(damage)
    end
end

function GamePlayer:onBreakBlock(damage)
    local heldItemId = self:getHeldItemId()
    if heldItemId == 0 then
        return
    end
    local durability = ToolAttrConfig:getDurability(heldItemId)
    if durability == -1 then
        return
    end
    local record = self.toolRecord[tostring(heldItemId)]
    if record == nil then
        record = 0
    end
    record = record + damage
    if record >= durability then
        record = record - durability
        self:removeItem(heldItemId, 1)
    end
    self.toolRecord[tostring(heldItemId)] = record
end

function GamePlayer:onWin()
    ReportManager:reportUserWin(self.userId)
    self:addPlayerScore(ScoreConfig.WIN_SCORE)
end

function GamePlayer:onKill(dier)
    self.kills = self.kills + 1
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    self:addPlayerScore(ScoreConfig.KILL_SCORE)
    if self.isLife == false then
        return
    end
    if dier.config ~= nil then
        self:addItem(dier.config.itemIdOfKill, dier.config.itemNumOfKill)
        self:addMoney(dier.config.goldOfKill)
    else
        self:addMoney(10)
    end
end

function GamePlayer:onDie()
    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end
    self.isLife = false
    self.dies = self.dies + 1
    self.inventory = InventoryUtil:getPlayerInventoryInfo(self)
    self:setSpeedAddition(0)
    if self.config ~= nil then
        self:addMoney(self.config.goldOfCompensation)
    else
        self:addMoney(10)
    end
    if self.team == nil then
        return
    end
    if self.team:isDeath() then
        self:onRealDeath()
    else
        RespawnHelper.sendRespawnCountdown(self, RespawnConfig:getRespawnCd(GameMatch:getGameRunningTime()))
    end
end

function GamePlayer:sendPlayerSettlement()
    local settlement = {}
    settlement.rank = self.rank
    settlement.name = self.name
    settlement.isWin = 0
    settlement.points = self.scorePoints
    settlement.gold = self.gold
    settlement.available = self.available
    settlement.hasGet = self.hasGet
    settlement.vip = self.vip
    settlement.kills = self.kills
    settlement.adSwitch = self.adSwitch or 0
    if settlement.gold <= 0 then
        settlement.adSwitch = 0
    end
    HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), true)
end

function GamePlayer:overGame()
    if self.team.isWin then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOverWin(), GameOverCode.GameOver)
    else
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
    end
end

function GamePlayer:onRealDeath()
    HostApi.broadCastPlayerLifeStatus(self.userId, false)
    local isEnd = (GameMatch:getLifeTeams() == 1)
    self:reward(false, isEnd)
end

function GamePlayer:onLogout()
    if self.team ~= nil then
        self.team:onPlayerQuit()
    end
end

function GamePlayer:onQuit()
    local isCount = 0
    if RewardManager:isUserRewardFinish(self.userId) then
        isCount = 1
    end
    ReportManager:reportUserData(self.userId, self.kills, self.rank, isCount)
end

function GamePlayer:onGameEnd()
    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end
    self:addPlayerScore(math.ceil(self.resourceScore))
    self.rank = RewardUtil:getPlayerRank(self)
    RankNpcConfig:savePlayerRankScore(self)
end

function GamePlayer:reward(isWin, isEnd)
    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end

    UserExpManager:addUserExp(self.userId, isWin, 4)

    if isWin then
        HostApi.sendPlaySound(self.rakssid, 10023)
    else
        HostApi.sendPlaySound(self.rakssid, 10024)
    end
    if isEnd then
        return
    end
    self:onGameEnd()
    RewardManager:getUserReward(self.userId, self.rank, function(data)
        if GameMatch:isGameOver() == false then
            self:sendPlayerSettlement()
        end
    end)

    self.isSingleReward = true
end

return GamePlayer

--endregion


