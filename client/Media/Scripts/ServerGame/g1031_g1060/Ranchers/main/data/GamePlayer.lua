require "Match"
require "data.GameManor"
require "data.GameHouse"
require "data.GameField"
require "data.GameWareHouse"
require "data.GameBuilding"
require "data.GameTasks"
require "data.GameAchievement"
require "data.GameAccelerateItems"
require "config.DataBaseManager"
require "config.RanchersRankManager"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
    self:initStaticAttr(0)
    self.money = 0
    self.score = 0
    self:setCurrency(self.money)
    self:initPlayerInfo()

end

function GamePlayer:initPlayerInfo()
    self.level = 1
    self.exp = 0
    self.giftNum = 0
    self.currentFieldNum = ManorConfig.initFieldNum
    self.fieldLevel = FieldLevelConfig:getFieldLevelByCurFieldNum(self.currentFieldNum)
    self.manorIndex = 0
    self.prosperity = 0  -- 繁荣度
    self.sendGiftCount = 0
    self.achievementPoint = 0
    self.targetManorIndex = 0
    self.taskStartLevel = GameConfig.taskStartLevel
    self.exploreStartLevel = GameConfig.exploreStartLevel
    self.isInServiceGate = false
    self.isInManorGate = false
    self.isMaster = false
    self.isCanFlying = false

    self.manor = nil
    self.house = nil
    self.field = nil
    self.wareHouse = nil
    self.building = nil
    self.task = nil
    self.achievement = nil
    self.accelerateItems = nil

    self.myProsperityRank = 0
    self.myAchievementRank = 0
    self.myClanRank = 0

    self.isCacheProsperityRank = false
    self.isCacheAchievementRank = false
    self.isCacheClanRank = false
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:addMoney(money)
    if money > 0 then
        self:addCurrency(money)
    end
end

function GamePlayer:subMoney(money)
    self:subCurrency(money)
end

function GamePlayer:prepareDataSaveToDB()
    local isCanFlying = 0
    if self.isCanFlying == true then
        isCanFlying = 1
    end

    local data = {
        level = self.level,
        exp = self.exp,
        giftNum = self.giftNum,
        achievementPoint = self.achievementPoint,
        money = self.money,
        prosperity = self.prosperity,
        currentFieldNum = self.currentFieldNum,
        inventory = InventoryUtil:getPlayerInventoryInfo(self),
        isCanFlying = isCanFlying,
    }
    --HostApi.log("GamePlayer:prepareDataSaveToDB: data = " .. json.encode(data))
    return data
end

function GamePlayer:initDataFromDB(data)
    self.level = data.level or 1
    self.exp = data.exp or 0
    self.giftNum = data.giftNum or 0
    self.achievementPoint = data.achievementPoint or 0
    self.prosperity = data.prosperity or 0
    self.currentFieldNum = data.currentFieldNum or ManorConfig.initFieldNum
    self:fillInvetory(data.inventory)
    self:addExp(0)
    self:setCurrency(tonumber(data.money or 0))
    self.fieldLevel = FieldLevelConfig:getFieldLevelByCurFieldNum(self.currentFieldNum)
    if data.isCanFlying ~= nil and data.isCanFlying == 1 then
        self:setAllowFlying(true)
        self.isCanFlying = true
    end

    local taskNum = TasksLevelConfig:getNumByLevel(self.level)
    if taskNum > 0 and self.task:getTaskOperationStatus() == false then
        self.task:startTaskOperation()
        self.task:initOriginalTasks(taskNum, self.level)
        local ranchTask = self.task:initRanchTask()
        self:getRanch():setOrders(ranchTask)
    end

    RanchersWeb:updateLandInfo(self.userId, self.exp, self.giftNum, self.level, self.building.prosperity)
end

function GamePlayer:setPlayerRanchInfo()

    local ranchInfo = self:initRanchInfo()
    self:getRanch():setInfo(ranchInfo)

    local ranchHouse = self.house:initRanchHouse()
    self:getRanch():setHouse(ranchHouse)

    local ranchField = self.field:initRanchPlant(self.fieldLevel, self.currentFieldNum)
    self:getRanch():setBuildByType(BuildingConfig.TYPE_PLANT, ranchField)

    local wareHouse = self.wareHouse:initRanchWareHouse()
    self:getRanch():setStorage(wareHouse)

    local farmBuilding = self.building:initRanchBuilding(BuildingConfig.TYPE_FARMING)
    self:getRanch():setBuildByType(BuildingConfig.TYPE_FARMING, farmBuilding)

    local factoryBuilding = self.building:initRanchBuilding(BuildingConfig.TYPE_FACTORY)
    self:getRanch():setBuildByType(BuildingConfig.TYPE_FACTORY, factoryBuilding)

    local ranchTimePrice = TimePaymentConfig:initRanchTimePrice()
    self:getRanch():setTimePrices(ranchTimePrice)

    local prosperityRank = self:initRanchProsperityRank()
    self:getRanch():setRankByType(RankListener.type_prosperity, prosperityRank)

    local achievementRank = self:initRanchAchievementRank()
    self:getRanch():setRankByType(RankListener.type_achievement, achievementRank)

    local clanRank = self:initRanchClanRank()
    self:getRanch():setRankByType(RankListener.type_clan, clanRank)
end

function GamePlayer:saveAchievementRankData(achievementPoint)
    local key = RanchersRankManager:getAchievementKey()
    local uid = tostring(self.userId)
    local score = achievementPoint
    HostApi.ZIncrBy(key, uid, score)
end

function GamePlayer:initRanchClanRank()
    local ranks = {}
    for i, v in pairs(RanchersRankManager.clan) do
        local data = RanchRank.new()
        data.id = v.clanId
        data.level = 0
        data.rank = v.rank
        data.value = v.prosperity
        data.name = v.name
        data.userNum = v.userCount .. "/" .. v.maxCount
        ranks[data.rank] = data
    end
    return ranks
end

function GamePlayer:initRanchProsperityRank()
    local ranks = {}
    for i, v in pairs(RanchersRankManager.prosperity) do
        local data = RanchRank.new()
        data.id = v.userId
        data.level = v.level
        data.rank = v.rank
        data.value = v.score
        data.name = v.name
        ranks[data.rank] = data
    end

    return ranks
end

function GamePlayer:initRanchAchievementRank()
    local ranks = {}
    for i, v in pairs(RanchersRankManager.achievement) do
        local data = RanchRank.new()
        data.id = v.userId
        data.level = v.level
        data.rank = v.rank
        data.value = v.score
        data.name = v.name
        ranks[data.rank] = data
    end

    return ranks
end

function GamePlayer:addExp(exp)
    if exp < 0 then
        return false
    end
    self.appIntegral = self.appIntegral + exp
    local needExp = PlayerLevelConfig:getExpByLevel(self.level)
    local maxLevel = PlayerLevelConfig:getMaxLevel()
    if self.level < maxLevel then
        if self.exp + exp >= needExp then
            self:addMoney(PlayerLevelConfig:getRewardMoneyByLevel(self.level))
            local items = PlayerLevelConfig:getRewardItemsByLevel(self.level)
            if items ~= nil then
                if self.wareHouse:isEnoughCapacity(items) then
                    self.wareHouse:addItems(items)
                    local wareHouse = self.wareHouse:initRanchWareHouse()
                    self:getRanch():setStorage(wareHouse)
                    HostApi.sendRanchGain(self.rakssid, items)
                else
                    -- 发送邮件发放奖励
                    local l_items = {}
                    for i, v in pairs(items) do
                        local item = {}
                        item.itemId = v.itemId
                        item.num = v.num
                        table.insert(l_items, item)
                    end
                    local title = "xxx"
                    local data = {}
                    data.items = l_items
                    data.msg = "xxx"

                    local lang = MailLanguageConfig:getLangByType(MailType.LevelUp)
                    if lang ~= nil then
                        title = lang.title
                        data.msg = lang.name
                    end

                    WebService:SendMails(0, self.userId, MailType.LevelUp, title, data, "g1031")
                end
            end

            self.level = self.level + 1
            self.exp = self.exp + exp - needExp
            self:sendRanchUnlockItems()

            local taskNum = TasksLevelConfig:getNumByLevel(self.level)
            if taskNum > 0 then
                self.task:startTaskOperation()
                self.task:initOriginalTasks(taskNum, self.level)
                local ranchTask = self.task:initRanchTask()
                self:getRanch():setOrders(ranchTask)
            end

            local nextNeedExp = PlayerLevelConfig:getExpByLevel(self.level)
            if self.exp >= nextNeedExp then
                self:addExp(0)
            end
        else
            self.exp = self.exp + exp
        end
    else
        self.exp = self.exp + exp
    end

    local ranchInfo = self:initRanchInfo()
    self:getRanch():setInfo(ranchInfo)

end

function GamePlayer:addAchievementPoint(achievementPoint)
    if achievementPoint > 0 then
        self.achievementPoint = self.achievementPoint + achievementPoint
        self:saveAchievementRankData(achievementPoint)
    end
end

function GamePlayer:getLevel()
    return self.level
end

function GamePlayer:getExp()
    return self.exp
end

function GamePlayer:sendRanchUnlockItems()
    local data = PlayerLevelConfig:getUnlockItemsByLevel(self.level)
    if data == nil then
        return false
    end

    local items = {}
    local index = 1
    for i, v in pairs(data) do
        if v ~= 0 then
            items[index] = v
            index = index + 1
        end
    end

    HostApi.sendRanchUnlockItem(self.rakssid, items)
end

function GamePlayer:initRanchInfo()
    local ranchInfo = RanchInfo.new()
    ranchInfo.index = self.level
    ranchInfo.level = self.level
    ranchInfo.giftNum = self.giftNum
    ranchInfo.sendGiftCount = self.sendGiftCount
    ranchInfo.taskStartLevel = self.taskStartLevel
    ranchInfo.exploreStartLevel = self.exploreStartLevel
    ranchInfo.exp = self:getExp()
    ranchInfo.achievementPoint = self.achievementPoint
    ranchInfo.nextExp = PlayerLevelConfig:getExpByLevel(self.level)
    ranchInfo.prosperity = 0
    ranchInfo.isCanFlying = self.isCanFlying
    if self.building ~= nil and self.building.prosperity ~= nil then
        ranchInfo.prosperity = self.building.prosperity
    end
    ranchInfo.prosperityRank = self.myProsperityRank
    ranchInfo.achievementRank = self.myAchievementRank
    ranchInfo.clanRank = self.myClanRank
    ranchInfo.startPos = VectorUtil.newVector3(0, 0, 0)
    ranchInfo.endPos = VectorUtil.newVector3(0, 0, 0)
    ranchInfo.isDestroyHouse = true
    ranchInfo.isDestroyWarehouse = true

    local startPos = ManorConfig:getStartPosByManorIndex(self.manorIndex)
    if startPos ~= nil then
        ranchInfo.startPos = VectorUtil.newVector3(startPos.x, startPos.y, startPos.z)
    end

    local endPos = ManorConfig:getEndPosByManorIndex(self.manorIndex)
    if endPos ~= nil then
        ranchInfo.endPos = VectorUtil.newVector3(endPos.x, endPos.y, endPos.z)
    end

    if self.wareHouse ~= nil and self.wareHouse.entityId ~= 0 then
        ranchInfo.isDestroyWarehouse = false
    end

    if self.house ~= nil and self.house.isInWorld == true then
        ranchInfo.isDestroyHouse = false
    end

    self.prosperity = ranchInfo.prosperity
    return ranchInfo
end

function GamePlayer:initMailsInfo(items)
    local mails = {}
    local index = 1
    for _, item in pairs(items) do
        local mailInfo = MailInfo.new()
        mailInfo.id = item.id
        mailInfo.toUser = item.toUser
        mailInfo.fromUser = item.fromUser
        mailInfo.mailType = item.mailType
        mailInfo.title = item.title
        mailInfo.content = item.content
        mails[index] = mailInfo
        index = index + 1
    end
    return mails
end

function GamePlayer:savePlayerData(immediate)
    if self.manorIndex == 0 then
        return false
    end

    for i, v in pairs(SceneManager.errorTarget) do
        if tostring(self.userId) == tostring(v.userId) then
            return false
        end
    end

    if not DataBaseManager:isGetAllDataFinished(self) then
        return false
    end

    -- save manor area
    self.manor:setName(self.name)
    local manorData = self.manor:prepareDataSaveToDB()
    DataBaseManager:saveManorData(self.userId, manorData, immediate)

    -- save building
    local buildingData = self.building:prepareDataSaveToDB()
    DataBaseManager:saveBuildingData(self.userId, buildingData, immediate)

    -- save house
    local houseData = self.house:prepareDataSaveToDB()
    DataBaseManager:saveHoseData(self.userId, houseData, immediate)

    -- save player info (inventory, money, level, exp, fieldLevel, currentFieldNum, prosperity)
    local playerData = self:prepareDataSaveToDB()
    DataBaseManager:savePlayerData(self.userId, playerData, immediate)

    -- save wareHouse
    local wareHouseData = self.wareHouse:prepareDataSaveToDB()
    DataBaseManager:saveWareHouseData(self.userId, wareHouseData, immediate)

    -- save field
    local fieldData = self.field:prepareDataSaveToDB()
    DataBaseManager:saveFieldData(self.userId, fieldData, immediate)

    -- save tasks
    if self.task:getTaskOperationStatus() == true then
        local taskData = self.task:prepareDataSaveToDB()
        DataBaseManager:saveTaskData(self.userId, taskData, immediate)
    end

    -- save achievement
    local achievementData = self.achievement:prepareDataSaveToDB()
    DataBaseManager:saveAchievementData(self.userId, achievementData, immediate)

    -- save explore data
    DataBaseManager:saveRanchExploreData(self.userId, self.achievement.exploreAchievementData)

    -- save accelerate items data
    local accelerateItems = self.accelerateItems:prepareDataSaveToDB()
    DataBaseManager:saveAccelerateItemsData(self.userId, accelerateItems, immediate)

    -- save roads data
    local roadsData = self.field:prepareRoadsDataSaveToDB()
    DataBaseManager:saveRoadsData(self.userId, roadsData, immediate)

end

function GamePlayer:reward()
    RewardManager:getUserGoldReward(self.userId, self.score, function(data)

    end)
end

function GamePlayer:onQuit()
    if self.building ~= nil then
        HostApi.ZRemove(RanchersRankManager:getProsperityKey(), tostring(self.userId))
        HostApi.ZIncrBy(RanchersRankManager:getProsperityKey(), tostring(self.userId), self.building.prosperity)
        RanchersWeb:updateLandInfo(self.userId, self.exp, self.giftNum, self.level, self.building.prosperity)
    end
end

function GamePlayer:getPlayerRanks()
    local prosperityKey = RanchersRankManager:getProsperityKey()
    local achievementKey = RanchersRankManager:getAchievementKey()
    local uid = tostring(self.userId)
    HostApi.ZScore(prosperityKey, uid)
    HostApi.ZScore(achievementKey, uid)
end

function GamePlayer:setAchievementRankData(rank, score)
    self.myAchievementRank = rank
    self.isCacheAchievementRank = true
    self:buildingShowName()

    if rank <= 3 and rank > 0 then
        self:sendMsgToAllServer("ranch_boardCast_achievement", GameConfig.boardCastStayTime, GameConfig.boardCastRollTime)
    end
end

function GamePlayer:setProsperityRankData(rank, score)
    self.myProsperityRank = rank
    self.isCacheProsperityRank = true
    self:buildingShowName()

    if rank <= 3 and rank > 0 then
        self:sendMsgToAllServer("ranch_boardCast_prosperity", GameConfig.boardCastStayTime, GameConfig.boardCastRollTime)
    end
end

function GamePlayer:setClanRankData(rank)
    self.myClanRank = rank
    self.isCacheClanRank = true
    self:buildingShowName()
end

function GamePlayer:buildingShowName()
    if self.isCacheClanRank and self.isCacheProsperityRank and self.isCacheAchievementRank then
        local nameList = {}

        if self.myClanRank <= 10 and self.myClanRank > 0 then
            if self.staticAttr.role ~= -1 then
                local clanTitle = TextFormat.colorBlue .. self.staticAttr.clanName
                if self.staticAttr.role == 0 then
                    clanTitle = clanTitle .. TextFormat.colorWrite .. "[Member]"
                end
                if self.staticAttr.role == 10 then
                    clanTitle = clanTitle .. TextFormat.colorRed .. "[Elder]"
                end
                if self.staticAttr.role == 20 then
                    clanTitle = clanTitle .. TextFormat.colorOrange .. "[Chief]"
                end
                table.insert(nameList, clanTitle)
            end
        end

        -- pureName line
        local disName = self.name

        if self.myAchievementRank <= 10 and self.myAchievementRank > 0 then
            disName = TextFormat.colorOrange .. self.name
        end

        PlayerIdentityCache:buildNameList(self.userId, nameList, disName)

        self:setShowName(table.concat(nameList, "\n"))

        local ranchInfo = self:initRanchInfo()
        self:getRanch():setInfo(ranchInfo)
        self.isCacheAchievementRank = false
        self.isCacheProsperityRank = false
        self.isCacheClanRank = false
    end
end

function GamePlayer:sendMsgToAllServer(msg, stayTime, rollTime)
    local content = {
        msg = msg,
        name = self.name,
        stayTime = stayTime * 1000,
        rollTime = rollTime * 1000
    }
    WebService:SendMsg(nil, json.encode(content), 3, 'game', "g1031")
end

function GamePlayer:addScore(score)
    self.score = self.score + score
end

return GamePlayer

--endregion


