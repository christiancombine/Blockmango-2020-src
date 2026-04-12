---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "Match"
require "data.GameTeam"
require "config.TeamConfig"
require "config.ScoreConfig"
require "util.RespawnManager"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
    self:initStaticAttr()

    self.money = 0
    self.score = 0
    self.kills = 0
    self.deathTimes = 0
    self.rank = 0
    self.realLife = true
    self.isLife = true
    self.lastKillTime = 0
    self.multiKill = 0
    self.winner = 0
    self.hasSendEnchantmentData = false
    self.isSingleReward = false

    --self.tempInventory = {}
    self.enchantmentBuyItems = {}

    --ad
    self.respawnAdTimes = 0     --复活广告的次数

    self:initTeam()
    self:initScorePoint()
    self:setCurrency(self.money)
    self:setShowName(self:buildShowName())
    self:teleportPosWithYaw(self.team:getTeleportPos(), 0)

    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.SERIAL_KILL] = 0
end

function GamePlayer:initTeam()
    local team = GameMatch.teams[self.dynamicAttr.team]
    if team == nil then
        print(type(self.dynamicAttr.team) .. "        value ==   " .. self.dynamicAttr.team)
        local config = TeamConfig:getTeam(self.dynamicAttr.team)
        assert(config)
        team = GameTeam.new(config, TeamConfig:getTeamColor(config.id))
        GameMatch.teams[config.id] = team
    end
    team:addPlayer()
    self.team = team
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
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

function GamePlayer:showRespawn()
    RespawnHelper.sendRespawnCountdown(self, GameConfig.respawnTime)
end

function GamePlayer:onKill()
    self.kills = self.kills + 1
    self.score = self.score + ScoreConfig.KILL_SCORE
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    self.appIntegral = self.appIntegral + 1
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
    self.appIntegral = self.appIntegral + 10
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

function GamePlayer:onLogout()
    if self.isLife then
        self.isLife = false
        self.team:subPlayer()
    end
end

function GamePlayer:onQuit()
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

function GamePlayer:showBuyRespawn()
    self.realLife = false
    local isRespawn = RespawnConfig:showBuyRespawn(self.rakssid, self.respawnTimes)
    if isRespawn == false then
        self:onDie()
        GameMatch:ifGameOverByPlayer()
    end
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self.realLife = false
        self:reward(false, self.defaultRank, GameMatch:getLifeTeams() == 1)
        self.team:subPlayer()
        HostApi.broadCastPlayerLifeStatus(self.userId, self.isLife)
    end
    RespawnManager:removeSchedule(self.userId)
end

function GamePlayer:sendEnchantmentData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = {}
    data.props = {}
    local items = self:getEnchantmentItemsData()
    for i, item in pairs(items) do
        if item ~= nil then
            data.title = item.title
            table.insert(data.props, item)
        end
    end

    self.entityPlayerMP:changeEnchantmentProps(json.encode(data))
end

function GamePlayer:getEnchantmentItemsData()
    local enchantmentItems = EnchantmentShop:getEnchantmentItems()
    if enchantmentItems == nil then
        return nil
    end
    local items = {}
    for i, enchantmentItem in pairs(enchantmentItems) do
        local item = {}
        item.id = enchantmentItem.id
        item.name = enchantmentItem.name
        item.desc = enchantmentItem.description
        item.image = enchantmentItem.image
        item.itemId = enchantmentItem.itemId
        item.title = enchantmentItem.title
        item.price = enchantmentItem.price
        item.isBuy = false
        for i, v in pairs(self.enchantmentBuyItems) do
            if v == item.id then
                item.isBuy = true
            end
        end
        items[i] = item
    end

    return items
end

function GamePlayer:addEnchantmentGoodsItem(enchantmentItem)
    local enchantments = {}
    local index = 1
    for i, level in pairs(enchantmentItem.enchantLevel) do
        if tonumber(level) ~= 0 then
            for j, enchantId in pairs(enchantmentItem.enchantId) do
                if i == j then
                    local enchantment = {}
                    enchantment[1] = enchantId
                    enchantment[2] = level
                    enchantments[index] = enchantment
                    index = index + 1
                end
            end
        end
    end
    self:addEnchantmentsItem(enchantmentItem.itemId, enchantmentItem.num, enchantmentItem.meta, enchantments)
end

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        if data.money ~= nil then
            self:setCurrency(data.money)
        end
        if data.enchantmentItem ~= nil then
            for i, v in pairs(data.enchantmentItem) do
                local enchantmentItem = EnchantmentShop:findEnchantmentById(v)
                if enchantmentItem == nil then
                    return
                end
                self.enchantmentBuyItems[v] = v
                self:addEnchantmentGoodsItem(enchantmentItem)
            end
        end
    end
end

function GamePlayer:savePlayerData(immediate)
    local data = {}
    data.money = self:getCurrency()
    data.enchantmentItem = self.enchantmentBuyItems
    DBManager:savePlayerData(self.userId, 1, data, immediate)
end

function GamePlayer:getDBDataRequire()
    DBManager:getPlayerData(self.userId, 1)
end

function GamePlayer:addInitItems()
    for i, v in pairs(ItemConfig.playerInitItem) do
        self:addItem(v.itemId, v.num, v.meta)
    end
end

return GamePlayer

--endregion
