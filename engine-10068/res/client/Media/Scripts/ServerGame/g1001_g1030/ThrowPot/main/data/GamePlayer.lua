---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "config.TitleConfig"
require "config.SkillConfig"
require "config.ChipConfig"
require "util.RealRankUtil"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr()

    self.config = {}
    self.score = 0
    self.money = 0
    self.games = 0
    self.integral = 0
    self.hasWin = false
    self.titles = {}
    self.skills = {}
    self.chips = {}

    self:reset()
    self:setOccupation()
    self:setCurrency(self.money)

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

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        self.money = data.money or 0
        self.games = data.games or 0
        self.integral = data.integral or 0
        self.hasWin = data.hasWin or false
        self.titles = data.titles or {}
        self.skills = data.skills or {}
        self.chips = data.chips or {}
    end
    self:setCurrency(self.money)
    self:sendTitleData()
    self:updateInv()
    self:setShowName(self:buildShowName())
    HostApi.sendPlaySound(self.rakssid, 10031)
end

function GamePlayer:updateInv()
    self:clearInv()
    if GameMatch:isGameRunning() == false then
        for id, num in pairs(self.chips) do
            local chip = ChipConfig:getChip(id)
            if chip ~= nil then
                self:addItem(chip.itemId, num, 0)
            end
        end
    end
    for id, has in pairs(self.skills) do
        if has then
            local skill = SkillConfig:getSkill(id)
            if skill ~= nil then
                self:addItem(skill.itemId, 1, 0)
            end
        end
    end
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.LIVE] = 0
end

function GamePlayer:reset()
    self.score = 0
    self.isLife = false
    self.isHaveTnt = false
    self.rank = 0
    self.isReport = false
    self.flyingTime = 0
    self.invincible = false
    self.invincibleTime = 0
    self:setAllowFlying(false)
    self:initScorePoint()
    self:updateInv()
    self:setArmItem(0)
    self:resetClothes()
    self.isSingleReward = false
    if GameMatch:isGameRunning() then
        self:teleGamePos()
    else
        self:teleInitPos()
    end
    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:setOccupation()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setOccupation(1)
    end
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:teleInitPos()
    self.isLife = false
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:teleGamePos()
    self.isLife = true
    self:teleportPos(GameConfig.gamePos)
    self:updateInv()
    self:resetItemIndex()
end

function GamePlayer:teleRoundPos(pos)
    self:teleportPos(pos)
end

function GamePlayer:resetItemIndex()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeCurrentItem(8)
    end
end

function GamePlayer:becomeTntPlayer(name)
    self.isHaveTnt = true
    self:setSpeedAddition(GameConfig.speedLevel)
    for _, chothes in pairs(GameConfig.tntActors) do
        self:changeClothes(chothes.first, chothes.second)
    end
    if name then
        MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgBecomeTNTPlayer(name))
    else
        MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgGameStartHasTNT())
    end
    self:sendGameData()
    self:setArmItem(46)
    HostApi.sendPlaySound(self.rakssid, 304)
end

function GamePlayer:becomeNormalPlayer(name)
    self.isHaveTnt = false
    self:setSpeedAddition(0)
    self:resetClothes()
    self:removeEffect(PotionID.INVISIBILITY)
    if name then
        HostApi.sendPlaySound(self.rakssid, 309)
        MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgBecomeNormalPlayer(name))
    else
        MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgGameStartNoTNT())
    end
    self:setArmItem(0)
    self:sendGameData()
end

function GamePlayer:sendGameData()
    local lifes = GameMatch:getLifePlayer()
    if self.isHaveTnt then
        MsgSender.sendTopTipsToTarget(self.rakssid, 200, Messages:msgGameData(GameMatch.RoundCount, 1, lifes))
    else
        MsgSender.sendTopTipsToTarget(self.rakssid, 200, Messages:msgGameData(GameMatch.RoundCount, 0, lifes))
    end
end

function GamePlayer:boom()
    if self.isLife then
        self.isLife = false
        self.isHaveTnt = false
        self:setHealth(0)
        self:reward()
        MsgSender.sendTopTipsToTarget(self.rakssid, 1, " ")
        MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgBoomToTarget())
        MsgSender.sendMsg(Messages:msgBoomToAll(self:getDisplayName()))
        HostApi.sendPlaySound(self.rakssid, 310)
    end
end

function GamePlayer:onTick()
    if self.isLife == false then
        return
    end
    local time = os.time()
    if time - self.flyingTime == 0 then
        self:setAllowFlying(false)
    end
    if time - self.invincibleTime == 0 then
        self:setInvincible(false)
    end
end

function GamePlayer:setFlying()
    self.flyingTime = os.time() + 10
    self:setAllowFlying(true)
end

function GamePlayer:setInvincible(invincible)
    self.invincible = invincible
    if self.invincible then
        self.invincibleTime = os.time() + 3
    end
end

function GamePlayer:showMask()
    GamePacketSender:sendShowDarkMask(self.rakssid)
end

function GamePlayer:isInvincible()
    return self.invincible
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    for id, status in pairs(self.titles) do
        if status == 3 then
            local title = TitleConfig:getTitle(id)
            if title ~= nil then
                table.insert(nameList, (title.color .. title.title))
                break
            end
        end
    end

    PlayerIdentityCache:buildNameList(self.userId, nameList, self.name)
    return table.concat(nameList, "\n")
end

function GamePlayer:addChip(chip, num)
    if self.chips[chip.id] == nil then
        self.chips[chip.id] = 0
    end
    self.chips[chip.id] = self.chips[chip.id] + (num or 1)
    self:addItem(chip.itemId, num or 1, 0)
    MsgSender.sendMsgToTarget(self.rakssid, Messages:msgGetChipHint(chip.tip))
    HostApi.notifyGetItem(self.rakssid, chip.itemId, 0, num or 1)
end

function GamePlayer:subChip(chip, num)
    if self.chips[chip.id] == nil then
        return false
    end
    if self.chips[chip.id] >= num then
        self.chips[chip.id] = self.chips[chip.id] - num
        return true
    end
    return false
end

function GamePlayer:hasSkill(skill)
    return self.skills[skill.id] or false
end

function GamePlayer:addSkill(skill)
    self.skills[skill.id] = true
    local needSendData = true
    local skills = SkillConfig:getSkills()
    for id, skill in pairs(skills) do
        if self.skills[skill.id] == nil then
            needSendData = false
        end
    end
    if needSendData then
        self:sendTitleData()
    end
end

function GamePlayer:buyTitle(title)
    if TitleConfig:canBuyTitle(title.id, self) then
        self:useTitle(title)
        return true
    end
    return false
end

function GamePlayer:hasTitle(title)
    local result = self.titles[title.id]
    if result then
        return result >= 2
    end
    return false
end

function GamePlayer:useTitle(title)
    for id, status in pairs(self.titles) do
        if status == 3 then
            self.titles[id] = 2
        end
    end
    self.titles[title.id] = 3
    self:setShowName(self:buildShowName())
    HostApi.sendPlaySound(self.rakssid, 301)
end

function GamePlayer:sendTitleData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = {}
    data.title = "throw.pot.shop.tab.title"
    data.props = {}
    local titles = TitleConfig:getTitles()
    for id, title in pairs(titles) do
        local item = {}
        item.id = title.id
        item.name = title.name
        item.image = title.image
        item.price = 0
        item.desc = title.desc
        if self.titles[title.id] ~= nil then
            item.status = self.titles[title.id]
        else
            if TitleConfig:canBuyTitle(title.id, self) then
                item.status = 1
            else
                item.status = 0
            end
        end
        table.insert(data.props, item)
    end
    self.entityPlayerMP:changeSwitchableProps(json.encode(data))
end

function GamePlayer:sendPlayerSettlement()
    local settlement = {}
    settlement.rank = self.rank
    settlement.name = self.name
    settlement.isWin = 0
    settlement.gold = self.gold
    settlement.points = self.scorePoints
    settlement.available = self.available
    settlement.hasGet = self.hasGet
    settlement.vip = self.vip
    settlement.kills = 0
    settlement.adSwitch = self.adSwitch or 0
    if settlement.gold <= 0 then
        settlement.adSwitch = 0
    end
    HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), true)
end

function GamePlayer:onGameEnd()
    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end
    local needSendData = false
    self.scorePoints[ScoreID.LIVE] = os.time() - RewardManager:getStartGameTime()
    self.score = math.max(RewardManager:getStartPlayerCount() - self.rank + 1, 1)
    self.games = self.games + 1
    self.appIntegral = self.appIntegral + self.score
    if self.games == 3 or self.game == 50 or self.game == 100 then
        needSendData = true
    end
    if self.integral < 1000 and self.integral + self.score >= 1000 then
        needSendData = true
    end
    self.integral = self.integral + self.score
    if self.rank == 1 then
        if self.hasWin == false then
            needSendData = true
        end
        self.hasWin = true
    end
    self:addCurrency(self.score)
    if needSendData then
        self:sendTitleData()
    end
    MsgSender.sendMsgToTarget(self.rakssid, Messages:msgRewardHint(self.score, self.score))
    RealRankUtil:savePlayerRankScore(self)
    RankNpcConfig:savePlayerRankScore(self)
end

function GamePlayer:reward()
    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end

    UserExpManager:addUserExp(self.userId, false)

    self.rank = GameMatch:getLifePlayer() + 1
    self:onGameEnd()
    if self.rank == 2 then
        return
    end
    RewardManager:getUserReward(self.userId, self.rank, function(data)
        if GameMatch.hasEndGame == false then
            self:sendPlayerSettlement()
        end
    end)
    ReportManager:reportUserData(self.userId, 0, self.rank, 1)
    self.isReport = true
    self.isSingleReward = true
end

return GamePlayer

--endregion