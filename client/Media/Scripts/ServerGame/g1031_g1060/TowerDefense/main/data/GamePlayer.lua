GamePlayer = class("GamePlayer", BaseGamePlayer)

function GamePlayer:init()
    self.super:init()
    self.Wallet = GameWallet.new(self:getEntityPlayer():getWallet(), self.rakssid)
    self.Wallet:setMoneyCount(Define.Money.HallGold, 0)
    self.loginTime = os.time()
    self.gameStartTime = os.time()
    self:initStaticAttr()
    self:initAttr()
    self:teleportPos(GameConfig.initPos)
    self:setAllowFlying(true)
    self:initExchangeSetting()
end

function GamePlayer:initAttr()
    --单局游戏数据
    self.killNum = 0
    self.missNum = 0
    self.passRound = 0
    self.totalMoney = 0
    self.pathList = {}
    self.paySkillOffList = {}
    self.formulaList = {}
    self.exchangeLimit = 0

    --Db Data

    self.level = 1
    self.cur_exp = 0
    self.rewardExp = 0
    self.equipSkinId = 0

    self.starTowerList = {}
    self.checkPointRecord = {}
    self.hallInfo = {}
    self.gameReward = {}
    self.towerList = {}     --塔
    self.skinList = {}      --皮肤
    self.payItemList = {}   --特权

    self.killMonsterTotal = 0
    self.killMonsterWeek = 0
    self.weekKey = 0
end

function GamePlayer:initStarTowerList(towerList)
    for _, tower in pairs(towerList) do
        local towerConfig = TowerConfig:getConfigById(tower.towerId)
        if towerConfig then
            self.starTowerList[tostring(towerConfig.StarLevel)] = self.starTowerList[tostring(towerConfig.StarLevel)] or {}
            table.insert(self.starTowerList[tostring(towerConfig.StarLevel)], tower)
        end
    end
end

function GamePlayer:initPaySkillList(skillList)
    for _, skillId in pairs(skillList) do
        local skillConfig = SkillStoreConfig:getConfigById(tonumber(skillId))
        self.paySkillOffList[skillId] = {
            price = skillConfig.price,
            off = TableUtil.copyTable(skillConfig.off),
            prop = skillConfig.prop,
            onUse = false,
        }
    end
    GamePacketSender:sendTowerPaySkillList(self.rakssid, skillList)
end

function GamePlayer:onBuyPaySkill(skillId, cd)
    if self.paySkillOffList[skillId] ~= nil then
        if self.paySkillOffList[skillId].off ~= nil and #self.paySkillOffList[skillId].off >= 1 then
            table.remove(self.paySkillOffList[skillId].off, 1)
        end
        LuaTimer:schedule(function()
            self.paySkillOffList[skillId].onUse = false
        end, cd)
    end
end

function GamePlayer:getTowerByStar(starLevel)
    return self.starTowerList[tostring(starLevel)]
end

function GamePlayer:tryPlaceTower(pos, config)
    local process = ProcessManager:getProcessByProcessId(self.processId)
    if not process then
        ---TODO 需要检查一下逻辑 @yanzhuolin
        LogUtil.log("player try place tower failed, progress is nil. progressId=" .. tostring(self.processId),
                LogUtil.LogLevel.Error)
        return
    end
    if process.status == Define.GameStatus.GameWaitPlayer then
        return
    end

    for i, towerId in pairs(GameConfig.initTowers) do
        if self.payItemList and (self.payItemList[tostring(Define.PayStoreItem.ShortcutBarOne)] or self.payItemList[tostring(Define.PayStoreItem.ShortcutBarTwo)]) then
            break
        end
        if config.Id == towerId then
            break
        end
        if i == #GameConfig.initTowers then
            return
        end
    end

    local posV3 = VectorUtil.toVector3(pos)
    posV3.y = posV3.y + 1
    posV3.x = posV3.x + 0.5
    posV3.z = posV3.z + 0.5

    if not process.placeLockGrid[VectorUtil.hashVector3(posV3)] and self:canPlaceTower(pos, config.CoinId, config.BuildPrice, self.processId) then
        if self.Wallet:costMoney(config.CoinId, config.BuildPrice) then
            if not process.placeLockGrid[VectorUtil.hashVector3(posV3)] then
                --放出精灵球
                local builder = DataBuilder.new()
                builder:addParam("yaw", self:getBoxYaw(posV3))
                builder:addParam("userId", self.userId)
                builder:addVector3Param("pos", posV3)
                local data = builder:getListData()
                if process then
                    process:broadCastPacket("CreateBox", data)
                end
                --延迟放置放置塔
                process.placeLockGrid[VectorUtil.hashVector3(posV3)] = 1
                LuaTimer:schedule(function(posV3, config)
                    if process ~= nil then
                        process.placeLockGrid[VectorUtil.hashVector3(posV3)] = nil
                    end
                    self:onPlaceTower(posV3, config)

                end, 1150, nil, posV3, config)
            end
        end
    end
end

function GamePlayer:onPlaceTower(pos, config)
    local yaw = self:getYaw() + 180
    if yaw > 360 then
        yaw = yaw - 360
    end
    local facePos = self:getPosition()
    local Tower = GameTower.new(pos, config, tostring(self.userId), facePos, self.processId)
    self:onPlaceTowerSuccess(config.Id)
    LuaTimer:schedule(function()
        if not self:checkReady() then
            return
        end
        if self.formulaList["Start" .. Tower.config.StarLevel] == nil then
            self.formulaList["Start" .. Tower.config.StarLevel] = {}
        end
        table.insert(self.formulaList["Start" .. Tower.config.StarLevel], Tower)
        self:checkTowerFormulaByStarLevel(Tower.config.StarLevel)
    end, GameConfig.CheckFormulaInterval, nil)
end

function GamePlayer:checkTowerFormulaByStarLevel(StarLevel)
    --检查某一星级的塔是否能合成玩家已拥有的更高级的塔
    local process = ProcessManager:getProcessByProcessId(self.processId)
    if not process then
        LogUtil.log("checkTowerFormulaByStarLevel failed, process = nil.", LogUtil.LogLevel.Error)
        return
    end
    local higherStarTowerList = self:getTowerByStar(StarLevel + 1)
    local formulaList = self.formulaList["Start" .. StarLevel]
    if not higherStarTowerList or not formulaList or #formulaList < 3 then
        return
    end

    for _, targetTower in pairs(higherStarTowerList) do
        local formulaConfig = TowerFormulaConfig:getConfigByTowerId(targetTower.towerId)
        if formulaConfig ~= nil then
            local formulaItem = {
                itemA = 0,
                itemB = 0,
                itemC = 0,
            }
            for _, tower in pairs(formulaList) do
                local towerClassId = tower.config.ClassId
                if towerClassId == formulaConfig.ClassAId and formulaItem.itemA == 0 then
                    formulaItem.itemA = tower
                elseif towerClassId == formulaConfig.ClassBId and formulaItem.itemB == 0 then
                    formulaItem.itemB = tower
                elseif towerClassId == formulaConfig.ClassCId and formulaItem.itemC == 0 then
                    formulaItem.itemC = tower
                end

                if formulaItem.itemA ~= 0 and formulaItem.itemB ~= 0 and formulaItem.itemC ~= 0 then
                    --符合条件，移除旧塔，建新塔
                    local newConfig = TowerConfig:getConfigById(targetTower.towerId)
                    local newPos = tower.pos
                    formulaItem.itemC:onDie()
                    formulaItem.itemB:onDie()
                    formulaItem.itemA:onDie()
                    process.placeLockGrid[VectorUtil.hashVector3(newPos)] = 1

                    LuaTimer:schedule(function()
                        if not self:checkReady() then
                            return
                        end
                        if process ~= nil then
                            process.placeLockGrid[VectorUtil.hashVector3(newPos)] = nil
                        end
                        self:onPlaceTower(newPos, newConfig)
                        self:checkTowerFormulaByStarLevel(StarLevel)
                    end, GameConfig.FormulaTowerToBuildInterval, nil)
                    break
                end
            end
            if #formulaList < 3 then
                break
            end
        end
    end
end

function GamePlayer:onPlaceTowerSuccess(towerId)
    local tower = self.towerList[towerId]
    local process = ProcessManager:getProcessByProcessId(self.processId)
    if not process then
        ---TODO @yanzhuolin 检查一下原因
        LogUtil.log("player place tower failed, process = nil!", LogUtil.LogLevel.Error)
        return
    end
    ReportUtil.reportPlayerBuildTower(self, towerId, process.checkPointConfig.id, process.roundNum)
    if tower then
        if tower.isHave == true and tower.isHaveBook == false then
            self:addNewTowerBook(towerId)
        end
        tower.buildTimes = tower.buildTimes + 1
    end
end

function GamePlayer:addNewTower(towerId)
    local tower = {
        towerId = towerId,
        isHave = true,
        isHaveBook = false,
        openBookTimes = 0,
        isNew = true,
        buildTimes = 0,
    }
    self.towerList[towerId] = tower
    local config = TowerConfig:getConfigById(tower.towerId)

    if self.starTowerList[tostring(config.StarLevel)] == nil then
        self.starTowerList[tostring(config.StarLevel)] = {}
    end
    table.insert(self.starTowerList[tostring(config.StarLevel)], tower)
    self:checkTowerFormulaByStarLevel(config.StarLevel - 1)
    return true
end

function GamePlayer:addNewTowerBook(towerId)
    if self.towerList[towerId] == nil then
        return false
    end
    self.towerList[towerId].isHaveBook = true
    GamePacketSender:sendTowerDefenseGetNewTowerBook(self.rakssid, self.towerList[towerId])
end

function GamePlayer:onOpenBookDetail(towerId)
    if self.towerList[towerId] and self.towerList[towerId].isHaveBook then
        self.towerList[towerId].isNew = false
        self.towerList[towerId].openBookTimes = self.towerList[towerId].openBookTimes + 1
    end
    ReportUtil.reportClickGameBookItem(self, towerId)
end

function GamePlayer:addFreeTowerToList()
    local freeTowers = TowerConfig:getFreeTowerList()
    for _, tower in pairs(freeTowers) do
        if self.towerList[tower.Id] == nil then
            self:addNewTower(tower.Id)
        end
    end
end

function GamePlayer:killMonster(goldNum)
    self.killNum = self.killNum + 1
    self.totalMoney = self.totalMoney + goldNum
    self.Wallet:addMoney(Define.Money.GameGold, goldNum)
end

function GamePlayer:setProcess(processId)
    self.entityPlayerMP:setIntProperty("TrackerGroupId", processId)
    self.processId = processId
    GamePacketSender:sendTowerDefenseProcessId(self.rakssid, processId)
end

function GamePlayer:overGame()
    HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
end

function GamePlayer:rewardPlayer()
    --记录最大通关波数
    local oldMaxRound = self.checkPointRecord[tostring(self.hallInfo.checkPointId or "0")] or 0
    if oldMaxRound < self.passRound then
        self.checkPointRecord[tostring(self.hallInfo.checkPointId or "0")] = self.passRound
    end
    --给玩家发平台奖励
    RewardManager:getUserGoldReward(self.userId, self:getGoldNum(), function()
    end)
end

function GamePlayer:onPlayAgainInit()
    self.Wallet:initGameMoney()
    self:initExchangeSetting()

    --- 更新统计的杀怪数量
    local weekKey = DateUtil.getYearWeek()
    self.killMonsterTotal = self.killMonsterTotal + self.killNum
    if self.weekKey == weekKey then
        self.killMonsterWeek = self.killMonsterWeek + self.killNum
    else
        self.killMonsterWeek = self.killNum
        self.weekKey = weekKey
    end

    self.killNum = 0
    self.missNum = 0
    self.totalMoney = 0
    self.paySkillOffList = {}

    self.formulaList = {
        Start1 = {},
        Start2 = {},
        Start3 = {},
        Start4 = {},
        Start5 = {}
    }
end

function GamePlayer:addPath(path)
    table.insert(self.pathList, path)
end

function GamePlayer:onLogout()
    --- 更新统计的杀怪数量
    local weekKey = DateUtil.getYearWeek()
    self.killMonsterTotal = self.killMonsterTotal + self.killNum
    if self.weekKey == weekKey then
        self.killMonsterWeek = self.killMonsterWeek + self.killNum
    else
        self.killMonsterWeek = self.killNum
        self.weekKey = weekKey
    end

    self:rewardPlayer()
    for _, path in pairs(self.pathList) do
        if path ~= nil then
            path:onPlayerLogout(self)
        end
    end
    self.pathList = nil

    ReportUtil.reportPlayerGameTime(self)
end

function GamePlayer:getBoxYaw(placePos)
    local pos = self:getPosition()
    local vector = math.atan2(pos.x - placePos.x, pos.z - placePos.z)
    return math.floor(vector / math.pi * -180)
end

function GamePlayer:buildShowName()
    local nameList = {}
    local namePrefix = TextFormat.colorGreen .. "[Lv." .. tostring(self.level) .. "] "
    local disName = self.name
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName, namePrefix)
    return table.concat(nameList, "\n")
end

function GamePlayer:updateShowName()
    self:setShowName(self:buildShowName())
    self.clientPeer:setShowName(" ", self.rakssid)
end

function GamePlayer:initSkin()
    if self.equipSkinId ~= 0 then
        self:equipSkin(self.equipSkinId)
        return
    end
    local sex = self:getSex()
    if sex == Define.Sex.Boy then
        self:onGetNewSkin(GameConfig.defaultBoySkin)
    else
        self:onGetNewSkin(GameConfig.defaultGirlSkin)
    end
end

function GamePlayer:equipSkin(skinId)
    local config = SkinStoreConfig:getConfigById(skinId)
    if config and self.skinList[tostring(skinId)] and self.skinList[tostring(skinId)].isHave == true then
        self:changeActor(config.actorName)
        self:addItem(2801, 1)
        self.equipSkinId = skinId
    end
    GamePacketSender:sendEquipSkin(self)
    self.clientPeer:setShowName(" ", self.rakssid)
end

function GamePlayer:onGetNewSkin(skinId)
    local config = SkinStoreConfig:getConfigById(skinId)
    if config then
        local skin = {
            id = skinId,
            isHave = true,
        }
        self.skinList = self.skinList or {}
        self.skinList[tostring(skinId)] = skin
    end
    self:equipSkin(skinId)
end

function GamePlayer:getGoldNum(isTest)
    if isTest then
        local gameTime = math.floor((os.time() - self.gameStartTime) * 0.1) * math.max(self.vip, 1)
        self.gameStartTime = os.time()
        return gameTime
    else
        return math.floor((os.time() - self.inGameTime) * 0.1) * math.max(self.vip, 1)
    end
end

function GamePlayer:initExchangeSetting()
    local checkPointConfig = CheckPointConfig:getCheckPointConfigById(self.hallInfo.checkPointId or 1)
    self.exchangeLimit = checkPointConfig.exchangeLimit
    GamePacketSender:sendExchangeRate(self)
    GamePacketSender:sendGameGoldExchangeRate(self, checkPointConfig.exchangeLimit)
end

function GamePlayer:reportRecord()
    ---上报杀怪总榜
    MaxRecordHelper:reportOneRecord(Define.RankKey.KillMonsterTotal, tostring(self.userId), self.killMonsterTotal * 1000 + self.level)
    ---上报杀怪周榜
    MaxRecordHelper:reportOneRecord(Define.RankKey.KillMonsterWeek .. DateUtil.getYearWeek(), tostring(self.userId), self.killMonsterWeek * 1000 + self.level)
    ---上报图鉴总榜
    local towerBookNum = 0
    for _, tower in pairs(self.towerList) do
        if tower.isHaveBook then
            towerBookNum = towerBookNum + 1
        end
    end
    MaxRecordHelper:reportOneRecord(Define.RankKey.TowerBook, tostring(self.userId), towerBookNum * 1000 + self.level)
end

function GamePlayer:addExp(exp)
    self.rewardExp = self.rewardExp + exp
end

return GamePlayer