require "Match"
require "data.GameTeam"
require "config.TeamConfig"
require "config.RankNpcConfig"
require "config.ScoreConfig"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr()

    self.money = 0
    self.victoryCount = 0
    self.knightCount = 0

    self.isInMineArea = false
    self.isExchanged = false
    self.jewelId = 0
    self.rank = 0
    self.score = 0
    self.kills = 0
    self.lastKillTime = 0

    self.tempInventory = {}
    self.tools = {}
    self.toolLevel = 0
    self.hasSendTeamInfo = false
    self.isSingleReward = false

    self:teleInitPosWithYaw()
    self:initScorePoint()
    self:setCurrency(self.money)
    self:setShowName(self:buildShowName())
    self:initRankData()
    self:addItem(MerchantConfig.tools["0"].itemId, 1, 0)

    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.SERIAL_KILL] = 0
end

function GamePlayer:teleInitPosWithYaw()
    self:teleportPosWithYaw(GameConfig.initPos, GameConfig.yaw)
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:initTeam(index)
    local team = GameMatch.teams[index]
    if team == nil then
        local config = TeamConfig:getTeam(index)
        team = GameTeam.new(config, TeamConfig:getTeamColor(config.id))
        GameMatch.teams[config.id] = team
    end
    team:addPlayer()
    self.team = team
    HostApi.changePlayerTeam(0, self.userId, self.team.id)
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    -- pureName line
    local disName = self.name
    if self.team ~= nil then
        disName = disName .. self.team:getDisplayName()
    end
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:addMoney(money)
    if money ~= 0 then
        self:addCurrency(money)
    end
end

function GamePlayer:subMoney(money)
    self:subCurrency(money)
end

function GamePlayer:upgradeTools()
    local nextTool = MerchantConfig:getToolsByLevel(self.toolLevel + 1)
    if nextTool == nil then
        return false
    end
    if self.money < nextTool.price then
        return false
    end
    self:subMoney(nextTool.price)
    local tool = MerchantConfig:getToolsByLevel(self.toolLevel)
    if tool ~= nil then
        self:removeItem(tool.itemId, 1)
    end
    self.toolLevel = self.toolLevel + 1
    self:addItem(nextTool.itemId, 1, 0)
    return true
end

function GamePlayer:sendToolsData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = {}
    data.props = {}
    local item = self:getToolsItemData()
    if item then
        data.title = item.title
        table.insert(data.props, item)
    end
    self.entityPlayerMP:changeUpgradeProps(json.encode(data))
end

function GamePlayer:getToolsItemData()
    local tool = MerchantConfig:getToolsByLevel(self.toolLevel)
    if tool == nil then
        return nil
    end
    local item = {}
    item.id = tool.id
    item.level = tool.level
    item.name = tool.name
    item.image = tool.image
    item.desc = tool.desc
    item.title = tool.title
    item.price = -1
    item.value = 0
    item.nextValue = 0
    local nextTool = MerchantConfig:getToolsByLevel(self.toolLevel + 1)
    if nextTool ~= nil then
        item.nextValue = 0
        item.price = nextTool.price
    end
    return item
end

function GamePlayer:showRespawn()
    RespawnHelper.sendRespawnCountdown(self, GameConfig.respawnTime)
end

function GamePlayer:initRankData()
    self.RankData = {}
    self.RankData[RankNpcConfig.RANK_VICTORY] = {}
    self.RankData[RankNpcConfig.RANK_VICTORY].week = {}
    self.RankData[RankNpcConfig.RANK_VICTORY].day = {}
    self.RankData[RankNpcConfig.RANK_KNIGHT] = {}
    self.RankData[RankNpcConfig.RANK_KNIGHT].week = {}
    self.RankData[RankNpcConfig.RANK_KNIGHT].day = {}
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

function GamePlayer:saveRankData()
    RankNpcConfig:savePlayerRank(self)
end

function GamePlayer:onKill()
    self.kills = self.kills + 1
    self.score = self.score + ScoreConfig.KILL_SCORE
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    if self.multiKill == 0 then
        self.multiKill = 1
    else
        if os.time() - self.lastKillTime <= 30 then
            self.multiKill = self.multiKill + 1
            self.scorePoints[ScoreID.SERIAL_KILL] = self.scorePoints[ScoreID.SERIAL_KILL] + 1
        else
            self.multiKill = 1
        end
    end

    self.lastKillTime = os.time()

    if self.multiKill >= 5 then
        self.score = self.score + ScoreConfig.KILL_5_SCORE
        MsgSender.sendMsg(IMessages:msgFiveKill(self:getDisplayName()))
    elseif self.multiKill >= 4 then
        self.score = self.score + ScoreConfig.KILL_4_SCORE
        MsgSender.sendMsg(IMessages:msgThirdKill(self:getDisplayName()))
    elseif self.multiKill >= 3 then
        self.score = self.score + ScoreConfig.KILL_4_SCORE
        MsgSender.sendMsg(IMessages:msgThirdKill(self:getDisplayName()))
    elseif self.multiKill >= 2 then
        self.score = self.score + ScoreConfig.KILL_2_SCORE
        MsgSender.sendMsg(IMessages:msgDoubleKill(self:getDisplayName()))
    end

end

function GamePlayer:addScore(score)
    self.score = self.score + score
end

function GamePlayer:onWin()
    ReportManager:reportUserWin(self.userId)
    self.score = self.score + ScoreConfig.WIN_SCORE
    self.victoryCount = 1
    self.appIntegral = self.appIntegral + 1
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
    HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), false)
end

function GamePlayer:overGame()
    if self.team.isWin then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOverWin(), GameOverCode.GameOver)
    else
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
    end
end

function GamePlayer:onQuit()
    if self.jewelId ~= 0 then
        JewelConfig:GenerateById(self.jewelId)
    end

    local isCount = 0
    if RewardManager:isUserRewardFinish(self.userId) then
        isCount = 1
    end
    ReportManager:reportUserData(self.userId, self.kills, self.rank, isCount)
end

function GamePlayer:onGameEnd(win)
    self:reward(win, GameMatch:getPlayerRank(self), true)
end

function GamePlayer:reward(isWin, rank, isEnd)
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
    RewardManager:getUserReward(self.userId, rank, function(data)
        if GameMatch:isGameOver() == false then
            self:sendPlayerSettlement()
        end
    end)

    self.isSingleReward = true
end

return GamePlayer

--endregion


