---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "config.TitleConfig"
require "config.TalentConfig"
require "config.SegmentConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
    self:initStaticAttr()

    self.score = 0
    self.wins = 0
    self.no1 = 0
    self.kills = 0

    self.money = 0
    self.games = 0
    self.wingames = 0
    self.integral = 0
    self.pkscore = 0
    self.titles = {}
    self.talents = {}

    self.isLife = true
    self.status = 0
    self.attack = 0
    self.defense = 0
    self.addHp = 0

    self:reset()
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
        self.wingames = data.wingames or 0
        self.integral = data.integral or 0
        self.pkscore = data.pkscore or 0
        self.titles = data.titles or {}
        self.talents = data.talents or {}
    end
    self:setCurrency(self.money)
    self:sendTitleData()
    self:sendTalentData()
    self:setShowName(self:buildShowName())
    self:resetTalentAttr()
end

function GamePlayer:reset()
    self.isSingleReward = false
    self.isReport = false
    self.score = 0
    self.kills = 0
    if GameMatch:isGameStarted() then
        self.isLife = false
        self.rank = 16
    else
        self.isLife = true
        self.rank = 0
    end
    self:teleInitPos()
    self:initScorePoint()
    self:clearInv()
    self:resetHealth()
    self:setFoodLevel(20)
    self:clearEffects()
    if self.status == 3 then
        self.status = 0
    end
    if self.status ~= 0 then
        self.status = 0
        self:setShowName(self:buildShowName())
    end
    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:resetHealth()
    self:changeMaxHealth(35 + self.addHp)
    self:setHealth(35 + self.addHp)
end

function GamePlayer:resetTalentAttr()
    for type, level in pairs(self.talents) do
        local talent = TalentConfig:getTalentByParams(type, level)
        if talent ~= nil then
            if type == TalentConfig.ATTACK then
                self.attack = talent.value
            end
            if type == TalentConfig.DEFENSE then
                self.defense = talent.value
            end
            if type == TalentConfig.HEALTH then
                self.addHp = talent.value
                self:resetHealth()
            end
        end
    end
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.WIN] = 0
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:teleGamePos()
    if self.isLife then
        self:teleportPos(GameConfig.gamePos)
    end
end

function GamePlayer:teleSitePos(pos, yaw)
    self:teleportPosWithYaw(pos, yaw)
    self:setFoodLevel(20)
    self.status = 0
    self:setShowName(self:buildShowName())
end

function GamePlayer:teleWatchPos(pos)
    self:teleportPos(pos)
    self:setFoodLevel(20)
    self.status = 3
    self:setShowName(self:buildShowName())
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

    table.insert(nameList, self:getSegmentName())

    local status = ""
    if self.status == 1 then
        status = TextFormat.colorGold .. "[Winner]"
    elseif self.status == 2 then
        status = TextFormat.colorRed .. "[Loser]"
    end
    PlayerIdentityCache:buildNameList(self.userId, nameList, self.name .. status)
    return table.concat(nameList, "\n")
end

function GamePlayer:getSegmentName()
    local segment = SegmentConfig:getSegmentByScore(self.pkscore)
    return segment.color .. segment.title
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:onWin()
    self:resetHealth()
    self.scorePoints[ScoreID.WIN] = self.scorePoints[ScoreID.WIN] + 1
    self.wins = self.wins + 1
    self.kills = self.kills + 1
    self.pkscore = self.pkscore + 5
    self.status = 1
    self.appIntegral = self.appIntegral + 1
    self:setShowName(self:buildShowName())
    self:sendTalentData()
    MsgSender.sendMsgToTarget(self.rakssid, Messages:msgPkScoreTip("+5"))
    MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgWinTip())
end

function GamePlayer:onLost()
    self.isLife = false
    self.status = 2
    local needSendData = false
    if self.pkscore ~= 0 then
        needSendData = true
    end
    self.pkscore = math.max(self.pkscore - 4, 0)
    self:resetHealth()
    self:reward()
    self:setShowName(self:buildShowName())
    if needSendData then
        self:sendTalentData()
    end
    MsgSender.sendMsgToTarget(self.rakssid, Messages:msgPkScoreTip("-4"))
    MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgLoseTip())
end

function GamePlayer:buyTitle(title)
    if title.type == 2 then
        return false
    end
    if TitleConfig:canBuyTitle(title.id, self) then
        if self.money >= title.price then
            self:subCurrency(title.price)
            self:useTitle(title)
            return true
        end
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
    data.title = "budokai.title.tab"
    data.props = {}
    local titles = TitleConfig:getTitles()
    for id, title in pairs(titles) do
        local item = {}
        item.id = title.id
        item.name = title.name
        item.image = title.image
        item.price = title.price
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

function GamePlayer:upgradeTalent(type)
    if self.talents[type] == nil then
        self.talents[type] = 0
    end
    local talent = TalentConfig:getTalentByParams(type, self.talents[type])
    if talent == nil then
        return false
    end
    if talent.price == 0 or self.money < talent.price then
        return false
    end
    self:subCurrency(talent.price)
    self.talents[type] = self.talents[type] + 1
    self:resetTalentAttr()
    return true
end

function GamePlayer:sendTalentData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = {}
    data.title = "budokai.talent.tab"
    data.props = {}
    local attack = self:getTalentItemData(TalentConfig.ATTACK)
    local defense = self:getTalentItemData(TalentConfig.DEFENSE)
    local health = self:getTalentItemData(TalentConfig.HEALTH)
    local segment = self:getSegmentData()
    if attack then
        table.insert(data.props, attack)
    end
    if defense then
        table.insert(data.props, defense)
    end
    if health then
        table.insert(data.props, health)
    end
    if segment then
        table.insert(data.props, segment)
    end
    self.entityPlayerMP:changeUpgradeProps(json.encode(data))
end

function GamePlayer:getTalentItemData(type)
    local talent = TalentConfig:getTalentByParams(type, self.talents[type] or 0)
    if talent == nil then
        return nil
    end
    local item = {}
    item.id = talent.id
    item.level = talent.level
    item.name = talent.name
    item.image = talent.image
    item.desc = talent.desc
    item.price = talent.price
    item.value = talent.value
    item.nextValue = talent.nextValue
    return item
end

function GamePlayer:getSegmentData()
    local segment = SegmentConfig:getSegmentByScore(self.pkscore)
    if segment == nil then
        return nil
    end
    local item = {}
    item.id = "1001"
    item.level = segment.level
    item.name = segment.name
    item.image = segment.image
    item.desc = segment.desc
    item.price = -1
    item.value = self.pkscore
    item.nextValue = segment.pkscore.max + 1
    if item.nextValue > 2001 then
        item.nextValue = 2001
    end
    return item
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
    settlement.kills = self.kills
    settlement.adSwitch = self.adSwitch or 0
    if settlement.gold <= 0 then
        settlement.adSwitch = 0
    end
    HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), false)
end

function GamePlayer:onGameEnd()
    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end
    if self.rank == 1 then
        self.no1 = self.no1 + 1
        self.appIntegral = self.appIntegral + 8
    end
    local needSendData = false
    self.score = math.max(RewardManager:getStartPlayerCount() - self.rank + 1, 1)
    self.games = self.games + 1
    if self.games == 3 or self.game == 50 or self.game == 100 then
        needSendData = true
    end
    if self.integral < 1000 and self.integral + self.score >= 1000 then
        needSendData = true
    end
    self.integral = self.integral + self.score
    if self.rank == 1 then
        if self.wingames < 10 and self.wingames + 1 >= 10 then
            needSendData = true
        end
        if self.wingames == 0 then
            needSendData = true
        end
        self.wingames = self.wingames + 1
    end
    if needSendData then
        self:sendTitleData()
    end
    MsgSender.sendMsgToTarget(self.rakssid, Messages:msgGetScore(self.score))
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

function GamePlayer:onQuit()
    ReportManager:reportUserData(self.userId, self.kills)
end

return GamePlayer

--endregion


